// scripts/generate-manifest.mjs

import { createHash, sign } from "node:crypto";
import { readFileSync } from "node:fs";
import { copyFile, mkdir, readFile, stat, writeFile } from "node:fs/promises";
import path from "node:path";
import { generateKeyPair } from "./generate-keys";

type ReleaseAsset = {
  version: string;
  file: string;
  url: string;
  size: number;
  sha256: string;
};

type ReleaseAssetWithSignature = ReleaseAsset & {
  signature: string;
};

type BoardManifest = {
  firmware: ReleaseAssetWithSignature;
  website: ReleaseAssetWithSignature;
};

type Manifest = {
  buildTime: string;
  boards: Record<string, BoardManifest>;
};

const ini = readFileSync("platformio.ini", "utf8");

const boards = Array.from(
  ini.matchAll(/^\s*\[env:([^\]]+)\]/gm),
  (match) => match[1],
);

const keyPair = await generateKeyPair();

const root = process.cwd();

const version = JSON.parse(
  await readFile(path.join(root, "version.json"), "utf8"),
);

const FIRMWARE_VERSION = `v${version.firmware}`;
const WEBSITE_VERSION = `v${version.website}`;
const REPO =
  "https://github.com/AetherKnowledge/solar-monitor/releases/download";
const releaseDir = path.join(root, "release");

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

function assetPayload(asset: ReleaseAsset): Buffer {
  return Buffer.from(
    [
      asset.version,
      asset.file,
      asset.url,
      asset.size.toString(),
      asset.sha256,
    ].join("\n"),
    "utf8",
  );
}

function signAsset(asset: ReleaseAsset): ReleaseAssetWithSignature {
  const signature = sign("sha256", assetPayload(asset), {
    key: keyPair.privateKey,
  });

  return {
    ...asset,
    signature: signature.toString("base64"),
  };
}

async function generateBoardManifest(board: string): Promise<BoardManifest> {
  const firmwarePath = path.join(root, `.pio/build/${board}/firmware.bin`);
  const websitePath = path.join(root, `.pio/build/${board}/website.bin`);

  const firmware = await fileInfo(firmwarePath);
  const website = await fileInfo(websitePath);

  // Create release directory if it doesn't exist
  await mkdir(releaseDir, { recursive: true });

  // Copy release assets
  await copyFile(firmwarePath, path.join(releaseDir, board + "_firmware.bin"));
  await copyFile(websitePath, path.join(releaseDir, board + "_website.bin"));

  return {
    firmware: signAsset({
      version: FIRMWARE_VERSION,
      file: "firmware.bin",
      url: `${REPO}/${FIRMWARE_VERSION}/${board}_firmware.bin`,
      size: firmware.size,
      sha256: firmware.sha256,
    }),
    website: signAsset({
      version: WEBSITE_VERSION,
      file: "website.bin",
      url: `${REPO}/${WEBSITE_VERSION}/${board}_website.bin`,
      size: website.size,
      sha256: website.sha256,
    }),
  };
}

const manifest: Manifest = {
  buildTime: new Date().toISOString(),

  boards: Object.fromEntries(
    await Promise.all(
      boards.map(async (board) => [board, await generateBoardManifest(board)]),
    ),
  ),
};

// Write manifest to release folder
const manifestJson = JSON.stringify(manifest, null, 2) + "\n";

await writeFile(path.join(releaseDir, "manifest.json"), manifestJson);

// Also write manifest to repository root
await writeFile(path.join(root, "manifest.json"), manifestJson);
console.log("✓ Copied firmware.bin");
console.log("✓ Copied website.bin");
console.log("✓ Generated release/manifest.json");
