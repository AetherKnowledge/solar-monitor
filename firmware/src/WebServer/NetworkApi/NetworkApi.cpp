#include "NetworkApi.h"
#include <Networking/Networking.h>
#include <WiFi.h>

void registerNetworkApi(AsyncWebServer &server)
{
    server.on("/api/network/wifinetworks", HTTP_GET,
              [](AsyncWebServerRequest *request)
              { handleGetWifiNetworks(request); });

    server.on(
        "/api/network/wifinetworks/scan",
        HTTP_POST,
        [](AsyncWebServerRequest *request)
        {
            handleScanWifiNetworks(request);
        });

    Serial.println("Network API registered");
}

void handleGetWifiNetworks(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    doc["status"] = Enum::toString(wifiScanStatus);
    JsonArray networks = doc["networks"].to<JsonArray>();

    for (const auto &network : cachedWifiNetworks)
    {
        JsonObject json = networks.add<JsonObject>();
        network.toJson(json);
    }

    String response;
    serializeJson(doc, response);

    Serial.println("WiFi Networks: " + response);

    request->send(200, "application/json", response);
}

void handleScanWifiNetworks(AsyncWebServerRequest *request)
{
    if (wifiScanStatus != UpdateStatus::InProgress)
    {
        wifiScanRequested = true;
    }

    JsonDocument doc;
    doc["status"] = Enum::toString(wifiScanStatus);

    String response;
    serializeJson(doc, response);

    Serial.println("Scan WiFi Networks: " + response);

    request->send(200, "application/json", response);
}