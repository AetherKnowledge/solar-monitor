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

    void onUpdateFinish(AsyncWebServerRequest* request, bool isFirmware);
    void onChunk(uint8_t* data,
                 size_t len,
                 size_t index,
                 bool final,
                 std::optional<size_t> total,
                 bool isFirmware);

    const UpdateProgress& getUpdateProgress();
}  // namespace UpdateHandler