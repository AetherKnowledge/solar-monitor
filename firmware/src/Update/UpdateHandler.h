#pragma once

#include <Arduino.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
namespace UpdateHandler {
    void onFirmwareUpdateFinish(AsyncWebServerRequest* request);
    void onFirmwareUpdate(
        AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);

    void onWebsiteUpdateFinish(AsyncWebServerRequest* request);
    void onWebsiteUpdate(
        AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
}  // namespace UpdateHandler