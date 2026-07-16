#include <Arduino.h>
#include <Networking/Networking.h>
#include <WebServer/WebServer.h>
#include <Config/Config.h>
#include <Mqtt/MqttManager.h>

#define LED_PIN 2

void setup()
{
    Serial.begin(115200);
    // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    loadConfig();

    if (configLoaded)
    {
        connectToWiFi(config.network.ssid, config.network.password, config.network.mode);
        MqttManager::setup();
        MqttManager::connect();
    }

    startWebServer();
}

void loop()
{
    if (wifiScanRequested)
    {
        startScanning();
        wifiScanRequested = false;
    }

    if (wifiScanStatus == UpdateStatus::InProgress)
    {
        scanNetworks();
    }

    if (networkUpdateRequested)
    {
        updateNetworkConfig(pendingNetworkConfig);
        networkUpdateRequested = false;
    }

    MqttManager::loop();

    static unsigned long lastPublish = 0;

    if (MqttManager::isConnected() && millis() - lastPublish >= 10000)
    {
        lastPublish = millis();
        MqttManager::publish("solar-monitor/uptime", millis() / 1000UL);
    }

    delay(10);
}