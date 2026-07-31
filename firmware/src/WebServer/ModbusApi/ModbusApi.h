#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace ModbusApi {
    void registerApi(AsyncWebServer& server);
    void handleGetDevices(AsyncWebServerRequest* request);
    void handleGetDevice(AsyncWebServerRequest* request, String id);
    void handleUpdateDevice(AsyncWebServerRequest* request, JsonVariant& json, String id);
    void handleGetValues(AsyncWebServerRequest* request, String id);
    void handleGetStatus(AsyncWebServerRequest* request);
}  // namespace ModbusApi