import { spawnSync } from "node:child_process";
import { readFileSync } from "node:fs";
import { parseArgs } from "node:util";

const { values } = parseArgs({
  options: { board: { type: "string", short: "b" } },
});

const ini = readFileSync("platformio.ini", "utf8");
const environment =
  values.board ?? ini.match(/^\s*default_envs\s*=\s*([^\r\n;#]+)/m)?.[1].trim();
const environments = Array.from(
  ini.matchAll(/^\s*\[env:([^\]]+)\]/gm),
  (match) => match[1],
);

if (!environment || !environments.includes(environment)) {
  console.error(
    "Choose one configured PlatformIO environment with -b <environment>.",
  );
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

console.log(`Building release for ${environment}...`);
run(pnpmCommand, [...pnpmPrefix, "--filter", "web", "build"]);
run("pio", ["run", "-e", environment, "-t", "buildweb"]);
run("pio", ["run", "-e", environment]);
run(pnpmCommand, [...pnpmPrefix, "manifest", "-b", environment]);
