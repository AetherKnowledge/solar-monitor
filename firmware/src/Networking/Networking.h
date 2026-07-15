#pragma once

#include <vector>
#include "NetworkingTypes.h"
#include <WiFiType.h>
#include <Common/CommonTypes.h>

extern std::vector<WiFiNetwork> cachedWifiNetworks;
extern Status wifiScanStatus;
extern volatile bool wifiScanRequested;

bool connectToWiFi(const String &ssid, const String &password, WiFiMode_t mode);
void startScanning();
void scanNetworksAsync();