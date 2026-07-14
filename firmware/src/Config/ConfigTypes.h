#pragma once

#include <Arduino.h>

struct NetworkConfig
{
    String mode;
    String ssid;
    String password;

    const char *c_str() const
    {
        String configString = "Mode: " + mode + "\nSSID: " + ssid + "\nPassword: ***";

        return configString.c_str();
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

    const char *c_str() const
    {
        String configString = "Enabled: " + String(enabled) + "\nHost: " + host + "\nPort: " + String(port) + "\nUsername: " + username + "\nPassword: ***" + "\nAuto Discovery Enabled: " + String(autoDiscoveryEnabled) + "\nAuto Discovery Prefix: " + autoDiscoveryPrefix + "\nClient ID: " + clientId;

        return configString.c_str();
    }
};

struct Config
{
    NetworkConfig network;
    MQTTConfig mqtt;

    const char *c_str() const
    {
        String configString = "Network Config:\n" + String(network.c_str()) + "\nMQTT Config:\n" + String(mqtt.c_str());

        return configString.c_str();
    }
};