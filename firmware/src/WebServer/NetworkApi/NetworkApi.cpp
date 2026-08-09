#include "NetworkApi.h"
#include <Networking/NetworkService.h>
#include <WiFi.h>
#include <Common/Json.h>
#include <Common/Network.h>
#include <Config/ConfigManager.h>
#include <Common/Logger.h>

namespace NetworkApi {
    void registerRoutes(PsychicHttpServer& server) {
        server.on("/api/network/wifinetworks", HTTP_GET, handleGetNetworks);

        server.on("/api/network/wifinetworks/scan", HTTP_POST, handleScanNetworks);

        server.on("/api/network/config", HTTP_GET, handleGetConfig);

        server.on("/api/network/config", HTTP_POST, handleUpdateConfig);

        Log.println("Network API registered");
    }

    esp_err_t handleGetNetworks(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        doc["status"] = Enum::toString(NetworkService::scanStatus);
        serializeVector(doc["networks"], NetworkService::cachedWifiNetworks);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleScanNetworks(PsychicRequest* request, PsychicResponse* response) {
        if (NetworkService::scanStatus != UpdateStatus::InProgress) {
            NetworkService::scanStatus = UpdateStatus::Requested;
        }

        JsonDocument doc;
        doc["status"] = Enum::toString(NetworkService::scanStatus);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        ConfigManager::config.network.toJson(doc.to<JsonObject>());
        doc["updateStatus"] = Enum::toString(NetworkService::updateStatus);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleUpdateConfig(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json) {
        NetworkService::requestUpdate(json);
        return Response::success(response, "OK", 202);
    }
}  // namespace NetworkApi