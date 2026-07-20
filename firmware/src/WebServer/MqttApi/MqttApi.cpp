#include "MqttApi.h"
#include <Config/Config.h>
#include <Common/Network.h>

void registerMqttApi(AsyncWebServer& server) {
    server.on("/api/mqtt/config", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleGetMqttConfig(request);
    });

    server.addHandler(new AsyncCallbackJsonWebHandler(
        "/api/mqtt/config", [](AsyncWebServerRequest* request, JsonVariant& json) {
            handleUpdateMqttConfig(request, json);
        }));

    Serial.println("MQTT API registered");
}

void handleGetMqttConfig(AsyncWebServerRequest* request) {
    JsonDocument doc;
    config.mqtt.toJson(doc.to<JsonObject>());

    String response;
    serializeJson(doc, response);

    Serial.println("MQTT Config: " + response);

    request->send(200, "application/json", response);
}

void handleUpdateMqttConfig(AsyncWebServerRequest* request, JsonVariant& json) {
    MQTTConfig newConfig;
    newConfig.fromJson(json);

    updateMqttConfig(newConfig);

    Response::success(request, 202, "OK");
}