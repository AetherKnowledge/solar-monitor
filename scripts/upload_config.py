import os
import subprocess
import sys
import tempfile
import shutil

from SCons.Script import Import

Import("env")

PROJECT_DIR = env.subst("$PROJECT_DIR")

CONFIG_FILE = os.path.join(PROJECT_DIR, "config.json")

# Must match partitions.csv
STORAGE_OFFSET = "0x350000"
STORAGE_SIZE = 0xA0000  # 640 KiB

UPLOAD_PORT = env.subst("$UPLOAD_PORT")
UPLOAD_SPEED = env.subst("$UPLOAD_SPEED")

MKLITTLEFS = env.PioPlatform().get_package_dir("tool-mklittlefs")
ESPTOOL = env.PioPlatform().get_package_dir("tool-esptoolpy")

if os.name == "nt":
    MKLITTLEFS_BIN = os.path.join(MKLITTLEFS, "mklittlefs.exe")
else:
    MKLITTLEFS_BIN = os.path.join(MKLITTLEFS, "mklittlefs")

ESPTOOL_PY = os.path.join(
    ESPTOOL,
    "esptool.py"
)


def upload_config(source, target, env):
    if not os.path.isfile(CONFIG_FILE):
        print()
        print(f"ERROR: config.json not found:")
        print(CONFIG_FILE)
        print()

        sys.exit(1)

    print()
    print("Uploading config.json to storage LittleFS")
    print(f"Source: {CONFIG_FILE}")
    print(f"Storage offset: {STORAGE_OFFSET}")
    print(f"Storage size: {STORAGE_SIZE} bytes")
    print()

    with tempfile.TemporaryDirectory() as temp_dir:
        # The root of this temporary directory becomes
        # the root of the storage LittleFS partition.
        destination = os.path.join(
            temp_dir,
            "config.json"
        )

        shutil.copy2(
            CONFIG_FILE,
            destination
        )

        image_file = os.path.join(
            temp_dir,
            "storage.bin"
        )

        print("Building storage LittleFS image...")

        subprocess.check_call([
            MKLITTLEFS_BIN,
            "-c",
            temp_dir,
            "-s",
            str(STORAGE_SIZE),
            image_file
        ])

        print()
        print("Flashing storage partition...")

        command = [
            sys.executable,
            ESPTOOL_PY,
            "--chip",
            "esp32",
            "--port",
            UPLOAD_PORT,
        ]

        if UPLOAD_SPEED:
            command.extend([
                "--baud",
                UPLOAD_SPEED
            ])

        command.extend([
            "write_flash",
            STORAGE_OFFSET,
            image_file
        ])

        subprocess.check_call(command)

    print()
    print("Config uploaded successfully.")
    print()


env.AddCustomTarget(
    name="uploadconfig",
    dependencies=None,
    actions=upload_config,
    title="Upload Config",
    description="Build and upload config.json to the storage LittleFS partition"
)