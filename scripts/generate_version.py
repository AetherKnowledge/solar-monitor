import json
from pathlib import Path

Import("env")

project_dir = Path(env.subst("$PROJECT_DIR"))

version_file = project_dir / "version.json"
output_file = project_dir / "firmware" / "include" / "Version.h"


def generate_version_header():
    if not version_file.exists():
        raise RuntimeError(
            f"Version file does not exist: {version_file}"
        )

    with version_file.open("r", encoding="utf-8") as file:
        versions = json.load(file)

    firmware_version = versions.get("firmware")

    if not firmware_version:
        raise RuntimeError(
            "Missing 'firmware' version in version.json"
        )

    content = f"""\
#pragma once

// AUTO-GENERATED FILE.
// Do not edit manually. Generated from version.json.

namespace Version {{
    inline constexpr const char* FIRMWARE = "{firmware_version}";
}}
"""

    output_file.parent.mkdir(
        parents=True,
        exist_ok=True
    )

    # Only rewrite the file when its contents actually changed.
    if output_file.exists():
        existing = output_file.read_text(
            encoding="utf-8"
        )

        if existing == content:
            print(
                f"Firmware version: {firmware_version}"
            )
            return

    output_file.write_text(
        content,
        encoding="utf-8"
    )

    print(
        f"Generated Version.h: {firmware_version}"
    )


generate_version_header()