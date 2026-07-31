import { createPublicKey, generateKeyPairSync } from "node:crypto";
import { accessSync, readFileSync } from "node:fs";
import { writeFile } from "node:fs/promises";
import path from "node:path";
import { pathToFileURL } from "node:url";

export type KeyPair = {
  privateKey: string;
  publicKey: string;
};

export async function generateKeyPair(): Promise<KeyPair> {
  let privateKey: string | undefined = undefined;
  let publicKey: string | undefined = undefined;

  try {
    privateKey = readFileSync(
      path.join(process.cwd(), "signature/private.pem"),
      "utf8",
    );
  } catch (e) {
    throw new Error(
      "Private key not found. Please get the original private key or generate a new private key with pnpm generate-keys. Note that generating a new private key will invalidate all existing signatures.",
    );
  }

  try {
    publicKey = readFileSync(
      path.join(process.cwd(), "signature/public.pem"),
      "utf8",
    );

    console.log("Using existing private and public keys.");
  } catch (e) {
    console.log("Public key not found, generating from private key...");
    publicKey = createPublicKey(privateKey)
      .export({ type: "spki", format: "pem" })
      .toString();
    await writeFile(
      path.join(process.cwd(), "signature/public.pem"),
      publicKey,
    );
  }

  return {
    privateKey: privateKey!,
    publicKey: publicKey!,
  };
}

async function generatePrivateKey(): Promise<KeyPair> {
  let privateKey: string;
  let publicKey: string;

  let exists = false;

  try {
    accessSync(path.join(process.cwd(), "signature/private.pem"));
    exists = true;
  } catch {}

  if (exists) {
    throw new Error(
      "Private key already exists. Delete it manually if you really want to generate a new one.",
    );
  }

  const pair = generateKeyPairSync("ec", {
    namedCurve: "prime256v1",
    privateKeyEncoding: {
      type: "pkcs8",
      format: "pem",
    },
    publicKeyEncoding: {
      type: "spki", // Standard format for public keys
      format: "pem",
    },
  });

  await writeFile(
    path.join(process.cwd(), "signature/private.pem"),
    pair.privateKey,
  );
  await writeFile(
    path.join(process.cwd(), "signature/public.pem"),
    pair.publicKey,
  );

  privateKey = pair.privateKey;
  publicKey = pair.publicKey;

  console.log("Generated new private and public keys.");

  return {
    privateKey: privateKey!,
    publicKey: publicKey!,
  };
}

if (import.meta.url === pathToFileURL(process.argv[1]).href) {
  console.log("Generating new private and public keys...");
  await generatePrivateKey();
}
