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

const result = spawnSync("pio", ["run", "-e", environment, "-t", "backup"], {
  stdio: "inherit",
  shell: false,
});

if (result.error) {
  console.error(result.error.message);
  process.exit(1);
}

process.exit(result.status ?? 1);
