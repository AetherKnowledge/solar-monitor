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

    server.serveStatic("/_app", LittleFS, "/website/_app")
        .setCacheControl("public, max-age=31536000, immutable")
        .setTryGzipFirst(true);

    server.serveStatic("/roboto.woff2", LittleFS, "/website/roboto.woff2")
        .setCacheControl("public, max-age=31536000, immutable")
        .setTryGzipFirst(true);

    server.serveStatic("/", LittleFS, "/website")
        .setDefaultFile("index.html")
        .setCacheControl("no-cache")
        .setTryGzipFirst(true);

    server.onNotFound([](AsyncWebServerRequest *request)
                      {
    if (request->url().startsWith("/api/"))
    {
        request->send(404);
        return;
    }

    AsyncWebServerResponse *response =
        request->beginResponse(LittleFS,
                               "/website/index.html.gz",
                               "text/html");

    response->addHeader("Cache-Control", "no-cache");
    response->addHeader("Content-Encoding", "gzip");

    request->send(response); });

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