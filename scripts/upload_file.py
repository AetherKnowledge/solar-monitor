import csv
import datetime
import json
import os
import shutil
import subprocess
import sys
import tempfile

from SCons.Script import Import

Import("env")

PROJECT_DIR = env.subst("$PROJECT_DIR")

UPLOAD_PORT = env.subst("$UPLOAD_PORT")
UPLOAD_SPEED = env.subst("$UPLOAD_SPEED")

PIOENV = env.subst("$PIOENV")

BUILD_DIR = os.path.join(
    PROJECT_DIR,
    ".pio",
    "build",
    PIOENV,
)

WEBSITE_IMAGE = os.path.join(BUILD_DIR, "website.bin")

PARTITIONS_FILE = env.BoardConfig().get("build.partitions")

if not PARTITIONS_FILE:
    raise RuntimeError("board_build.partitions is not configured.")

if not os.path.isabs(PARTITIONS_FILE):
    PARTITIONS_FILE = os.path.join(PROJECT_DIR, PARTITIONS_FILE)

DATA_DIR = env.subst("$PROJECT_DATA_DIR")

FILES = [
    "config.json",
    "persistence.json",
]

ESPTOOL = env.PioPlatform().get_package_dir("tool-esptoolpy")

ESPTOOL_PY = os.path.join(ESPTOOL, "esptool.py")
PARTITION_TOOL = os.path.join(
    env.PioPlatform().get_package_dir("framework-arduinoespressif32"),
    "tools",
    "gen_esp32part.py",
)


def get_chip():
    mcu = env.BoardConfig().get("build.mcu")

    if mcu == "esp32":
        return "esp32"

    if mcu == "esp32s3":
        return "esp32s3"

    raise RuntimeError(f"Unsupported MCU: {mcu}")


def get_partition(name: str, partitions_file=PARTITIONS_FILE):
    if not os.path.isfile(partitions_file):
        raise FileNotFoundError(partitions_file)

    with open(partitions_file, newline="") as f:
        reader = csv.reader(f)

        for row in reader:
            if not row:
                continue

            if row[0].strip().startswith("#"):
                continue

            if len(row) < 5:
                continue

            if row[0].strip() == name:
                size = row[4].strip().upper()
                multiplier = {"K": 1024, "M": 1024 * 1024}.get(size[-1:], 1)
                if multiplier != 1:
                    size = size[:-1]
                return row[3].strip(), int(size, 0) * multiplier

    raise RuntimeError(f"Partition '{name}' not found")


def esptool_command(chip):
    command = [
        sys.executable,
        ESPTOOL_PY,
        "--chip",
        chip,
        "--port",
        UPLOAD_PORT,
    ]

    if UPLOAD_SPEED:
        command.extend([
            "--baud",
            UPLOAD_SPEED,
        ])

    return command


def flash_image(chip, offset, image):
    command = esptool_command(chip)
    command.extend([
        "write_flash",
        offset,
        image,
    ])

    subprocess.check_call(command)


def read_flash(chip, offset, size, image):
    subprocess.check_call(esptool_command(chip) + [
        "read-flash",
        hex(offset),
        hex(size),
        image,
    ])


def littlefs_image(size, context=None):
    from littlefs import LittleFS

    if size <= 0 or size % 4096:
        raise ValueError("LittleFS image size must be a positive multiple of 4096.")
    return LittleFS(
        context=context,
        block_size=4096,
        block_count=size // 4096,
        read_size=1,
        prog_size=1,
        cache_size=4096,
        lookahead_size=32,
        name_max=64,
        disk_version=0x00020001,
        mount=False,
    )


def extract_littlefs(image, destination):
    from littlefs import UserContext

    with open(image, "rb") as file:
        data = bytearray(file.read())
    fs = littlefs_image(len(data), UserContext(buffer=data))
    # Never format a backup image when mounting fails.
    fs.mount()
    try:
        for filename in FILES:
            with fs.open("/" + filename, "rb") as source:
                content = source.read()
            with open(os.path.join(destination, filename), "wb") as target:
                target.write(content)
    finally:
        fs.unmount()


