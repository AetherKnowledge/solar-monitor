#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void registerConfigApi(AsyncWebServer &server);
void handleGetNetworkConfig(AsyncWebServerRequest *request);
void handleUpdateNetworkConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len);
void handleGetMqttConfig(AsyncWebServerRequest *request);
void handleUpdateMqttConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len);