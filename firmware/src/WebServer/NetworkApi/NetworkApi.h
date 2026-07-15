#pragma once

#include <ESPAsyncWebServer.h>
#include <Config/Config.h>

void registerNetworkApi(AsyncWebServer &server);
void handleGetNetworkConfig(AsyncWebServerRequest *request);
void handleUpdateNetworkConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len);
void handleGetWifiNetworks(AsyncWebServerRequest *request);
void handleScanWifiNetworks(AsyncWebServerRequest *request);