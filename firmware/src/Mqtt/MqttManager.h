#pragma once

#include <ArduinoJson.h>
#include <Config/ConfigTypes.h>
#include <concepts>

namespace MqttManager {
    void setup();
    bool connect();
    void disconnect();
    bool isConnected();

    void loop();

    bool publish(const String& topic, const String& payload, bool retain = true);
    bool publish(const String& topic, const JsonDocument& payload, bool retain = true);

    void subscribeAll();

    void generateTopics();

    template <typename T>
    concept WriteRegister = requires(T r) {
        { r.discovery.commandTopic } -> std::same_as<String&>;
        { r.value } -> std::same_as<double&>;
    };

    template <WriteRegister TRegister>
    bool usePayload(char* topic,
                    byte* payload,
                    unsigned int length,
                    TRegister& reg,
                    const ModbusDevice& device);

    template <typename T>
    bool publish(const String& topic, const T& value, bool retain = true) {
        return publish(topic, String(value), retain);
    }

    void callback(char* topic, byte* payload, unsigned int length);
}  // namespace MqttManager