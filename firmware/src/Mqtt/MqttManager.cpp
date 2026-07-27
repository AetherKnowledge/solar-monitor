#include "MqttDiscovery.h"
#include "MqttTypes.h"
#include <Config/ConfigManager.h>
#include <PubSubClient.h>
#include "MqttManager.h"
#include <WiFi.h>
#include <Modbus/WriteRegisterManager.h>
#include <Common/Logger.h>
#include <System/SystemManager.h>

static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);

namespace MqttManager {
    bool isConnected() {
        return mqttClient.connected();
    }

    void setup() {
        Log.println();
        Log.println("Setting up MQTT");

        mqttClient.setServer(ConfigManager::config.mqtt.host.c_str(),
                             ConfigManager::config.mqtt.port);
        mqttClient.setCallback(callback);

        mqttClient.setBufferSize(1024);
    }

    void reload() {
        disconnect();
        setup();
    }

    bool connect() {
        Log.println();
        Log.println("Connecting to MQTT");

        MQTTConfig& mqttConfig = ConfigManager::config.mqtt;
        String clientId = mqttConfig.clientId;

        if (clientId.isEmpty()) {
            clientId = "solar-monitor";
        }

        if (mqttClient.connected())
            return true;

        bool connected;

        if (mqttConfig.username.isEmpty()) {
            connected = mqttClient.connect(clientId.c_str());
        } else {
            connected = mqttClient.connect(clientId.c_str(),
                                           mqttConfig.username.c_str(),
                                           mqttConfig.password.c_str(),
                                           "solar-monitor/status",
                                           0,
                                           true,
                                           "offline");
        }

        if (connected) {
            Log.println();
            Log.println("Connected to MQTT");
            mqttClient.publish("solar-monitor/status", "online", true);

            subscribeAll();
            publishAll();
        } else {
            Log.println();
            Log.println("Failed to connect to MQTT");
        }

        Log.printf("MQTT connection status: %d\n", mqttClient.state());
        return connected;
    }

    void disconnect() {
        Log.println();
        Log.println("Disconnecting from MQTT");

        if (!mqttClient.connected())
            return;

        mqttClient.disconnect();
        return;
    }

    void loop() {
        if (WiFi.status() != WL_CONNECTED)
            return;

        if (!mqttClient.connected()) {
            static unsigned long lastAttempt = 0;

            Log.println("MQTT not connected, attempting to reconnect...");

            if (millis() - lastAttempt > 5000) {
                lastAttempt = millis();
                if (!MqttManager::connect()) {
                    Log.printf("MQTT connect failed, rc=%d\n", mqttClient.state());
                }
            }

            return;
        }

        mqttClient.loop();
    }

    bool requestUpdate(const MQTTConfig& newConfig) {
        ConfigManager::config.mqtt = newConfig;
        ConfigManager::save();
        reload();

        Log.println();
        Log.println("MQTT configuration updated");
        Log.println(ConfigManager::config.mqtt.toString().c_str());

        return true;
    }

    bool publish(const String& topic, const String& payload, bool retain) {
        if (!mqttClient.connected())
            return false;

        // Log.printf("Publishing to MQTT topic: %s, payload: %s\n", topic.c_str(),
        // payload.c_str());
        return mqttClient.publish(topic.c_str(), payload.c_str(), retain);
    }

    bool publish(const String& topic, const JsonDocument& payload, bool retain) {
        String payloadString;
        serializeJson(payload, payloadString);
        return publish(topic, payloadString, retain);
    }

    void publishAll() {
        for (const auto& device : ConfigManager::config.modbusDevices) {
            MqttDiscovery::publishDevice(device);
        }
        MqttDiscovery::publishDevice(SystemManager::systemDevice);
    }

    void subscribeAll() {
        for (const auto& device : ConfigManager::config.modbusDevices) {
            for (const auto& reg : device.numberWriteRegisters) {
                mqttClient.subscribe(reg.discovery.commandTopic.c_str());
            }

            for (const auto& reg : device.selectWriteRegisters) {
                mqttClient.subscribe(reg.discovery.commandTopic.c_str());
            }
        }

        for (const auto& control : SystemManager::systemDevice.controls) {
            mqttClient.subscribe(control->discovery.commandTopic.c_str());
        }
    }

    void callback(char* topic, byte* payload, unsigned int length) {
        Log.printf(
            "Received MQTT message on topic: %s, payload: %.*s\n", topic, length, (char*)payload);

        for (auto& device : ConfigManager::config.modbusDevices) {
            device.execute(topic, payload, length);
        }

        SystemManager::systemDevice.execute(topic, payload, length);
    }
}  // namespace MqttManager