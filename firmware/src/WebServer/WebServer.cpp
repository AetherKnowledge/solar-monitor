#include "WebServer.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "NetworkApi/NetworkApi.h"
#include "MqttApi/MqttApi.h"
#include "ConfigApi/ConfigApi.h"

AsyncWebServer server(80);

bool startWebServer()
{
    Serial.println("Starting web server");

    if (!LittleFS.begin())
    {
        Serial.println("LittleFS Mount Failed");
        return false;
    }

    registerWebServerApis();

    server.serveStatic("/", LittleFS, "/website")
        .setDefaultFile("index.html")
        .setTryGzipFirst(true);

    server.begin();

    Serial.println("Web server started");
    return true;
}

void registerWebServerApis()
{
    registerNetworkApi(server);
    registerMqttApi(server);
    registerConfigApi(server);
}

bool stopWebServer()
{
    server.end();
    Serial.println("Web server stopped");
    return true;
}