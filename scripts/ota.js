import "dotenv/config";
import { spawnSync } from "node:child_process";

const ip = process.env.DEVICE_IP;

if (!ip) {
  console.error("DEVICE_IP is not set");
  process.exit(1);
}

const [, , type] = process.argv;

const targets = {
  firmware: {
    file: ".pio/build/esp32dev/firmware.bin",
    endpoint: "/api/update/firmware",
  },
  web: {
    file: ".pio/build/esp32dev/littlefs.bin",
    endpoint: "/api/update/website",
  },
  config: {
    file: "config.json",
    endpoint: "/api/config",
  },
  restart: {
    endpoint: "/api/system/restart",
  },
};

const target = targets[type];

if (!target) {
  console.error("Usage: node scripts/ota.js <firmware|web|config|restart>");
  process.exit(1);
}

const args = ["-X", "POST"];

if (target.file) {
  args.push("-F", `file=@${target.file}`);
}

args.push(`http://${ip}${target.endpoint}`);

spawnSync("curl", args, {
  stdio: "inherit",
  shell: true,
});
