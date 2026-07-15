#include "Config.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

Config config;

bool loadConfig()
{
    Serial.println("Loading config");

    if (!LittleFS.begin())
    {
        Serial.println("LittleFS Mount Failed");
        return false;
    }

    // incase the config file does not exist or is invalid, reset to default values
    resetConfig();

    File file = LittleFS.open("/config.json", "r");
    if (!file)
    {
        Serial.println("Failed to open config file");
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        Serial.println("Failed to deserialize JSON");
        file.close();
        return false;
    }

    config.fromJson(doc.as<JsonObjectConst>());

    Serial.println("Config loaded");
    Serial.println(config.toString().c_str());

    return true;
}

bool saveConfig()
{
    JsonDocument doc;
    config.toJson(doc.to<JsonObject>());

    File file = LittleFS.open("/config.json", "w");
    if (!file)
    {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    serializeJsonPretty(doc, file);
    file.close();

    Serial.println("Config saved");

    return true;
}

void resetConfig()
{
    config.network.mode = "ap+sta";
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

bool setNetworkConfig(const NetworkConfig &newConfig)
{
    config.network = newConfig;
    saveConfig();

    Serial.println("Network configuration updated");
    Serial.println(newConfig.toString().c_str());

    return true;
}

bool setMqttConfig(const MQTTConfig &newConfig)
{
    config.mqtt = newConfig;
    saveConfig();
    Serial.println("MQTT configuration updated");
    Serial.println(newConfig.toString().c_str());
    return true;
}