#pragma once

#include <Config/ConfigManager.h>
#include <ESPAsyncWebServer.h>

void registerMqttApi(AsyncWebServer& server);
void handleGetMqttConfig(AsyncWebServerRequest* request);
void handleUpdateMqttConfig(AsyncWebServerRequest* request, JsonVariant& json);