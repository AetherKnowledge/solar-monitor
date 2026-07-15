#pragma once

#include <ESPAsyncWebServer.h>

void registerNetworkApi(AsyncWebServer &server);
void handleGetWifiNetworks(AsyncWebServerRequest *request);
void handleScanWifiNetworks(AsyncWebServerRequest *request);