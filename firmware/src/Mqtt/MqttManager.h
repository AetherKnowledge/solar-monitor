#pragma once

#include <ArduinoJson.h>
#include <Config/ConfigTypes.h>
#include <concepts>

namespace MqttManager {
    extern volatile bool reloadRequested;

    void setup();
    bool connect();
    void reload();
    void disconnect();
    bool isConnected();

    void loop();

    bool publish(const String& topic, const String& payload, bool retain = true);
    bool publish(const String& topic, const JsonDocument& payload, bool retain = true);
    bool requestUpdate(const MQTTConfig& newConfig);

    void subscribeAll();

    void generateTopics();

    bool usePayload(char* topic,
                    byte* payload,
                    unsigned int length,
                    WriteRegister& reg,
                    const ModbusDevice& device);

    template <typename T>
    bool publish(const String& topic, const T& value, bool retain = true) {
        return publish(topic, String(value), retain);
    }

    void callback(char* topic, byte* payload, unsigned int length);
}  // namespace MqttManager