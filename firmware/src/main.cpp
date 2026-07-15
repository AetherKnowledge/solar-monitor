#include <Arduino.h>
#include <WiFi.h>
#include <Networking/Networking.h>
#include <WebServer/WebServer.h>
#include <Config/Config.h>

const char *ssid = "GlobeAtHome";
const char *password = "donotopen8";

#define LED_PIN 2

void setup()
{
    Serial.begin(115200);
    // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);

    Serial.print("Connecting");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    loadConfig();
    startWebServer();
}

void loop()
{
    if (wifiScanRequested)
    {
        wifiScanRequested = false;
        startScanning();
    }

    scanNetworksAsync();
    delay(10);
}