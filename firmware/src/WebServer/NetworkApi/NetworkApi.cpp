#include "NetworkApi.h"
#include <Networking/Networking.h>
#include <WiFi.h>

void registerNetworkApi(AsyncWebServer &server)
{
    server.on("/api/network/config", HTTP_GET,
              [](AsyncWebServerRequest *request)
              { handleGetNetworkConfig(request); });

    server.on(
        "/api/network/config",
        HTTP_POST,
        [](AsyncWebServerRequest *) {},
        nullptr,
        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t,
           size_t)
        { handleUpdateNetworkConfig(request, data, len); });

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

void handleGetNetworkConfig(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    config.network.toJson(doc.to<JsonObject>());

    String response;
    serializeJson(doc, response);

    Serial.println("Network Config: " + response);

    request->send(200, "application/json", response);
}

void handleUpdateNetworkConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len)
{
    JsonDocument doc;

    if (deserializeJson(doc, data, len))
    {
        request->send(400, "text/plain", "Invalid JSON");
        return;
    }

    NetworkConfig newConfig;
    newConfig.fromJson(doc.as<JsonObjectConst>());

    setNetworkConfig(newConfig);

    request->send(200, "text/plain", "OK");
}

void handleGetWifiNetworks(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    doc["status"] = scanStatusToString(wifiScanStatus);
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
    if (wifiScanStatus != SCAN_STATUS_IN_PROGRESS)
    {
        wifiScanRequested = true;
    }

    JsonDocument doc;
    doc["status"] = scanStatusToString(wifiScanStatus);

    String response;
    serializeJson(doc, response);

    Serial.println("Scan WiFi Networks: " + response);

    request->send(200, "application/json", response);
}