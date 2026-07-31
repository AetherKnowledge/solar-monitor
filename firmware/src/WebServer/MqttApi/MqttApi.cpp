#include "MqttApi.h"
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>
#include <Common/Network.h>
#include <Common/Logger.h>

namespace MqttApi {
    void registerRoutes(PsychicHttpServer& server) {
        server.on("/api/mqtt/config", HTTP_GET, handleGetConfig);

        server.on("/api/mqtt/config", HTTP_POST, handleUpdateConfig);

        Log.println("MQTT API registered");
    }

    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        ConfigManager::config.mqtt.toJson(doc.to<JsonObject>());

        return Response::sendJson(response, doc);
    }

    esp_err_t handleUpdateConfig(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json) {
        MqttManager::requestUpdate(json);
        return Response::success(response, 202, "OK");
    }
}  // namespace MqttApi