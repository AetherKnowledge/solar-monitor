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
            [](AsyncWebServerRequest* request) { UpdateHandler::onFirmwareUpdateFinish(request); },

            // Upload callback - unused when sending raw octet-stream
            nullptr,

            // Body callback
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) {
                UpdateHandler::onFirmwareUpdate(request, data, len, index, total);
            });

        server.on(
            "/api/update/website",
            HTTP_POST,

            // Called when request finishes
            [](AsyncWebServerRequest* request) { UpdateHandler::onWebsiteUpdateFinish(request); },

            // Upload callback
            nullptr,

            // Raw body callback
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) { UpdateHandler::onWebsiteUpdate(request, data, len, index, total); });

        server.on("/api/version", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetVersion(request);
        });

        Log.println("Update API registered");
    }

    void handleGetVersion(AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["firmware"] = Version::FIRMWARE;
        doc["website"] = WebServer::WEBSITE_VERSION;

        String response;
        serializeJson(doc, response);

        Log.println("Version: " + response);

        request->send(200, "application/json", response);
    }

}  // namespace UpdateApi