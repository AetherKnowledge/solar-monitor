#pragma once

#include <ESPAsyncWebServer.h>
#include <Config/Config.h>

void registerMqttApi(AsyncWebServer &server);
void handleGetMqttConfig(AsyncWebServerRequest *request);
void handleUpdateMqttConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len);