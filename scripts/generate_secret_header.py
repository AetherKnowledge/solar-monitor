from pathlib import Path
import secrets
import re

Import("env")

PROJECT_DIR = Path(env["PROJECT_DIR"])

ENV_FILE = PROJECT_DIR / ".env"
HEADER_FILE = PROJECT_DIR / "firmware/include" / "SecretKey.h"

KEY_NAME = "SECRET_KEY"
KEY_BYTES = 32


def load_env(path: Path):
    values = {}

    if not path.exists():
        return values

    for line in path.read_text().splitlines():
        line = line.strip()

        if not line or line.startswith("#"):
            continue

        if "=" not in line:
            continue

        key, value = line.split("=", 1)
        values[key.strip()] = value.strip()

    return values


env_values = load_env(ENV_FILE)

secret = env_values.get(KEY_NAME)

if secret is None:
    secret = secrets.token_hex(KEY_BYTES)
    env_values[KEY_NAME] = secret

    with ENV_FILE.open("w") as f:
        for key, value in env_values.items():
            f.write(f"{key}={value}\n")

    print(f"[secret] Generated new {KEY_NAME}")

if not re.fullmatch(r"[0-9a-fA-F]{64}", secret):
    raise ValueError(
        f"{KEY_NAME} must contain exactly {KEY_BYTES * 2} hexadecimal characters."
    )

bytes_list = [
    f"0x{secret[i:i+2].upper()}"
    for i in range(0, len(secret), 2)
]

HEADER_FILE.parent.mkdir(parents=True, exist_ok=True)

with HEADER_FILE.open("w") as f:
    f.write("// Auto-generated. Do not edit.\n")
    f.write("#pragma once\n\n")
    f.write("#include <cstdint>\n\n")
    f.write("namespace Secret {\n\n")
    f.write(f"inline constexpr uint8_t KEY[{KEY_BYTES}] = {{\n")

    for i in range(0, len(bytes_list), 8):
        f.write("    ")
        f.write(", ".join(bytes_list[i:i + 8]))
        if i + 8 < len(bytes_list):
            f.write(",")
        f.write("\n")

    f.write("};\n\n")
    f.write("} // namespace Secret\n")

print(f"[secret] Wrote {HEADER_FILE}")