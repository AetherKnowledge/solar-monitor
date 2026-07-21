#pragma once

#include <Config/ConfigManager.h>
#include <ESPAsyncWebServer.h>

namespace MqttApi {
    void registerApi(AsyncWebServer& server);
    void handleGetConfig(AsyncWebServerRequest* request);
    void handleUpdateConfig(AsyncWebServerRequest* request, JsonVariant& json);
}  // namespace MqttApi