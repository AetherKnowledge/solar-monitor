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
    // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    SystemManager::setup();
    ConfigManager::load();

    if (ConfigManager::hasLoaded) {
        NetworkManager::connect(ConfigManager::config.network.ssid,
                                ConfigManager::config.network.password,
                                ConfigManager::config.network.mode);
        ModbusManager::setup();
        MqttManager::setup();
    }

    DisplayManager::setup();
    SoundManager::setup();
    WebServer::start();
}

void loop() {
    SystemManager::loop();
    NetworkManager::loop();
    ModbusManager::loop();
    MqttManager::loop();
    DisplayManager::loop();
    SoundManager::loop();

    delay(10);
}