#include "ConfigApi.h"
#include <Common/Network.h>
#include <Config/ConfigManager.h>
#include <Common/Logger.h>
#include <Config/ConfigManager.h>
#include <System/SystemManager.h>

namespace ConfigApi {
    File uploadFile;
    bool uploadFailed = false;

    void registerRoutes(PsychicHttpServer& server) {
        server.on("/api/config", HTTP_GET, handleGetConfig);

        auto* uploadHandler = new PsychicUploadHandler();
        uploadHandler->onUpload(onUpload);
        uploadHandler->onRequest(onUploadFinish);
        server.on("/api/config", HTTP_POST, uploadHandler);

        Log.println("Config API registered");
    }

    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        File configFile = ConfigManager::ConfigFS.open(ConfigManager::CONFIG_LOCATION, "r");

        DeserializationError error = deserializeJson(doc, configFile);
        configFile.close();

        if (error) {
            Log.printf("Failed to deserialize config: %s\n", error.c_str());
            return Response::error(response, 500, "Failed to deserialize config");
        }

        return Response::sendJson(response, doc);
    }

    esp_err_t onUpload(PsychicRequest* request,
                       const String& filename,
                       uint64_t index,
                       uint8_t* data,
                       size_t len,
                       bool final) {
        if (index == 0) {
            Log.printf("Uploading %s\n", filename.c_str());
            uploadFailed = false;

            ConfigManager::ConfigFS.remove("/config.tmp");
            uploadFile = ConfigManager::ConfigFS.open("/config.tmp", "w");

            if (!uploadFile) {
                Log.println("Failed to open config.tmp");
                uploadFailed = true;
                return ESP_FAIL;
            }
        }

        if (!uploadFailed) {
            size_t written = uploadFile.write(data, len);

            Log.printf("Chunk index=%u len=%u written=%u final=%d\n",
                       (unsigned)index,
                       (unsigned)len,
                       (unsigned)written,
                       final);

            if (written != len) {
                Log.println("Write failed");
                uploadFailed = true;
                return ESP_FAIL;
            }
        }

        if (final && uploadFile) {
            uploadFile.flush();  // optional but good practice
            uploadFile.close();

            Log.printf("Final upload size: %u\n", (unsigned)(index + len));
        }
        return ESP_OK;
    }

    esp_err_t onUploadFinish(PsychicRequest* request, PsychicResponse* response) {
        if (uploadFailed) {
            ConfigManager::ConfigFS.remove("/config.tmp");
            return Response::error(response, 500, "Upload failed");
        }

        if (!validateConfig()) {
            ConfigManager::ConfigFS.remove("/config.tmp");
            return Response::error(response, 400, "Invalid config");
        }

        ConfigManager::ConfigFS.remove("/config.json");
        ConfigManager::ConfigFS.rename("/config.tmp", "/config.json");

        SystemManager::requestRestart();

        return Response::success(response, 200, "Config updated");
    }

    bool validateConfig() {
        File file = ConfigManager::ConfigFS.open("/config.tmp", "r");

        if (!file)
            return false;

        JsonDocument doc;

        DeserializationError err = deserializeJson(doc, file);

        file.close();

        return !err;
    }

}  // namespace ConfigApi
