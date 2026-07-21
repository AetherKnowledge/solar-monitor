#include <Arduino.h>
#include <Networking/NetworkManager.h>
#include <WebServer/WebServer.h>
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>
#include <Modbus/ModbusManager.h>
#include <LittleFS.h>
#include <System/SystemManager.h>

#define LED_PIN 2

void setup() {
    Serial.begin(115200);
    // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    if (!LittleFS.begin()) {
        Serial.println("Mount failed");
    }

    ConfigManager::load();

    if (ConfigManager::hasLoaded) {
        NetworkManager::connect(ConfigManager::config.network.ssid,
                                ConfigManager::config.network.password,
                                ConfigManager::config.network.mode);
        ModbusManager::setup();
        MqttManager::setup();
    }

    startWebServer();
}

void loop() {
    SystemManager::loop();
    NetworkManager::loop();
    ModbusManager::loop();
    MqttManager::loop();

    delay(10);
}