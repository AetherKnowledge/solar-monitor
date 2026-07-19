#include "MqttDiscovery.h"
#include <Config/Config.h>
#include <PubSubClient.h>
#include <WiFi.h>

static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);

namespace MqttManager {
    bool isConnected() {
        return mqttClient.connected();
    }

    void setup() {
        Serial.println();
        Serial.println("Setting up MQTT");

        mqttClient.setServer(config.mqtt.host.c_str(), config.mqtt.port);

        mqttClient.setBufferSize(1024);
    }

    bool connect() {
        Serial.println();
        Serial.println("Connecting to MQTT");

        MQTTConfig& mqttConfig = config.mqtt;
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
            Serial.println();
            Serial.println("Connected to MQTT");
            mqttClient.publish("solar-monitor/status", "online", true);

            MqttDiscovery::start();
        } else {
            Serial.println();
            Serial.println("Failed to connect to MQTT");
        }

        return connected;
    }

    void disconnect() {
        Serial.println();
        Serial.println("Disconnecting from MQTT");

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
                if (!connect()) {
                    Serial.printf("MQTT connect failed, rc=%d\n", mqttClient.state());
                }
            }

            return;
        }

        mqttClient.loop();
    }

    bool publish(const String& topic, const String& payload, bool retain = true) {
        if (!mqttClient.connected())
            return false;

        // Serial.printf("Publishing to MQTT topic: %s, payload: %s\n", topic.c_str(),
        // payload.c_str());
        return mqttClient.publish(topic.c_str(), payload.c_str(), retain);
    }

    bool publish(const String& topic, const JsonDocument& payload, bool retain = true) {
        String payloadString;
        serializeJson(payload, payloadString);
        return publish(topic, payloadString, retain);
    }

    void callback(char* topic, byte* payload, unsigned int length) {}
}  // namespace MqttManager