#include "NetworkApi.h"
#include <Networking/NetworkManager.h>
#include <WiFi.h>
#include <Common/Json.h>
#include <Common/Network.h>
#include <Config/ConfigManager.h>
#include <Common/Logger.h>

namespace NetworkApi {
    void registerApi(AsyncWebServer& server) {
        server.on("/api/network/wifinetworks", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetNetworks(request);
        });

        server.on("/api/network/wifinetworks/scan", HTTP_POST, [](AsyncWebServerRequest* request) {
            handleScanNetworks(request);
        });

        server.on("/api/network/config", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetConfig(request);
        });

        server.addHandler(new AsyncCallbackJsonWebHandler(
            "/api/network/config", [](AsyncWebServerRequest* request, JsonVariant& json) {
                handleUpdateConfig(request, json);
            }));

        Log.println("Network API registered");
    }

    void handleGetNetworks(AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["status"] = Enum::toString(NetworkManager::scanStatus);
        serializeVector(doc["networks"], NetworkManager::cachedWifiNetworks);

        String response;
        serializeJson(doc, response);

        Log.println("WiFi Networks: " + response);

        request->send(200, "application/json", response);
    }

    void handleScanNetworks(AsyncWebServerRequest* request) {
        if (NetworkManager::scanStatus != UpdateStatus::InProgress) {
            NetworkManager::scanStatus = UpdateStatus::Requested;
        }

        JsonDocument doc;
        doc["status"] = Enum::toString(NetworkManager::scanStatus);

        String response;
        serializeJson(doc, response);

        Log.println("Scan WiFi Networks: " + response);

        request->send(200, "application/json", response);
    }

    void handleGetConfig(AsyncWebServerRequest* request) {
        JsonDocument doc;
        ConfigManager::config.network.toJson(doc.to<JsonObject>());
        doc["updateStatus"] = Enum::toString(NetworkManager::updateStatus);

        String response;
        serializeJson(doc, response);

        Log.println("Network Config: " + response);

        request->send(200, "application/json", response);
    }

    void handleUpdateConfig(AsyncWebServerRequest* request, JsonVariant& json) {
        NetworkConfig newConfig;
        newConfig.fromJson(json);
        NetworkManager::updateStatus = UpdateStatus::Requested;
        NetworkManager::pendingConfig = newConfig;

        Response::success(request, 202, "OK");
    }
}  // namespace NetworkApi