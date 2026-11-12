import { spawnSync } from "node:child_process";
import { readFileSync } from "node:fs";

const ini = readFileSync("platformio.ini", "utf8");
const environments = Array.from(
  ini.matchAll(/^\s*\[env:([^\]]+)\]/gm),
  (match) => match[1],
);

if (environments.length === 0) {
  console.error("No PlatformIO environments found in platformio.ini.");
  process.exit(1);
}

const pnpmPath = process.env.npm_execpath;
if (!pnpmPath) {
  console.error("Run this script through pnpm release.");
  process.exit(1);
}

const pnpmIsExecutable = pnpmPath.toLowerCase().endsWith(".exe");
const pnpmCommand = pnpmIsExecutable ? pnpmPath : process.execPath;
const pnpmPrefix = pnpmIsExecutable ? [] : [pnpmPath];

function run(command: string, args: string[]) {
  const result = spawnSync(command, args, { stdio: "inherit", shell: false });
  if (result.error) {
    console.error(result.error.message);
    process.exit(1);
  }
  if (result.status !== 0) {
    process.exit(result.status ?? 1);
  }
}

console.log("Building release for all configured boards...");
run(pnpmCommand, [...pnpmPrefix, "--filter", "web", "build"]);
for (const environment of environments) {
  console.log(`Building ${environment}...`);
  run("pio", ["run", "-e", environment, "-t", "buildweb"]);
  run("pio", ["run", "-e", environment]);
}
run(pnpmCommand, [...pnpmPrefix, "manifest"]);
