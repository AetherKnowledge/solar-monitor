#include "NetworkApi.h"
#include <Networking/Networking.h>
#include <WiFi.h>
#include <Common/Json.h>
#include <Common/Network.h>
#include <Config/Config.h>

void registerNetworkApi(AsyncWebServer& server) {
    server.on("/api/network/wifinetworks", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleGetWifiNetworks(request);
    });

    server.on("/api/network/wifinetworks/scan", HTTP_POST, [](AsyncWebServerRequest* request) {
        handleScanWifiNetworks(request);
    });

    server.on("/api/network/config", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleGetNetworkConfig(request);
    });

    server.addHandler(new AsyncCallbackJsonWebHandler(
        "/api/network/config", [](AsyncWebServerRequest* request, JsonVariant& json) {
            handleUpdateNetworkConfig(request, json);
        }));

    Serial.println("Network API registered");
}

void handleGetWifiNetworks(AsyncWebServerRequest* request) {
    JsonDocument doc;
    doc["status"] = Enum::toString(wifiScanStatus);
    serializeVector(doc["networks"], cachedWifiNetworks);

    String response;
    serializeJson(doc, response);

    Serial.println("WiFi Networks: " + response);

    request->send(200, "application/json", response);
}

void handleScanWifiNetworks(AsyncWebServerRequest* request) {
    if (wifiScanStatus != UpdateStatus::InProgress) {
        wifiScanRequested = true;
    }

    JsonDocument doc;
    doc["status"] = Enum::toString(wifiScanStatus);

    String response;
    serializeJson(doc, response);

    Serial.println("Scan WiFi Networks: " + response);

    request->send(200, "application/json", response);
}

void handleGetNetworkConfig(AsyncWebServerRequest* request) {
    JsonDocument doc;
    config.network.toJson(doc.to<JsonObject>());

    String response;
    serializeJson(doc, response);

    Serial.println("Network Config: " + response);

    request->send(200, "application/json", response);
}

void handleUpdateNetworkConfig(AsyncWebServerRequest* request, JsonVariant& json) {
    NetworkConfig newConfig;
    newConfig.fromJson(json);
    networkUpdateRequested = true;
    pendingNetworkConfig = newConfig;

    Response::success(request, 202, "OK");
}