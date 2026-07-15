#include "MqttApi.h"

void registerMqttApi(AsyncWebServer &server)
{
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

    Serial.println("MQTT API registered");
}

void handleGetMqttConfig(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    config.mqtt.fromJson(doc.to<JsonObject>());

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
        request->send(400, "text/plain", "Invalid JSON");
        return;
    }

    MQTTConfig newConfig;
    newConfig.fromJson(doc.as<JsonObjectConst>());

    setMqttConfig(newConfig);

    request->send(200, "text/plain", "OK");
}