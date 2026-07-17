#include "ConfigApi.h"
#include <Config/Config.h>
#include <Common/Response.h>

void registerConfigApi(AsyncWebServer &server)
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

    server.on("/api/config/mqtt", HTTP_GET, [](AsyncWebServerRequest *request)
              { handleGetMqttConfig(request); });

    server.on(
        "/api/config/mqtt",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total)
        { handleUpdateMqttConfig(request, data, len); });

    Serial.println("Config API registered");
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
        Response::error(request, 400, "Invalid JSON");
        return;
    }

    NetworkConfig newConfig;
    newConfig.fromJson(doc.as<JsonObjectConst>());
    networkUpdateRequested = true;
    pendingNetworkConfig = newConfig;

    Response::success(request, 202, "OK");
}

void handleGetMqttConfig(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    config.mqtt.toJson(doc.to<JsonObject>());

    String response;
    serializeJson(doc, response);

    Serial.println("MQTT Config: " + response);

    request->send(200, "application/json", response);
}

void handleUpdateMqttConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len)
{
    JsonDocument doc;

    if (deserializeJson(doc, data, len))
    {
        Response::error(request, 400, "Invalid JSON");
        return;
    }

    MQTTConfig newConfig;
    newConfig.fromJson(doc.as<JsonObjectConst>());

    updateMqttConfig(newConfig);

    Response::success(request, 200, "OK");
}