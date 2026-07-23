#include "UpdateApi.h"
#include <Common/Network.h>
#include <Update.h>
#include <Update/UpdateHandler.h>
#include <System/SystemManager.h>
#include <Version.h>
#include <WebServer/WebServer.h>
#include <Common/Logger.h>

namespace UpdateApi {
    void registerApi(AsyncWebServer& server) {
        server.on(
            "/api/update/firmware",
            HTTP_POST,

            // Called when request finishes
            [](AsyncWebServerRequest* request) { UpdateHandler::onUpdateFinish(request, true); },

            // Upload callback - unused when sending raw octet-stream
            [](AsyncWebServerRequest* request,
               const String& filename,
               size_t index,
               uint8_t* data,
               size_t len,
               bool final) { UpdateHandler::onChunk(data, len, index, final, std::nullopt, true); },

            // Body callback
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) {
                UpdateHandler::onChunk(data, len, index, index + len == total, total, true);
            });

        server.on(
            "/api/update/website",
            HTTP_POST,

            // Called when request finishes
            [](AsyncWebServerRequest* request) { UpdateHandler::onUpdateFinish(request, false); },

            // Upload callback
            [](AsyncWebServerRequest* request,
               const String& filename,
               size_t index,
               uint8_t* data,
               size_t len,
               bool final) {
                UpdateHandler::onChunk(data, len, index, final, std::nullopt, false);
            },

            // Raw body callback
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) {
                UpdateHandler::onChunk(data, len, index, index + len == total, total, false);
            });

        server.on("/api/version", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetVersion(request);
        });

        Log.println("Update API registered");
    }

    void handleGetVersion(AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["firmware"] = Version::FIRMWARE;
        doc["website"] = WebServer::WEBSITE_VERSION;

        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    }

}  // namespace UpdateApi