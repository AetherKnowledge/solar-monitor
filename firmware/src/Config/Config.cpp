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

    config.network.mode = doc["network"]["mode"] | "ap+sta";
    config.network.ssid = doc["network"]["ssid"] | "";
    config.network.password = doc["network"]["password"] | "";

    config.mqtt.enabled = doc["mqtt"]["enabled"] | false;
    config.mqtt.host = doc["mqtt"]["host"] | "";
    config.mqtt.port = doc["mqtt"]["port"] | 1883;
    config.mqtt.username = doc["mqtt"]["username"] | "";
    config.mqtt.password = doc["mqtt"]["password"] | "";
    config.mqtt.autoDiscoveryEnabled = doc["mqtt"]["autoDiscoveryEnabled"] | false;
    config.mqtt.autoDiscoveryPrefix = doc["mqtt"]["autoDiscoveryPrefix"] | "homeassistant";
    config.mqtt.clientId = doc["mqtt"]["clientId"] | "solar-monitor";

    return true;
}

bool saveConfig()
{
    JsonDocument doc;

    doc["network"]["mode"] = config.network.mode;
    doc["network"]["ssid"] = config.network.ssid;
    doc["network"]["password"] = config.network.password;

    doc["mqtt"]["enabled"] = config.mqtt.enabled;
    doc["mqtt"]["host"] = config.mqtt.host;
    doc["mqtt"]["port"] = config.mqtt.port;
    doc["mqtt"]["username"] = config.mqtt.username;
    doc["mqtt"]["password"] = config.mqtt.password;
    doc["mqtt"]["autoDiscoveryEnabled"] = config.mqtt.autoDiscoveryEnabled;
    doc["mqtt"]["autoDiscoveryPrefix"] = config.mqtt.autoDiscoveryPrefix;
    doc["mqtt"]["clientId"] = config.mqtt.clientId;

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