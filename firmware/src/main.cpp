#include <Arduino.h>
#include <Networking/Networking.h>
#include <WebServer/WebServer.h>
#include <Config/Config.h>

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

    if (wifiScanStatus == InProgress)
    {
        scanNetworks();
    }

    if (networkUpdateRequested)
    {
        updateNetworkConfig(pendingNetworkConfig);
        networkUpdateRequested = false;
    }

    delay(10);
}