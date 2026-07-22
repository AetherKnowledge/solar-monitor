#pragma once

#include <Arduino.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include <optional>
namespace UpdateHandler {

    void onUpdateFinish(AsyncWebServerRequest* request, bool isFirmware);
    void onChunk(uint8_t* data,
                 size_t len,
                 size_t index,
                 bool final,
                 std::optional<size_t> total,
                 bool isFirmware);
}  // namespace UpdateHandler