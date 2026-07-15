#include "Config.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

Config config;

bool loadConfig()
{
    // incase the config file does not exist or is invalid, reset to default values
    resetConfig();

    File file = LittleFS.open("/config.json", "r");
    if (!file)
        return false;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        file.close();
        return false;
    }

    config.fromJson(doc.as<JsonObjectConst>());

    return true;
}

bool saveConfig()
{
    JsonDocument doc;
    config.toJson(doc.to<JsonObject>());

    File file = LittleFS.open("/config.json", "w");
    if (!file)
        return false;

    serializeJsonPretty(doc, file);
    file.close();

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