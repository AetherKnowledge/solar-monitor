#include <Arduino.h>
#include <Networking/NetworkManager.h>
#include <WebServer/WebServer.h>
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>
#include <Modbus/ModbusManager.h>
#include <LittleFS.h>

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
    if (NetworkManager::scanStatus == UpdateStatus::Requested) {
        NetworkManager::startScanning();
    }

    if (NetworkManager::scanStatus == UpdateStatus::InProgress) {
        NetworkManager::pollNetworkScan();
    }

    if (NetworkManager::updateStatus == UpdateStatus::Requested) {
        NetworkManager::updateConfig();
    }

    if (ModbusManager::updateStatus == UpdateStatus::Requested) {
        ModbusManager::updateConfig(ModbusManager::pendingDevices);
    }

    ModbusManager::loop();
    MqttManager::loop();

    delay(10);
}