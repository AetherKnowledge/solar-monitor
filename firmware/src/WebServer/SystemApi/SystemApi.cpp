#include "SystemApi.h"
#include <System/SystemManager.h>
#include <Common/Network.h>
#include <Common/Logger.h>

namespace SystemApi {
    void registerApi(PsychicHttpServer& server) {
        server.on("/api/system/restart", HTTP_POST, handleRestartRequest);

        server.on("/api/system", HTTP_GET, handleGetSystemData);

        Log.println("System API registered");
    }
    esp_err_t handleRestartRequest(PsychicRequest* request, PsychicResponse* response) {
        // Set the requestRestart flag in SystemManager
        SystemManager::requestRestart();
        return Response::success(response, 202, "OK");
    }

    esp_err_t handleGetSystemData(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        SystemManager::systemDevice.toJson(doc);

        return Response::sendJson(response, doc);
    }
}  // namespace SystemApi