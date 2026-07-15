#include "WebServer.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Config/Config.h>

AsyncWebServer server(80);

bool startWebServer()
{
    if (!LittleFS.begin())
    {
        Serial.println("LittleFS Mount Failed");
        return false;
    }

    server.serveStatic("/", LittleFS, "/website")
        .setDefaultFile("index.html")
        .setTryGzipFirst(true);

    server.begin();

    Serial.println("Web server started");
    return true;
}

bool stopWebServer()
{
    server.end();
    Serial.println("Web server stopped");
    return true;
}