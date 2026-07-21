#pragma once

#include <ESPAsyncWebServer.h>

namespace NetworkApi {
    void registerApi(AsyncWebServer& server);
    void handleGetNetworks(AsyncWebServerRequest* request);
    void handleScanNetworks(AsyncWebServerRequest* request);
    void handleGetConfig(AsyncWebServerRequest* request);
    void handleUpdateConfig(AsyncWebServerRequest* request, JsonVariant& json);
}  // namespace NetworkApi