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

    bool publish(const String &topic, const String &payload);
    bool publish(const String &topic, const JsonDocument &payload);

    template <typename T>
    bool publish(const String &topic, const T &value)
    {
        return publish(topic, String(value));
    }

    void callback(char *topic, byte *payload, unsigned int length);
}