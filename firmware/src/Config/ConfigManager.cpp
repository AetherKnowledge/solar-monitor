#include "ConfigManager.h"
#include <ArduinoJson.h>
#include <Mqtt/MqttManager.h>
#include <Networking/NetworkManager.h>
#include <Modbus/ModbusManager.h>
#include <Common/UpdateStatus.h>
#include <Common/Logger.h>
#include <Common/Network.h>

namespace ConfigManager {
    Config config;
    bool hasLoaded = false;
    fs::LittleFSFS ConfigFS;
    bool backupLoaded = false;

    bool loadFile(File& file) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Log.printf("Failed to deserialize config: %s\n", error.c_str());
            return false;
        }

        config.fromJson(doc.as<JsonObject>());

        Log.println("Config loaded");
        Log.println(config.toString().c_str());

        hasLoaded = true;
        return true;
    }

    bool load() {
        Log.println();
        Log.println("Loading config");

        if (!mountFS()) {
            Log.println("Failed to mount ConfigFS");
            return false;
        }

        File config = ConfigFS.open(CONFIG_LOCATION, "r");
        File backup = ConfigFS.open(CONFIG_BAK_LOCATION, "r");

        if (config && loadFile(config)) {
            backup.close();
            Log.println("Loaded default config successfully");
            return true;
        } else if (backup && loadFile(backup)) {
            config.close();
            backupLoaded = true;

            Log.println("Loaded backup config successfully");
            if (!copyFile(ConfigFS, CONFIG_BAK_LOCATION, CONFIG_LOCATION)) {
                Log.println("Failed to restore config from backup.");
            }
            return true;
        }

        backup.close();
        config.close();

        Log.println("Failed to load config and backup config! Resetting to default config.");
        reset();
        return false;
    }

    bool mountFS() {
        return ConfigFS.begin(false, "/storage", 10, "storage");
    }

    bool save() {
        JsonDocument doc;
        config.toJson(doc.to<JsonObject>());

        if (ConfigFS.exists(CONFIG_LOCATION) &&
            !copyFile(ConfigFS, CONFIG_LOCATION, CONFIG_BAK_LOCATION)) {
            Log.println("Backing up config failed!");
        }

        File config = ConfigFS.open(CONFIG_LOCATION, "w");
        if (!config) {
            Log.println("Failed to open config for writing.");
            return false;
        }

        if (serializeJson(doc, config) == 0) {
            Log.println("Failed to write config.");
            config.close();
            return false;
        }

        config.close();

        Log.println();
        Log.println("Config saved");

        return true;
    }

    void reset() {
        config = Config();
        save();
    }

}  // namespace ConfigManager