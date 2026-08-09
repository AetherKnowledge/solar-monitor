import csv
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

STORAGE_SIZE = 0xA0000
WEBSITE_SIZE = 0x80000

FILES = [
    "config.json",
    "persistence.json",
]

MKLITTLEFS = env.PioPlatform().get_package_dir("tool-mklittlefs")
ESPTOOL = env.PioPlatform().get_package_dir("tool-esptoolpy")

if os.name == "nt":
    MKLITTLEFS_BIN = os.path.join(MKLITTLEFS, "mklittlefs.exe")
else:
    MKLITTLEFS_BIN = os.path.join(MKLITTLEFS, "mklittlefs")

ESPTOOL_PY = os.path.join(ESPTOOL, "esptool.py")


def get_chip():
    mcu = env.BoardConfig().get("build.mcu")

    if mcu == "esp32":
        return "esp32"

    if mcu == "esp32s3":
        return "esp32s3"

    raise RuntimeError(f"Unsupported MCU: {mcu}")


def get_partition_offset(name: str):
    if not os.path.isfile(PARTITIONS_FILE):
        raise FileNotFoundError(PARTITIONS_FILE)

    with open(PARTITIONS_FILE, newline="") as f:
        reader = csv.reader(f)

        for row in reader:
            if not row:
                continue

            if row[0].strip().startswith("#"):
                continue

            if len(row) < 5:
                continue

            if row[0].strip() == name:
                return row[3].strip()

    raise RuntimeError(f"Partition '{name}' not found")


def flash_image(chip, offset, image):
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

    command.extend([
        "write_flash",
        offset,
        image,
    ])

    subprocess.check_call(command)


def build_littlefs(source_dir, size, output):
    subprocess.check_call([
        MKLITTLEFS_BIN,
        "-c",
        source_dir,
        "-s",
        str(size),
        output,
    ])


def upload_storage(source, target, env):
    chip = get_chip()
    offset = get_partition_offset("storage")

    print()
    print("Uploading storage filesystem")
    print(f"Offset : {offset}")
    print(f"Size   : {hex(STORAGE_SIZE)}")
    print()

    with tempfile.TemporaryDirectory() as temp_dir:
        for filename in FILES:
            src = os.path.join(PROJECT_DIR, filename)

            if not os.path.isfile(src):
                raise FileNotFoundError(src)

            shutil.copy2(src, os.path.join(temp_dir, filename))

        image = os.path.join(temp_dir, "storage.bin")

        print("Building LittleFS image...")
        build_littlefs(temp_dir, STORAGE_SIZE, image)

        print("Flashing...")
        flash_image(chip, offset, image)

    print()
    print("Storage uploaded successfully.")
    print()


def build_website(source, target, env):
    if not os.path.isdir(DATA_DIR):
        raise RuntimeError(f"Data directory not found: {DATA_DIR}")

    print()
    print("Building website filesystem")
    print(f"Source : {DATA_DIR}")
    print(f"Output : {WEBSITE_IMAGE}")
    print(f"Size   : {hex(WEBSITE_SIZE)}")
    print()

    os.makedirs(BUILD_DIR, exist_ok=True)

    build_littlefs(
        DATA_DIR,
        WEBSITE_SIZE,
        WEBSITE_IMAGE,
    )

    print()
    print("Website image built successfully.")
    print()


def upload_web(source, target, env):
    chip = get_chip()
    offset = get_partition_offset("website")

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