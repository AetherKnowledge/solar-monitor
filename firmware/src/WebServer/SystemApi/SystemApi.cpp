#include "SystemApi.h"
#include <System/SystemManager.h>
#include <Common/Network.h>
#include <Common/Logger.h>

namespace SystemApi {
    void registerApi(AsyncWebServer& server) {
        server.on("/api/system/restart", HTTP_POST, [](AsyncWebServerRequest* request) {
            handleRestartRequest(request);
        });

        server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetSystemData(request);
        });

        Log.println("System API registered");
    }
    void handleRestartRequest(AsyncWebServerRequest* request) {
        // Set the requestRestart flag in SystemManager
        SystemManager::requestRestart();
        Response::success(request);
    }

    void handleGetSystemData(AsyncWebServerRequest* request) {
        JsonDocument doc;
        SystemManager::systemDevice.toJson(doc);

        Response::sendJson(request, doc);
    }
}  // namespace SystemApi