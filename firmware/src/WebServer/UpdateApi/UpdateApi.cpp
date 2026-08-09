#include "UpdateApi.h"
#include <Common/Network.h>
#include <Update/UpdateHandler.h>
#include <Version.h>
#include <WebServer/WebServer.h>
#include <Common/Logger.h>
#include <Update/UpdateHandler.h>

namespace UpdateApi {
    void registerRoutes(PsychicHttpServer& server) {
        server.on("/api/update/status", HTTP_GET, handleGetStatus);

        auto* firmwareUploadHandler = new PsychicUploadHandler();
        firmwareUploadHandler->onUpload([](PsychicRequest* request,
                                           const String& filename,
                                           uint64_t index,
                                           uint8_t* data,
                                           size_t len,
                                           bool last) {
            return UpdateHandler::onChunk(request, data, len, index, last, true);
        });
        firmwareUploadHandler->onRequest([](PsychicRequest* request, PsychicResponse* response) {
            return UpdateHandler::onUpdateFinish(request, response, true);
        });
        server.on("/api/update/firmware/manual", HTTP_POST, firmwareUploadHandler);

        auto* websiteUploadHandler = new PsychicUploadHandler();
        websiteUploadHandler->onUpload([](PsychicRequest* request,
                                          const String& filename,
                                          uint64_t index,
                                          uint8_t* data,
                                          size_t len,
                                          bool last) {
            return UpdateHandler::onChunk(request, data, len, index, last, false);
        });
        websiteUploadHandler->onRequest([](PsychicRequest* request, PsychicResponse* response) {
            return UpdateHandler::onUpdateFinish(request, response, false);
        });
        server.on("/api/update/website/manual", HTTP_POST, websiteUploadHandler);

        server.on("/api/update/firmware",
                  HTTP_POST,
                  [](PsychicRequest* request, PsychicResponse* response, JsonVariant& json) {
                      return handleLatestUpdate(request, response, json, true);
                  });

        server.on("/api/update/website",
                  HTTP_POST,
                  [](PsychicRequest* request, PsychicResponse* response, JsonVariant& json) {
                      return handleLatestUpdate(request, response, json, false);
                  });

        server.on("/api/version", HTTP_GET, handleGetVersion);

        Log.println("Update API registered");
    }

    esp_err_t handleGetStatus(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        UpdateHandler::getUpdateProgress().toJson(doc.to<JsonObject>());
        return Response::sendJson(response, doc);
    }

    esp_err_t handleGetVersion(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        doc["firmware"] = Version::FIRMWARE;
        doc["website"] = WebServer::WEBSITE_VERSION;

        return Response::sendJson(response, doc);
    }

    esp_err_t handleLatestUpdate(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json,
                                 bool isFirmware) {
        UpdateHandler::UpdateRequest updateRequest;
        updateRequest.fromJson(json);

        Log.println("Received update request: " + updateRequest.toString());

        const String latestVersion = updateRequest.version;

        const String currentVersion =
            isFirmware ? String("v") + Version::FIRMWARE : String("v") + WebServer::WEBSITE_VERSION;

        switch (UpdateHandler::compareVersions(currentVersion, latestVersion)) {
            case 1:
                return Response::error(
                    response, "Current version is newer than requested version", 409);

            case 0:
                return Response::error(response, "Already up to date", 409);

            case -1:
                break;  // Proceed with update
        }

        if (UpdateHandler::requestUpdate(updateRequest, isFirmware)) {
            return Response::success(response, "Update started successfully");
        } else {
            return Response::error(response, "Failed to start update", 500);
        }
    }

}  // namespace UpdateApi