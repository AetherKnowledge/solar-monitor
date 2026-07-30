// scripts/generate-manifest.mjs

import { createHash } from "node:crypto";
import { copyFile, mkdir, readFile, stat, writeFile } from "node:fs/promises";
import path from "node:path";

const root = process.cwd();

const version = JSON.parse(
  await readFile(path.join(root, "version.json"), "utf8"),
);

const VERSION = `v${version.version}`;
const FIRMWARE_VERSION = `v${version.firmware}`;
const WEBSITE_VERSION = `v${version.website}`;
const REPO =
  "https://github.com/AetherKnowledge/solar-monitor/releases/download";

const firmwarePath = path.join(root, ".pio/build/esp32dev/firmware.bin");
const websitePath = path.join(root, ".pio/build/esp32dev/littlefs.bin");

const releaseDir = path.join(root, "release");

await mkdir(releaseDir, { recursive: true });

async function fileInfo(
  file: string,
): Promise<{ size: number; sha256: string }> {
  const buffer = await readFile(file);
  const { size } = await stat(file);

  return {
    size,
    sha256: createHash("sha256").update(buffer).digest("hex"),
  };
}

const firmware = await fileInfo(firmwarePath);
const website = await fileInfo(websitePath);

const manifest = {
  version: VERSION,
  buildTime: new Date().toISOString(),

  firmware: {
    version: FIRMWARE_VERSION,
    file: "firmware.bin",
    url: `${REPO}/${FIRMWARE_VERSION}/firmware.bin`,
    size: firmware.size,
    sha256: firmware.sha256,
  },

  website: {
    version: WEBSITE_VERSION,
    file: "website.bin",
    url: `${REPO}/${WEBSITE_VERSION}/website.bin`,
    size: website.size,
    sha256: website.sha256,
  },
};

// Copy release assets
await copyFile(firmwarePath, path.join(releaseDir, "firmware.bin"));

await copyFile(websitePath, path.join(releaseDir, "website.bin"));

// Write manifest to release folder
const manifestJson = JSON.stringify(manifest, null, 2) + "\n";

await writeFile(path.join(releaseDir, "manifest.json"), manifestJson);

// Also write manifest to repository root
await writeFile(path.join(root, "manifest.json"), manifestJson);
console.log("✓ Copied firmware.bin");
console.log("✓ Copied website.bin");
console.log("✓ Generated release/manifest.json");
