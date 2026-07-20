#include "ConfigManager.h"
#include <ArduinoJson.h>
#include <Mqtt/MqttManager.h>
#include <Networking/NetworkManager.h>
#include <Modbus/ModbusManager.h>
#include <Common/UpdateStatus.h>

namespace ConfigManager {
    Config config;
    bool hasLoaded = false;

    bool loadFile(File& file) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Serial.printf("Failed to deserialize config: %s\n", error.c_str());
            return false;
        }

        config.fromJson(doc.as<JsonObject>());

        Serial.println("Config loaded");
        Serial.println(config.toString().c_str());

        hasLoaded = true;
        return true;
    }

    bool load() {
        Serial.println();
        Serial.println("Loading config");

        if (!LittleFS.begin()) {
            Serial.println("LittleFS not mounted");
            return false;
        }

        File config = LittleFS.open(CONFIG_LOCATION, "r");
        File backup = LittleFS.open(CONFIG_BAK_LOCATION, "r");

        if (config && loadFile(config)) {
            backup.close();
            Serial.println("Loaded default config successfully");
            return true;
        } else if (backup && loadFile(backup)) {
            config.close();

            Serial.println("Loaded backup config successfully");
            if (!copyFile(CONFIG_BAK_LOCATION, CONFIG_LOCATION)) {
                Serial.println("Failed to restore config from backup.");
            }
            return true;
        }

        backup.close();
        config.close();

        Serial.println("Failed to load default and backup config!");
        return false;
    }

    bool save() {
        JsonDocument doc;
        config.toJson(doc.to<JsonObject>());

        if (LittleFS.exists(CONFIG_LOCATION) && !copyFile(CONFIG_LOCATION, CONFIG_BAK_LOCATION)) {
            Serial.println("Backing up config failed!");
        }

        File config = LittleFS.open(CONFIG_LOCATION, "w");
        if (!config) {
            Serial.println("Failed to open config for writing.");
            return false;
        }

        if (serializeJson(doc, config) == 0) {
            Serial.println("Failed to write config.");
            config.close();
            return false;
        }

        config.close();

        Serial.println();
        Serial.println("Config saved");

        return true;
    }

}  // namespace ConfigManager