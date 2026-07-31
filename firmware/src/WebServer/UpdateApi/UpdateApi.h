#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace UpdateApi {
    void registerApi(AsyncWebServer& server);
    void handleGetVersion(AsyncWebServerRequest* request);
    void handleLatestUpdate(AsyncWebServerRequest* request,
                            uint8_t* data,
                            size_t len,
                            size_t index,
                            size_t total,
                            bool isFirmware);
    void handleGetStatus(AsyncWebServerRequest* request);

}  // namespace UpdateApi