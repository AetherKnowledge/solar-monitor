#include "MqttApi.h"
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>
#include <Common/Network.h>
#include <Common/Logger.h>

namespace MqttApi {
    void registerApi(AsyncWebServer& server) {
        server.on("/api/mqtt/config", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetConfig(request);
        });

        server.addHandler(new AsyncCallbackJsonWebHandler(
            "/api/mqtt/config", [](AsyncWebServerRequest* request, JsonVariant& json) {
                handleUpdateConfig(request, json);
            }));

        Log.println("MQTT API registered");
    }

    void handleGetConfig(AsyncWebServerRequest* request) {
        JsonDocument doc;
        ConfigManager::config.mqtt.toJson(doc.to<JsonObject>());

        String response;
        serializeJson(doc, response);

        Log.println("MQTT Config: " + response);

        request->send(200, "application/json", response);
    }

    void handleUpdateConfig(AsyncWebServerRequest* request, JsonVariant& json) {
        MQTTConfig newConfig;
        newConfig.fromJson(json);

        MqttManager::requestUpdate(newConfig);

        Response::success(request, 202, "OK");
    }
}  // namespace MqttApi