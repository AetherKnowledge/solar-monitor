#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void registerModbusApi(AsyncWebServer &server);
void handleGetModbusConfig(AsyncWebServerRequest *request);