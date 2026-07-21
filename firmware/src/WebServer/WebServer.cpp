#include "WebServer.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "NetworkApi/NetworkApi.h"
#include "MqttApi/MqttApi.h"
#include "ConfigApi/ConfigApi.h"
#include "ModbusApi/ModbusApi.h"
#include "UpdateApi/UpdateApi.h"

AsyncWebServer server(80);
fs::LittleFSFS WebFS;

bool startWebServer() {
    Serial.println("Starting web server");

    bool fsMounted = WebFS.begin(false, "/website", 10, "website");

    if (!fsMounted) {
        Serial.println("LittleFS Mount Failed");
        return false;
    }

    registerWebServerApis();

    server.serveStatic("/_app", WebFS, "/_app")
        .setCacheControl("public, max-age=31536000, immutable")
        .setTryGzipFirst(true);

    server.serveStatic("/roboto.woff2", WebFS, "/roboto.woff2")
        .setCacheControl("public, max-age=31536000, immutable")
        .setTryGzipFirst(true);

    server.serveStatic("/", WebFS, "/")
        .setDefaultFile("index.html")
        .setCacheControl("no-cache")
        .setTryGzipFirst(true);

    server.onNotFound([](AsyncWebServerRequest* request) {
        if (request->url().startsWith("/api/")) {
            request->send(404);
            return;
        }

        AsyncWebServerResponse* response =
            request->beginResponse(WebFS, "/index.html.gz", "text/html");

        response->addHeader("Cache-Control", "no-cache");
        response->addHeader("Content-Encoding", "gzip");

        request->send(response);
    });

    server.begin();

    Serial.println("Web server started");
    return true;
}

void registerWebServerApis() {
    NetworkApi::registerApi(server);
    MqttApi::registerApi(server);
    ConfigApi::registerApi(server);
    ModbusApi::registerApi(server);
    UpdateApi::registerApi(server);
}

bool stopWebServer() {
    server.end();
    Serial.println("Web server stopped");
    return true;
}