def backup_storage(source, target, env):
    chip = get_chip()
    backup_dir = os.path.join(PROJECT_DIR, "backup")
    os.makedirs(backup_dir, exist_ok=True)

    print("Reading saved configuration and persistence over USB.")
    print("Close the serial monitor first. The device will reset during backup.")

    temp_dir = tempfile.mkdtemp(prefix=f"raw-{PIOENV}-", dir=backup_dir)
    print(f"Raw backup files will be kept in {temp_dir}")
    try:
        table_bin = os.path.join(temp_dir, "partitions.bin")
        table_csv = os.path.join(temp_dir, "partitions.csv")
        storage_image = os.path.join(temp_dir, "storage.bin")

        read_flash(chip, 0x8000, 0x1000, table_bin)
        subprocess.check_call([
            sys.executable, PARTITION_TOOL, table_bin, table_csv,
        ])
        offset, size = get_partition("storage", table_csv)
        if size <= 0 or size % 0x1000:
            raise RuntimeError("Device storage partition has an invalid size.")

        print(f"Device storage: {offset}, {size} bytes")
        read_flash(chip, int(offset, 0), size, storage_image)
        extract_littlefs(storage_image, temp_dir)

        for filename in FILES:
            file_path = os.path.join(temp_dir, filename)
            with open(file_path, encoding="utf-8") as file:
                if not isinstance(json.load(file), dict):
                    raise RuntimeError(f"{filename} must contain a JSON object.")

        timestamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y%m%dT%H%M%S%fZ")
        destination = os.path.join(backup_dir, f"{PIOENV}-{timestamp}")
        os.rename(temp_dir, destination)
    except Exception:
        print(f"Backup failed. Raw files retained in {temp_dir}")
        raise

    print(f"Backup saved to {destination}")


def build_littlefs(source_dir, size, output):
    fs = littlefs_image(size)
    fs.format()
    fs.mount()
    try:
        for directory, _, filenames in os.walk(source_dir):
            relative = os.path.relpath(directory, source_dir).replace(os.sep, "/")
            fs_directory = "" if relative == "." else relative
            if fs_directory:
                fs.makedirs(fs_directory, exist_ok=True)
            for filename in filenames:
                fs_path = "/".join(filter(None, [fs_directory, filename]))
                with open(os.path.join(directory, filename), "rb") as source:
                    with fs.open(fs_path, "wb") as target:
                        target.write(source.read())
    finally:
        fs.unmount()
    with open(output, "wb") as file:
        file.write(fs.context.buffer)


def upload_storage(source, target, env):
    chip = get_chip()
    offset, storage_size = get_partition("storage")

    print()
    print("Uploading storage filesystem")
    print(f"Offset : {offset}")
    print(f"Size   : {hex(storage_size)}")
    print()

    with tempfile.TemporaryDirectory() as temp_dir:
        for filename in FILES:
            src = os.path.join(PROJECT_DIR, filename)

            if not os.path.isfile(src):
                raise FileNotFoundError(src)

            shutil.copy2(src, os.path.join(temp_dir, filename))

        image = os.path.join(temp_dir, "storage.bin")

        print("Building LittleFS image...")
        build_littlefs(temp_dir, storage_size, image)

        print("Flashing...")
        flash_image(chip, offset, image)

    print()
    print("Storage uploaded successfully.")
    print()


def build_website(source, target, env):
    if not os.path.isdir(DATA_DIR):
        raise RuntimeError(f"Data directory not found: {DATA_DIR}")

    _, website_size = get_partition("website")

    print()
    print("Building website filesystem")
    print(f"Source : {DATA_DIR}")
    print(f"Output : {WEBSITE_IMAGE}")
    print(f"Size   : {hex(website_size)}")
    print()

    os.makedirs(BUILD_DIR, exist_ok=True)

    build_littlefs(
        DATA_DIR,
        website_size,
        WEBSITE_IMAGE,
    )

    print()
    print("Website image built successfully.")
    print()


def upload_web(source, target, env):
    chip = get_chip()
    offset, _ = get_partition("website")

    if not os.path.isfile(WEBSITE_IMAGE):
        raise RuntimeError(
            f"Website image not found:\n\n"
            f"    {WEBSITE_IMAGE}\n\n"
            f"Run:\n"
            f"    pio run -t buildwebsite"
        )

    print()
    print("Uploading website filesystem")
    print(f"Image  : {WEBSITE_IMAGE}")
    print(f"Offset : {offset}")
    print()

    flash_image(
        chip,
        offset,
        WEBSITE_IMAGE,
    )

    print()
    print("Website uploaded successfully.")
    print()


env.AddCustomTarget(
    name="backup",
    dependencies=None,
    actions=backup_storage,
    title="Backup Storage",
    description="Read device config and persistence into backup/ over USB",
)

env.AddCustomTarget(
    name="uploadstorage",
    dependencies=None,
    actions=upload_storage,
    title="Upload Storage",
    description="Build and upload storage filesystem",
)

env.AddCustomTarget(
    name="buildweb",
    dependencies=None,
    actions=build_website,
    title="Build Website",
    description="Build website LittleFS image",
)

env.AddCustomTarget(
    name="uploadweb",
    dependencies=None,
    actions=upload_web,
    title="Upload Website",
    description="Upload website LittleFS image",
)
