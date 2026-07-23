#include <Arduino.h>
#include <Networking/NetworkManager.h>
#include <WebServer/WebServer.h>
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>
#include <Modbus/ModbusManager.h>
#include <System/SystemManager.h>
#include <Display/DisplayManager.h>
#include <Sound/SoundManager.h>
#include <Common/Logger.h>
#include "Config/ConfigManager.h"

#define LED_PIN 32

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    DisplayManager::setup();
    SystemManager::setup();
    SoundManager::setup();

    DisplayManager::showLoadingProgress("Loading Config", 0);
    if (ConfigManager::load()) {
        DisplayManager::showLoadingProgress("Connecting to WiFi", 40);
        NetworkManager::connect(ConfigManager::config.network.ssid,
                                ConfigManager::config.network.password,
                                ConfigManager::config.network.mode);

        DisplayManager::showLoadingProgress("Starting Modbus", 60);
        ModbusManager::setup();

        DisplayManager::showLoadingProgress("Starting MQTT", 80);
        MqttManager::setup();
    }

    DisplayManager::showLoadingProgress("Starting Web Server", 90);
    WebServer::start();

    DisplayManager::showLoadingProgress("Startup Complete", 100);
    delay(500);
}

void loop() {
    SystemManager::loop();
    NetworkManager::loop();
    ModbusManager::loop();
    MqttManager::loop();
    DisplayManager::loop();
    SoundManager::loop();

    DisplayManager::showLoadingSpinner("Running");
}