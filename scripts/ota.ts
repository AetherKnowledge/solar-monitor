import "dotenv/config";
import { spawnSync } from "node:child_process";
import { existsSync, readFileSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";
import config from "../config.json";

const ip = process.env.DEVICE_IP;

if (!ip) {
  console.error("DEVICE_IP is not set");
  process.exit(1);
}

const argv = process.argv.slice(2);

let authPassword: string | undefined;
let environment: string | undefined;

const authIndex = argv.findIndex((arg) => arg === "-auth" || arg === "--auth");

if (authIndex !== -1) {
  authPassword = argv[authIndex + 1];

  if (!authPassword) {
    console.error("Missing password after -auth");
    process.exit(1);
  }

  argv.splice(authIndex, 2);
}

const envIndex = argv.findIndex(
  (arg) => arg === "-e" || arg === "--environment",
);

if (envIndex !== -1) {
  environment = argv[envIndex + 1];

  if (!environment) {
    console.error("Missing environment after -e");
    process.exit(1);
  }

  argv.splice(envIndex, 2);
}

const [type] = argv;

function getDefaultEnvironment(): string {
  const ini = readFileSync("platformio.ini", "utf8");

  const match = ini.match(/^\s*default_envs\s*=\s*(.+)$/m);

  if (!match) {
    console.error("Unable to determine default_envs from platformio.ini");
    process.exit(1);
  }

  return match[1].trim();
}

const env = environment ?? getDefaultEnvironment();

const targets = {
  firmware: {
    file: `.pio/build/${env}/firmware.bin`,
    endpoint: "/api/update/firmware/manual",
  },
  web: {
    file: `.pio/build/${env}/website.bin`,
    endpoint: "/api/update/website/manual",
  },
  config: {
    file: "config.json",
    endpoint: "/api/config",
  },
  restart: {
    endpoint: "/api/system/restart",
  },
} as const;

const target = targets[type as keyof typeof targets];

if (!target) {
  console.error(`
Usage:
  tsx scripts/ota.ts <firmware|web|config|restart>

Options:
  -auth <password>        Override configured password
  -e <environment>        PlatformIO environment

Examples:
  tsx scripts/ota.ts firmware
  tsx scripts/ota.ts web -auth admin
  tsx scripts/ota.ts firmware -e esp32dev
  tsx scripts/ota.ts firmware -e esp32dev -auth admin
`);
  process.exit(1);
}

const password =
  authPassword ??
  (config.site.passwordEnabled && config.site.password.trim() !== ""
    ? config.site.password
    : undefined);

const cookieJar = join(tmpdir(), "solar-monitor.cookies");

//
// Login
//
if (password) {
  console.log("Logging in...");

  const loginArgs = [
    "-s",
    "-S",
    "-f",
    "-X",
    "POST",
    "-H",
    "Content-Type: application/json",
    "-d",
    JSON.stringify({ password }),
    "-c",
    cookieJar,
    `http://${ip}/api/auth/login`,
  ];

  console.log(`curl ${loginArgs.join(" ")}`);

  const login = spawnSync("curl", loginArgs, {
    stdio: "inherit",
    shell: false,
  });

  if (login.status !== 0) {
    process.exit(login.status ?? 1);
  }
}

const args: string[] = ["-s", "-S", "-f", "-X", "POST"];

if (password) {
  args.push("-b", cookieJar);
}

if ("file" in target && target.file) {
  if (!existsSync(target.file)) {
    console.error(`File not found: ${target.file}`);
    process.exit(1);
  }

  args.push("-F", `file=@${target.file}`);
}

args.push(`http://${ip}${target.endpoint}`);

console.log(
  type === "restart"
    ? `Restarting device at ${ip} (${env})...`
    : `Uploading ${type} to ${ip} (${env})...`,
);

console.log(`curl ${args.join(" ")}`);

const result = spawnSync("curl", args, {
  stdio: "inherit",
  shell: false,
});

process.exit(result.status ?? 1);
