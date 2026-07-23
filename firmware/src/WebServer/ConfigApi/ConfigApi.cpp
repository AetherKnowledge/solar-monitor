#include "ConfigApi.h"
#include <Common/Network.h>
#include <Config/ConfigManager.h>
#include <Common/Logger.h>
#include <Config/ConfigManager.h>
#include <System/SystemManager.h>

namespace ConfigApi {
    File uploadFile;
    bool uploadFailed = false;

    void registerApi(AsyncWebServer& server) {
        server.on(
            "/api/config",
            HTTP_POST,

            // Called when upload finishes
            [](AsyncWebServerRequest* request) { onUploadFinish(request); },

            // Called for every uploaded chunk
            [](AsyncWebServerRequest* request,
               String filename,
               size_t index,
               uint8_t* data,
               size_t len,
               bool final) { onUpload(request, filename, index, data, len, final); });
        server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetConfig(request);
        });

        Log.println("Config API registered");
    }

    void handleGetConfig(AsyncWebServerRequest* request) {
        JsonDocument doc;
        File configFile = ConfigManager::ConfigFS.open(ConfigManager::CONFIG_LOCATION, "r");

        DeserializationError error = deserializeJson(doc, configFile);
        configFile.close();

        if (error) {
            Log.printf("Failed to deserialize config: %s\n", error.c_str());
            return;
        }

        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJsonPretty(doc, *response);
        request->send(response);
    }

    void onUpload(AsyncWebServerRequest* request,
                  String filename,
                  size_t index,
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
                return;
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
            }
        }

        if (final && uploadFile) {
            uploadFile.flush();  // optional but good practice
            uploadFile.close();

            Log.printf("Final upload size: %u\n", (unsigned)(index + len));
        }
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

    void onUploadFinish(AsyncWebServerRequest* request) {
        if (uploadFailed) {
            ConfigManager::ConfigFS.remove("/config.tmp");
            Response::error(request, 500, "Upload failed");
            return;
        }

        if (!validateConfig()) {
            ConfigManager::ConfigFS.remove("/config.tmp");
            Response::error(request, 400, "Invalid config");
            return;
        }

        ConfigManager::ConfigFS.remove("/config.json");
        ConfigManager::ConfigFS.rename("/config.tmp", "/config.json");

        SystemManager::requestRestart();

        Response::success(request, 200, "Config updated");
    }
}  // namespace ConfigApi
