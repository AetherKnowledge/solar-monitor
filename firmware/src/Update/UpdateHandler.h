#pragma once

#include <Arduino.h>
#include <Update.h>
#include <PsychicHttp.h>
#include <Common/UpdateStatus.h>
#include <optional>
#include "SignatureHandler.h"
#include <Common/Enum.h>

namespace UpdateHandler {
    struct UpdateProgress {
        UpdateStatus status;
        uint8_t progress;
        bool hasTotalSize;

        void toJson(JsonObject json) const {
            json["status"] = Enum::toString(status);
            json["progress"] = progress;
            json["hasTotalSize"] = hasTotalSize;
        }
    };

    struct UpdateRequest {
        String url;
        size_t size;
        String file;
        String sha256;
        String version;
        String signature;
        bool isFirmware = false;

        void fromJson(JsonVariant json) {
            url = json["url"] | "";
            size = json["size"] | 0;
            file = json["file"] | "";
            sha256 = json["sha256"] | "";
            version = json["version"] | "0.0.0";
            signature = json["signature"] | "";
        }

        String getPayload() const {
            return version + "\n" + file + "\n" + url + "\n" + String(size) + "\n" + sha256;
        }

        bool validatePayload() const {
            return verifySignature(getPayload(), signature);
        }

        String toString() const {
            return String("UpdateRequest { url: ") + url + ", size: " + size +
                   ", sha256: " + sha256 + ", version: " + version + ", isFirmware: " + isFirmware +
                   " }";
        }
    };

    void setup();

    esp_err_t onUpdateFinish(PsychicRequest* request, PsychicResponse* response, bool isFirmware);
    esp_err_t onChunk(PsychicRequest* request,
                      uint8_t* data,
                      size_t len,
                      uint64_t index,
                      bool final,
                      bool isFirmware);
    ;

    int compareVersions(String a, String b);

    const UpdateProgress& getUpdateProgress();
    bool downloadAndInstall(const UpdateRequest& request, bool isFirmware);

    bool requestUpdate(const UpdateRequest& request, bool isFirmware);

}  // namespace UpdateHandler