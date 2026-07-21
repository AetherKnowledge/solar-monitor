#include "MqttDiscovery.h"
#include "MqttTypes.h"
#include <Config/ConfigManager.h>
#include <PubSubClient.h>
#include "MqttManager.h"
#include <WiFi.h>
#include <Modbus/WriteRegisterManager.h>
#include <Common/Logger.h>

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

        // This is also here to prevent race condition of modbus devices being updated before MQTT
        // topics are generated
        generateTopics();
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

            generateTopics();
            subscribeAll();
            MqttDiscovery::start();
        } else {
            Log.println();
            Log.println("Failed to connect to MQTT");
        }
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

    void generateTopics() {
        for (auto& device : ConfigManager::config.modbusDevices) {
            for (auto& reg : device.readRegisters) {
                reg.discovery.stateTopic = MqttDiscovery::generateStateTopic(
                    device.discovery.identifier, reg.discovery.uniqueId);
            }

            for (auto& sensor : device.virtualSensors) {
                sensor.discovery.stateTopic = MqttDiscovery::generateStateTopic(
                    device.discovery.identifier, sensor.discovery.uniqueId);
            }

            for (auto& reg : device.numberWriteRegisters) {
                reg.discovery.commandTopic = MqttDiscovery::generateCommandTopic(
                    device.discovery.identifier, reg.discovery.uniqueId);
                reg.discovery.stateTopic = MqttDiscovery::generateStateTopic(
                    device.discovery.identifier, reg.discovery.uniqueId);
            }

            for (auto& reg : device.selectWriteRegisters) {
                reg.discovery.commandTopic = MqttDiscovery::generateCommandTopic(
                    device.discovery.identifier, reg.discovery.uniqueId);
                reg.discovery.stateTopic = MqttDiscovery::generateStateTopic(
                    device.discovery.identifier, reg.discovery.uniqueId);
            }
        }
    }

    bool updateMqttConfig(const MQTTConfig& newConfig) {
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

    void subscribeAll() {
        for (const auto& device : ConfigManager::config.modbusDevices) {
            for (const auto& reg : device.numberWriteRegisters) {
                mqttClient.subscribe(reg.discovery.commandTopic.c_str());
            }

            for (const auto& reg : device.selectWriteRegisters) {
                mqttClient.subscribe(reg.discovery.commandTopic.c_str());
            }
        }
    }

    template <WriteRegister TRegister>
    bool usePayload(
        char* topic, byte* payload, unsigned int length, TRegister& reg, ModbusDevice& device) {
        if (!reg.discovery.commandTopic.equals(topic)) {
            return false;
        }

        String value((char*)payload, length);
        bool result = WriteRegisterManager::writeRegister(device, reg, value.toDouble());

        if (result) {
            publish(reg.discovery.stateTopic, value, true);
        }

        Log.printf("Received payload for %s (%s) of device %s (%s): %s\n",
                   reg.discovery.name.c_str(),
                   reg.discovery.uniqueId.c_str(),
                   device.discovery.name.c_str(),
                   device.discovery.identifier.c_str(),
                   value.c_str());

        return result;
    }

    void callback(char* topic, byte* payload, unsigned int length) {
        for (auto& device : ConfigManager::config.modbusDevices) {
            for (auto& reg : device.numberWriteRegisters) {
                if (usePayload(topic, payload, length, reg, device))
                    return;
            }

            for (auto& reg : device.selectWriteRegisters) {
                if (usePayload(topic, payload, length, reg, device))
                    return;
            }
        }
    }
}  // namespace MqttManager