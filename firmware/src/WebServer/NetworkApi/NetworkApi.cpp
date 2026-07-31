#include "NetworkApi.h"
#include <Networking/NetworkManager.h>
#include <WiFi.h>
#include <Common/Json.h>
#include <Common/Network.h>
#include <Config/ConfigManager.h>
#include <Common/Logger.h>

namespace NetworkApi {
    void registerApi(PsychicHttpServer& server) {
        server.on("/api/network/wifinetworks", HTTP_GET, handleGetNetworks);

        server.on("/api/network/wifinetworks/scan", HTTP_POST, handleScanNetworks);

        server.on("/api/network/config", HTTP_GET, handleGetConfig);

        server.on("/api/network/config", HTTP_POST, handleUpdateConfig);

        Log.println("Network API registered");
    }

    esp_err_t handleGetNetworks(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        doc["status"] = Enum::toString(NetworkManager::scanStatus);
        serializeVector(doc["networks"], NetworkManager::cachedWifiNetworks);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleScanNetworks(PsychicRequest* request, PsychicResponse* response) {
        if (NetworkManager::scanStatus != UpdateStatus::InProgress) {
            NetworkManager::scanStatus = UpdateStatus::Requested;
        }

        JsonDocument doc;
        doc["status"] = Enum::toString(NetworkManager::scanStatus);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        ConfigManager::config.network.toJson(doc.to<JsonObject>());
        doc["updateStatus"] = Enum::toString(NetworkManager::updateStatus);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleUpdateConfig(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json) {
        NetworkConfig newConfig;
        newConfig.fromJson(json);

        NetworkManager::requestUpdate(newConfig);

        return Response::success(response, 202, "OK");
    }
}  // namespace NetworkApi