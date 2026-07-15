#include "NetworkApi.h"

void registerNetworkApi(AsyncWebServer &server)
{
    server.on("/api/config/network", HTTP_GET,
              [](AsyncWebServerRequest *request)
              { handleGetNetworkConfig(request); });

    server.on(
        "/api/config/network",
        HTTP_POST,
        [](AsyncWebServerRequest *) {},
        nullptr,
        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t,
           size_t)
        { handleUpdateNetworkConfig(request, data, len); });
}

void handleGetNetworkConfig(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    config.network.fromJson(doc.to<JsonObject>());

    String response;
    serializeJson(doc, response);

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