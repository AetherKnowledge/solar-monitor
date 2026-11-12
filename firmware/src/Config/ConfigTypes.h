#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Modbus/ModbusTypes.h>
#include <Networking/WifiMode.h>
#include <WiFiType.h>
#include <vector>
#include "Common/Enum.h"

struct SiteConfig {
    bool passwordEnabled = false;
    String password = "";

    void toJson(JsonObject json, bool withPassword = false) const {
        json["passwordEnabled"] = passwordEnabled;

        if (withPassword) {
            json["password"] = password;
        } else {
            json["hasPassword"] = !password.isEmpty();
            json["password"] = "";
        }
    }

    void fromJson(JsonObjectConst json) {
        passwordEnabled = json["passwordEnabled"] | passwordEnabled;
        password = json["password"] | password;
    }

    String toString() const {
        return "Password Enabled: " + String(passwordEnabled) + "\nPassword: ***";
    }
};

struct NetworkConfig {
    WiFiMode_t mode = WiFiMode_t::WIFI_MODE_APSTA;
    String wifiSsid = "SolarMonitor";
    String wifiPassword = "";
    String apSsid = "SolarMonitor";
    String apPassword = "SolarMonitor";
    bool apPasswordEnabled = true;

    String toString() const {
        return "Mode: " + String(mode) + "\nSSID: " + wifiSsid + "\nPassword: ***";
    }

    void toJson(JsonObject json, bool withPassword = false) const {
        json["mode"] = Enum::toString(mode);
        json["wifiSsid"] = wifiSsid;

        json["apSsid"] = apSsid;
        json["apPasswordEnabled"] = apPasswordEnabled;

        if (withPassword) {
            json["wifiPassword"] = wifiPassword;
            json["apPassword"] = apPassword;
        } else {
            json["hasWifiPassword"] = !wifiPassword.isEmpty();
            json["hasApPassword"] = !apPassword.isEmpty();
            json["wifiPassword"] = "";
            json["apPassword"] = "";
        }
    }

    void fromJson(JsonObjectConst json) {
        mode = Enum::fromString<WiFiMode_t>(json["mode"] | Enum::toString(mode));
        wifiSsid = json["wifiSsid"] | wifiSsid;
        wifiPassword = json["wifiPassword"] | wifiPassword;
        apSsid = json["apSsid"] | apSsid;
        apPassword = json["apPassword"] | apPassword;
        apPasswordEnabled = json["apPasswordEnabled"] | apPasswordEnabled;
    }
};

struct MQTTConfig {
    bool enabled = true;
    String host = "localhost";
    uint16_t port = 1883;
    String username = "";
    String password = "";
    bool autoDiscoveryEnabled = true;
    String autoDiscoveryPrefix = "homeassistant";
    String clientId = "";

    String toString() const {
        return "Enabled: " + String(enabled) + "\nHost: " + host + "\nPort: " + String(port) +
               "\nUsername: " + username + "\nPassword: ***" +
               "\nAuto Discovery Enabled: " + String(autoDiscoveryEnabled) +
               "\nAuto Discovery Prefix: " + autoDiscoveryPrefix + "\nClient ID: " + clientId;
    }

    void toJson(JsonObject json, bool withPassword = false) const {
        json["enabled"] = enabled;
        json["host"] = host;
        json["port"] = port;
        json["username"] = username;

        if (withPassword) {
            json["password"] = password;
        } else {
            json["hasPassword"] = !password.isEmpty();
            json["password"] = "";
        }

        json["autoDiscoveryEnabled"] = autoDiscoveryEnabled;
        json["autoDiscoveryPrefix"] = autoDiscoveryPrefix;
        json["clientId"] = clientId;
    }

    void fromJson(JsonObjectConst json) {
        enabled = json["enabled"] | enabled;
        host = json["host"] | host;
        port = json["port"] | port;
        username = json["username"] | username;
        password = json["password"] | password;
        autoDiscoveryEnabled = json["autoDiscoveryEnabled"] | autoDiscoveryEnabled;
        autoDiscoveryPrefix = json["autoDiscoveryPrefix"] | autoDiscoveryPrefix;
        clientId = json["clientId"] | clientId;
    }
};

struct Config {
    SiteConfig site;
    NetworkConfig network;
    MQTTConfig mqtt;
    std::vector<ModbusDevice> modbusDevices;

    String toString() const {
        return "Network Config:\n" + String(network.toString()) + "\nMQTT Config:\n" +
               String(mqtt.toString()) + "\nModbus Devices:\n" + String(modbusDevices.size());
    }

    ModbusDevice* getDeviceById(const String& id) {
        for (auto& device : modbusDevices) {
            if (device.discovery.identifier == id) {
                return &device;
            }
        }
        return nullptr;
    }

    void toJson(JsonObject json, bool withPassword = false) const {
        network.toJson(json["network"].to<JsonObject>(), withPassword);
        mqtt.toJson(json["mqtt"].to<JsonObject>(), withPassword);
        site.toJson(json["site"].to<JsonObject>(), withPassword);

        serializeVector(json["modbusDevices"], modbusDevices);
    }

    void fromJson(JsonObject json) {
        network.fromJson(json["network"].as<JsonObject>());
        mqtt.fromJson(json["mqtt"].as<JsonObject>());
        site.fromJson(json["site"].as<JsonObject>());

        deserializeVector(json["modbusDevices"], modbusDevices);
    }

    void reset() {
        network = NetworkConfig();
        mqtt = MQTTConfig();
        modbusDevices.clear();
        site = SiteConfig();
    }
};