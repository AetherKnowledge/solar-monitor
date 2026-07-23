#include "SystemApi.h"
#include <System/SystemManager.h>
#include <Common/Network.h>
#include <Common/Logger.h>

namespace SystemApi {
    void registerApi(AsyncWebServer& server) {
        server.on("/api/system/restart", HTTP_POST, [](AsyncWebServerRequest* request) {
            handleRestartRequest(request);
        });

        Log.println("System API registered");
    }
    void handleRestartRequest(AsyncWebServerRequest* request) {
        // Set the requestRestart flag in SystemManager
        SystemManager::requestRestart();
        Response::success(request);
    }
}  // namespace SystemApi