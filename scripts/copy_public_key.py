Import("env")

from pathlib import Path

ROOT = Path(env["PROJECT_DIR"])

PUBLIC_KEY = ROOT / "signature" / "public.pem"
OUTPUT = ROOT / "firmware" / "include" / "UpdatePublicKey.h"

if not PUBLIC_KEY.exists():
    raise FileNotFoundError(
        f"Public key not found: {PUBLIC_KEY}\n"
        "Run 'pnpm generate-keys' first."
    )

pem = PUBLIC_KEY.read_text(encoding="utf-8").strip()

header = f"""#pragma once

// AUTO-GENERATED FILE.
// Do not edit manually.

namespace UpdateHandler {{

inline constexpr char PUBLIC_KEY[] = R"({pem}
)";

}} // namespace UpdateHandler
"""

OUTPUT.parent.mkdir(parents=True, exist_ok=True)
OUTPUT.write_text(header, encoding="utf-8")

print(f"Generated {OUTPUT.relative_to(ROOT)}")