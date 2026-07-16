#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFiType.h>
#include <Networking/WifiMode.h>

struct NetworkConfig
{
    WiFiMode_t mode;
    String ssid;
    String password;

    String toString() const
    {
        return "Mode: " + String(mode) + "\nSSID: " + ssid + "\nPassword: ***";
    }

    void toJson(JsonObject json) const
    {
        json["ssid"] = ssid;
        json["mode"] = Enum::toString(mode);
        json["password"] = password;
    }

    void fromJson(JsonObjectConst json)
    {
        mode = Enum::fromString<WiFiMode_t>(json["mode"] | "ap+sta");
        ssid = json["ssid"] | "";
        password = json["password"] | "";
    }
};

struct MQTTConfig
{
    bool enabled;
    String host;
    uint16_t port;
    String username;
    String password;
    bool autoDiscoveryEnabled;
    String autoDiscoveryPrefix;
    String clientId;

    String toString() const
    {
        return "Enabled: " + String(enabled) + "\nHost: " + host + "\nPort: " + String(port) + "\nUsername: " + username + "\nPassword: ***" + "\nAuto Discovery Enabled: " + String(autoDiscoveryEnabled) + "\nAuto Discovery Prefix: " + autoDiscoveryPrefix + "\nClient ID: " + clientId;
    }

    void toJson(JsonObject json) const
    {
        json["enabled"] = enabled;
        json["host"] = host;
        json["port"] = port;
        json["username"] = username;
        json["password"] = password;
        json["autoDiscoveryEnabled"] = autoDiscoveryEnabled;
        json["autoDiscoveryPrefix"] = autoDiscoveryPrefix;
        json["clientId"] = clientId;
    }

    void fromJson(JsonObjectConst json)
    {
        enabled = json["enabled"] | false;
        host = json["host"] | "";
        port = json["port"] | 1883;
        username = json["username"] | "";
        password = json["password"] | "";
        autoDiscoveryEnabled = json["autoDiscoveryEnabled"] | false;
        autoDiscoveryPrefix = json["autoDiscoveryPrefix"] | "";
        clientId = json["clientId"] | "";
    }
};

struct Config
{
    NetworkConfig network;
    MQTTConfig mqtt;

    String toString() const
    {
        return "Network Config:\n" + String(network.toString()) + "\nMQTT Config:\n" + String(mqtt.toString());
    }

    void toJson(JsonObject json) const
    {
        network.toJson(json["network"].to<JsonObject>());
        mqtt.toJson(json["mqtt"].to<JsonObject>());
    }

    void fromJson(JsonObjectConst json)
    {
        network.fromJson(json["network"].as<JsonObjectConst>());
        mqtt.fromJson(json["mqtt"].as<JsonObjectConst>());
    }
};