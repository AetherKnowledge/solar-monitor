#include <Arduino.h>
#include <Networking/Networking.h>
#include <WebServer/WebServer.h>
#include <Config/Config.h>
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

    loadConfig();

    if (configLoaded) {
        connectToWiFi(config.network.ssid, config.network.password, config.network.mode);
        ModbusManager::setup();
        MqttManager::setup();
    }

    startWebServer();
}

void loop() {
    if (wifiScanRequested) {
        startScanning();
        wifiScanRequested = false;
    }

    if (wifiScanStatus == UpdateStatus::InProgress) {
        scanNetworks();
    }

    if (networkUpdateRequested) {
        updateNetworkConfig(pendingNetworkConfig);
        networkUpdateRequested = false;
    }

    ModbusManager::loop();
    MqttManager::loop();

    delay(10);
}