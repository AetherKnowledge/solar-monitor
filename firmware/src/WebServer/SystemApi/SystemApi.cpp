#include "SystemApi.h"
#include <System/SystemManager.h>
#include <Common/Network.h>
#include <Common/Logger.h>
#include <Config/ConfigManager.h>
#include "Version.h"

namespace SystemApi {
    void registerRoutes(PsychicHttpServer& server) {
        server.on("/api/system/restart", HTTP_POST, handleRestartRequest);

        server.on("/api/system", HTTP_GET, handleGetSystemData);

        server.on("/api/system/board", HTTP_GET, handleGetBoardType);

        Log.println("System API registered");
    }
    esp_err_t handleRestartRequest(PsychicRequest* request, PsychicResponse* response) {
        // Set the requestRestart flag in SystemManager
        SystemManager::requestRestart();
        return Response::success(response, "OK", 202);
    }

    esp_err_t handleGetSystemData(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        SystemManager::systemDevice.toJson(doc);

        return Response::send(response, doc);
    }

    esp_err_t handleGetBoardType(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        doc["boardType"] = Version::BOARD;

        return Response::send(response, doc);
    }
}  // namespace SystemApi