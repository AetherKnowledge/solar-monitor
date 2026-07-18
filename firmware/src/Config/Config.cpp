#include "Config.h"
#include <ArduinoJson.h>
#include <Networking/Networking.h>
#include <Mqtt/MqttManager.h>

Config config;
bool configLoaded = false;

volatile bool networkUpdateRequested = false;
NetworkConfig pendingNetworkConfig;

bool loadConfigFile(File &file)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        Serial.printf("Failed to deserialize config: %s\n",
                      error.c_str());
        return false;
    }

    config.fromJson(doc.as<JsonObject>());

    Serial.println("Config loaded");
    Serial.println(config.toString().c_str());

    configLoaded = true;
    return true;
}

bool loadConfig()
{
    Serial.println();
    Serial.println("Loading config");

    if (!LittleFS.begin())
    {
        Serial.println("LittleFS not mounted");
        return false;
    }

    File config = LittleFS.open(CONFIG_LOCATION, "r");
    File backup = LittleFS.open(CONFIG_BAK_LOCATION, "r");

    if (config && loadConfigFile(config))
    {
        backup.close();
        Serial.println("Loaded default config successfully");
        return true;
    }
    else if (backup && loadConfigFile(backup))
    {
        config.close();

        Serial.println("Loaded backup config successfully");
        if (!copyFile(CONFIG_BAK_LOCATION, CONFIG_LOCATION))
        {
            Serial.println("Failed to restore config from backup.");
        }
        return true;
    }

    backup.close();
    config.close();

    Serial.println("Failed to load default and backup config!");
    return false;
}

bool saveConfig()
{
    JsonDocument doc;
    config.toJson(doc.to<JsonObject>());

    if (LittleFS.exists(CONFIG_LOCATION) && !copyFile(CONFIG_LOCATION, CONFIG_BAK_LOCATION))
    {
        Serial.println("Backing up config failed!");
    }

    File config = LittleFS.open(CONFIG_LOCATION, "w");
    if (!config)
    {
        Serial.println("Failed to open config for writing.");
        return false;
    }

    if (serializeJson(doc, config) == 0)
    {
        Serial.println("Failed to write config.");
        config.close();
        return false;
    }

    config.close();

    Serial.println();
    Serial.println("Config saved");

    return true;
}

void resetConfig()
{
    config.network.mode = WiFiMode_t::WIFI_AP_STA;
    config.network.ssid = "";
    config.network.password = "";

    config.mqtt.enabled = false;
    config.mqtt.host = "";
    config.mqtt.port = 1883;
    config.mqtt.username = "";
    config.mqtt.password = "";
    config.mqtt.autoDiscoveryEnabled = false;
    config.mqtt.autoDiscoveryPrefix = "homeassistant";
    config.mqtt.clientId = "solar-monitor";
}

bool updateNetworkConfig(const NetworkConfig &newConfig)
{
    bool isValidConnection = connectToWiFi(newConfig.ssid, newConfig.password, newConfig.mode);
    if (!isValidConnection)
    {
        Serial.println();
        Serial.println("Failed to connect to the new network. Keeping the old configuration.");

        connectToWiFi(config.network.ssid, config.network.password, config.network.mode);

        return false;
    }

    config.network = newConfig;
    saveConfig();

    MqttManager::disconnect();
    MqttManager::setup();

    Serial.println();
    Serial.println("Network configuration updated");
    Serial.println(config.network.toString().c_str());

    return true;
}

bool updateMqttConfig(const MQTTConfig &newConfig)
{
    config.mqtt = newConfig;
    saveConfig();

    MqttManager::disconnect();
    MqttManager::setup();

    Serial.println();
    Serial.println("MQTT configuration updated");
    Serial.println(config.mqtt.toString().c_str());

    return true;
}