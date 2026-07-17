#pragma once

#include <Config/ConfigTypes.h>
#include <ArduinoJson.h>

namespace MqttManager
{
    void setup();
    bool connect();
    void disconnect();
    bool isConnected();

    void loop();

    bool publish(const String &topic, const String &payload, bool retain = true);
    bool publish(const String &topic, const JsonDocument &payload, bool retain = true);

    template <typename T>
    bool publish(const String &topic, const T &value, bool retain = true)
    {
        return publish(topic, String(value), retain);
    }

    void callback(char *topic, byte *payload, unsigned int length);
}