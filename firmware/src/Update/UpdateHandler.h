#pragma once

#include <Arduino.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include <Common/UpdateStatus.h>
#include <optional>

namespace UpdateHandler {
    struct UpdateProgress {
        UpdateStatus status;
        uint8_t progress;
        bool hasTotalSize;
    };

    struct UpdateRequest {
        String url;
        size_t size;
        String sha256;
        String version;
        bool isFirmware = false;

        void fromJson(JsonDocument json) {
            url = json["url"] | "";
            size = json["size"] | 0;
            sha256 = json["sha256"] | "";
            version = json["version"] | "0.0.0";
        }

        String toString() const {
            return String("UpdateRequest { url: ") + url + ", size: " + size +
                   ", sha256: " + sha256 + ", version: " + version + ", isFirmware: " + isFirmware +
                   " }";
        }
    };

    void setup();

    void onUpdateFinish(AsyncWebServerRequest* request, bool isFirmware);
    void onChunk(uint8_t* data,
                 size_t len,
                 size_t index,
                 bool final,
                 std::optional<size_t> total,
                 bool isFirmware);

    int compareVersions(String a, String b);

    const UpdateProgress& getUpdateProgress();
    bool downloadAndInstall(const UpdateRequest& request, bool isFirmware);

    bool requestUpdate(const UpdateRequest& request, bool isFirmware);

}  // namespace UpdateHandler