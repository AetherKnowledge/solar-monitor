#include <Arduino.h>
#include <WiFi.h>
#include <WebServer/WebServer.h>

const char *ssid = "GlobeAtHome";
const char *password = "donotopen8";

#define LED_PIN 2

void setup()
{
    Serial.begin(115200);
    // put your setup code here, to run once:
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    WiFi.begin(ssid, password);

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

    startWebServer();
}

void loop()
{
}