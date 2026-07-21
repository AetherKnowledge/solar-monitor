#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace ModbusApi {
    void registerApi(AsyncWebServer& server);
    void handleGetConfig(AsyncWebServerRequest* request);
    void handleUpdateConfig(AsyncWebServerRequest* request, JsonVariant& json);
}  // namespace ModbusApi