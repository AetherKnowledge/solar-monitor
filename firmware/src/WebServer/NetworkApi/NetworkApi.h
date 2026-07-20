#pragma once

#include <ESPAsyncWebServer.h>

void registerNetworkApi(AsyncWebServer& server);
void handleGetWifiNetworks(AsyncWebServerRequest* request);
void handleScanWifiNetworks(AsyncWebServerRequest* request);
void handleGetNetworkConfig(AsyncWebServerRequest* request);
void handleUpdateNetworkConfig(AsyncWebServerRequest* request, JsonVariant& json);