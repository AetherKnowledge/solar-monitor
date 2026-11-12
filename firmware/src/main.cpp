#include <Arduino.h>
#include <Networking/NetworkService.h>
#include <WebServer/WebServer.h>
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>
#include <Modbus/ModbusManager.h>
#include <System/SystemManager.h>
#include <Display/DisplayManager.h>
#include <Sound/SoundManager.h>
#include <Common/Logger.h>
#include "Config/ConfigManager.h"
#include "Display/DisplayManager.h"
#include "Update/UpdateHandler.h"
#include "Pins.h"

void setup() {
    Serial.begin(115200);
    pinMode(Pins::STATUS_LED, OUTPUT);
    digitalWrite(Pins::STATUS_LED, HIGH);

    DisplayManager::setup();
    SystemManager::setup();
    SoundManager::setup();

    DisplayManager::showLoadingProgress("Loading Config", 0);
    if (ConfigManager::load()) {
        DisplayManager::showLoadingProgress("Connecting to WiFi", 40);
        NetworkService::connect(ConfigManager::config.network);

        DisplayManager::showLoadingProgress("Starting Modbus", 60);
        ModbusManager::setup();

        DisplayManager::showLoadingProgress("Starting MQTT", 80);
        MqttManager::setup();
    }

    DisplayManager::showLoadingProgress("Starting Web Server", 90);
    WebServer::start();

    UpdateHandler::setup();
    DisplayManager::showLoadingProgress("Startup Complete", 100);
    DisplayManager::finishBoot();
    DisplayManager::showSuccess("Startup Complete");
    delay(500);
    DisplayManager::showDeviceInfo();
}

void loop() {
    SystemManager::loop();
    NetworkService::loop();
    ModbusManager::loop();
    MqttManager::loop();
    SoundManager::loop();
}