#include "UpdateApi.h"
#include <Common/Network.h>
#include <Update/UpdateHandler.h>
#include <Version.h>
#include <WebServer/WebServer.h>
#include <Common/Logger.h>
#include <Update/UpdateHandler.h>

namespace UpdateApi {
    void registerApi(AsyncWebServer& server) {
        server.on("/api/update/status", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetStatus(request);
        });

        server.on(
            "/api/update/firmware/manual",
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
            "/api/update/website/manual",
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

        server.on(
            "/api/update/firmware",
            HTTP_POST,
            [](AsyncWebServerRequest* request) {
                // response sent later
            },
            nullptr,
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) { handleLatestUpdate(request, data, len, index, total, true); });

        server.on(
            "/api/update/website",
            HTTP_POST,
            [](AsyncWebServerRequest* request) {
                // response sent later
            },
            nullptr,
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) { handleLatestUpdate(request, data, len, index, total, false); });

        server.on("/api/version", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetVersion(request);
        });

        Log.println("Update API registered");
    }

    void handleGetStatus(AsyncWebServerRequest* request) {
        JsonDocument doc;
        UpdateHandler::getUpdateProgress().toJson(doc.to<JsonObject>());
        Response::sendJson(request, doc);
    }

    void handleGetVersion(AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["firmware"] = Version::FIRMWARE;
        doc["website"] = WebServer::WEBSITE_VERSION;

        Response::sendJson(request, doc);
    }

    void handleLatestUpdate(AsyncWebServerRequest* request,
                            uint8_t* data,
                            size_t len,
                            size_t index,
                            size_t total,
                            bool isFirmware) {
        JsonDocument doc;

        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            Response::send(request, 400, "Invalid JSON");
            return;
        }

        UpdateHandler::UpdateRequest updateRequest;
        updateRequest.fromJson(doc);

        Log.println("Received update request: " + updateRequest.toString());

        const String latestVersion = updateRequest.version;

        const String currentVersion =
            isFirmware ? String("v") + Version::FIRMWARE : String("v") + WebServer::WEBSITE_VERSION;

        switch (UpdateHandler::compareVersions(currentVersion, latestVersion)) {
            case 1:
                Response::send(request, 409, "Current version is newer than requested version");
                return;

            case 0:
                Response::send(request, 409, "Already up to date");
                return;

            case -1:
                break;  // Proceed with update
        }

        if (UpdateHandler::requestUpdate(updateRequest, isFirmware)) {
            Response::send(request, 200, "Update started successfully");
        } else {
            Response::send(request, 500, "Failed to start update");
        }
    }

}  // namespace UpdateApi