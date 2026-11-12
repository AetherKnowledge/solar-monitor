import contextlib
import io
import importlib.util
import json
from pathlib import Path
import runpy
import subprocess
import sys
import tempfile
import types
import unittest
from unittest.mock import patch


SCRIPT = Path(__file__).resolve().parents[1] / "upload_file.py"


class BackupTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.table = self.root / "local.csv"
        self.table.write_text("storage,data,0x83,0x3B0000,0x40000,\n")
        self.device_table = "storage,data,0x83,0x350000,640K,\n"
        self.files = {
            "config.json": b'{"site":{"password":"fixture-password"}}',
            "persistence.json": b'{"inverter":{"energy":123.45}}',
        }
        self.board = {"build.partitions": str(self.table), "build.mcu": "esp32"}
        substitutions = {
            "$PROJECT_DIR": str(self.root),
            "$UPLOAD_PORT": "TEST_PORT",
            "$UPLOAD_SPEED": "115200",
            "$PIOENV": "esp32dev",
            "$PROJECT_DATA_DIR": str(self.root / "data"),
        }
        self.targets = {}
        platform = types.SimpleNamespace(
            get_package_dir=lambda name: str(self.root / "tools" / name)
        )
        self.env = types.SimpleNamespace(
            subst=substitutions.__getitem__,
            BoardConfig=lambda: self.board,
            PioPlatform=lambda: platform,
            AddCustomTarget=lambda **target: self.targets.update({target["name"]: target}),
        )
        scons = types.ModuleType("SCons")
        scons_script = types.ModuleType("SCons.Script")
        scons_script.Import = lambda name: None
        with patch.dict(sys.modules, {"SCons": scons, "SCons.Script": scons_script}):
            namespace = runpy.run_path(str(SCRIPT), init_globals={"env": self.env})
        self.backup = namespace["backup_storage"]
        self.extract = namespace["extract_littlefs"]
        self.build = namespace["build_littlefs"]
        self.scope = self.backup.__globals__
        self.calls = []
        self.fail_at = None
        self.scope["subprocess"] = types.SimpleNamespace(check_call=self.command)
        self.scope["extract_littlefs"] = lambda image, destination: self.command(
            ["extract-littlefs", image, destination]
        )

    def command(self, args, **kwargs):
        self.calls.append(args)
        if len(self.calls) == self.fail_at:
            raise subprocess.CalledProcessError(1, args)
        if "read-flash" in args:
            Path(args[-1]).write_bytes(b"device flash fixture")
        elif args[1] == self.scope["PARTITION_TOOL"]:
            Path(args[-1]).write_text(self.device_table)
        elif args[0] == "extract-littlefs":
            destination = Path(args[2])
            for name, content in self.files.items():
                (destination / name).write_bytes(content)
        else:
            self.fail(f"Unexpected subprocess: {args}")

    def run_backup(self):
        with contextlib.redirect_stdout(io.StringIO()):
            self.backup(None, None, self.env)

    def seed_backup(self):
        previous = self.root / "backup" / "previous"
        previous.mkdir(parents=True)
        (previous / "config.json").write_bytes(b"previous config")
        (previous / "persistence.json").write_bytes(b"previous persistence")
        return previous

    def assert_previous_preserved(self, previous):
        published = [p for p in (self.root / "backup").iterdir()
                     if not p.name.startswith("raw-")]
        self.assertEqual(published, [previous])
        self.assertEqual((previous / "config.json").read_bytes(), b"previous config")
        self.assertEqual((previous / "persistence.json").read_bytes(), b"previous persistence")

    def test_reads_device_layout_instead_of_local_layout(self):
        self.run_backup()
        reads = [args for args in self.calls if "read-flash" in args]
        self.assertEqual(len(reads), 2)
        self.assertEqual(reads[0][-3:-1], ["0x8000", "0x1000"])
        self.assertEqual(reads[1][-3:-1], ["0x350000", "0xa0000"])
        self.assertEqual(reads[1][reads[1].index("--port") + 1], "TEST_PORT")
        self.assertFalse(any("write_flash" in args or "write-flash" in args for args in self.calls))
        snapshots = list((self.root / "backup").iterdir())
        self.assertEqual(len(snapshots), 1)
        for name, content in self.files.items():
            self.assertEqual((snapshots[0] / name).read_bytes(), content)

    def test_new_layout_and_s3_layout(self):
        for offset, size, chip in [
            ("0x3B0000", "0x40000", "esp32"),
            ("0xF50000", "0xA0000", "esp32s3"),
        ]:
            with self.subTest(chip=chip):
                self.device_table = f"storage,data,0x83,{offset},{size},\n"
                self.board["build.mcu"] = chip
                self.calls.clear()
                self.run_backup()
                read = [args for args in self.calls if "read-flash" in args][1]
                self.assertEqual(read[-3:-1], [hex(int(offset, 0)), hex(int(size, 0))])
                self.assertEqual(read[read.index("--chip") + 1], chip)

    def test_partition_size_suffixes(self):
        for text, size in [("256K", 0x40000), ("640k", 0xA0000), ("1M", 0x100000)]:
            with self.subTest(size=text):
                self.table.write_text(f"storage,data,0x83,0x3B0000,{text},\n")
                self.assertEqual(self.scope["get_partition"]("storage")[1], size)

    def test_each_command_failure_preserves_existing_backup(self):
        previous = self.seed_backup()
        for stage in range(1, 5):
            with self.subTest(stage=stage):
                self.calls.clear()
                self.fail_at = stage
                with self.assertRaises(subprocess.CalledProcessError):
                    self.run_backup()
                self.assert_previous_preserved(previous)

    def test_missing_or_invalid_files_preserve_existing_backup(self):
        previous = self.seed_backup()
        for content in [None, b"invalid JSON", b"[]"]:
            with self.subTest(content=content):
                self.calls.clear()
                if content is None:
                    self.files.pop("persistence.json", None)
                else:
                    self.files["persistence.json"] = content
                with self.assertRaises((FileNotFoundError, json.JSONDecodeError, RuntimeError)):
                    self.run_backup()
                self.assert_previous_preserved(previous)

    def test_publish_failure_does_not_replace_existing_backup(self):
        previous = self.seed_backup()
        with patch.object(self.scope["os"], "rename", side_effect=OSError("disk full")):
            with self.assertRaises(OSError):
                self.run_backup()
        self.assert_previous_preserved(previous)

    def test_extraction_failure_retains_raw_files(self):
        self.fail_at = 4
        with self.assertRaises(subprocess.CalledProcessError):
            self.run_backup()
        captures = list((self.root / "backup").glob("raw-esp32dev-*"))
        self.assertEqual(len(captures), 1)
        for name in ["partitions.bin", "partitions.csv", "storage.bin"]:
            self.assertTrue((captures[0] / name).is_file(), name)
        self.assertEqual((captures[0] / "storage.bin").read_bytes(), b"device flash fixture")

    def test_missing_or_invalid_storage_partition_stops_before_second_read(self):
        for table in ["website,data,spiffs,0x330000,0x80000,\n",
                      "storage,data,0x83,0x350000,0,\n"]:
            with self.subTest(table=table):
                self.device_table = table
                self.calls.clear()
                with self.assertRaises(RuntimeError):
                    self.run_backup()
                self.assertEqual(len([args for args in self.calls if "read-flash" in args]), 1)
                self.assertTrue(all(p.name.startswith("raw-")
                                    for p in (self.root / "backup").iterdir()))

    @unittest.skipUnless(importlib.util.find_spec("littlefs"), "Requires PlatformIO's littlefs-python")
    def test_extracts_disk_versions_20_and_21_without_changing_dump(self):
        from littlefs import LittleFS

        for version in [0x00020000, 0x00020001]:
            with self.subTest(version=hex(version)):
                fs = LittleFS(block_size=4096, block_count=64, name_max=64,
                              disk_version=version, mount=False)
                fs.format()
                fs.mount()
                for name, content in self.files.items():
                    with fs.open(name, "wb") as file:
                        file.write(content)
                fs.unmount()
                image = self.root / "storage.bin"
                raw = bytes(fs.context.buffer)
                image.write_bytes(raw)
                self.extract(str(image), str(self.root))
                self.assertEqual(image.read_bytes(), raw)
                for name, content in self.files.items():
                    self.assertEqual((self.root / name).read_bytes(), content)

    @unittest.skipUnless(importlib.util.find_spec("littlefs"), "Requires PlatformIO's littlefs-python")
    def test_corrupt_image_is_not_formatted_or_changed(self):
        image = self.root / "corrupt.bin"
        raw = b"\xff" * 0x40000
        image.write_bytes(raw)
        with self.assertRaises(Exception):
            self.extract(str(image), str(self.root))
        self.assertEqual(image.read_bytes(), raw)
        self.assertFalse((self.root / "config.json").exists())

    @unittest.skipUnless(importlib.util.find_spec("littlefs"), "Requires PlatformIO's littlefs-python")
    def test_built_images_round_trip(self):
        from littlefs import LittleFS, UserContext

        source = self.root / "source"
        source.mkdir()
        for name, content in self.files.items():
            (source / name).write_bytes(content)
        nested = source / "_app" / "immutable"
        nested.mkdir(parents=True)
        asset_name = "filename-longer-than-thirty-two-characters.js"
        (nested / asset_name).write_bytes(b"browser asset")
        for size in [0x40000, 0x80000, 0xA0000]:
            with self.subTest(size=size):
                image = self.root / "built.bin"
                self.build(str(source), size, str(image))
                self.assertEqual(image.stat().st_size, size)
                self.extract(str(image), str(self.root))
                for name, content in self.files.items():
                    self.assertEqual((self.root / name).read_bytes(), content)
                fs = LittleFS(context=UserContext(buffer=bytearray(image.read_bytes())),
                              block_size=4096, block_count=size // 4096,
                              name_max=64, mount=False)
                fs.mount()
                try:
                    with fs.open("/_app/immutable/" + asset_name, "rb") as file:
                        self.assertEqual(file.read(), b"browser asset")
                finally:
                    fs.unmount()

    def test_repeated_backups_keep_previous_snapshots(self):
        self.run_backup()
        self.files["persistence.json"] = b'{"inverter":{"energy":200}}'
        self.run_backup()
        snapshots = list((self.root / "backup").iterdir())
        self.assertEqual(len(snapshots), 2)
        values = {json.loads((folder / "persistence.json").read_bytes())["inverter"]["energy"]
                  for folder in snapshots}
        self.assertEqual(values, {123.45, 200})

    def test_target_does_not_build_firmware(self):
        self.assertIsNone(self.targets["backup"]["dependencies"])


if __name__ == "__main__":
    unittest.main()
