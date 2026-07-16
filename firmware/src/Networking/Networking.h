#pragma once

#include <vector>
#include "NetworkingTypes.h"
#include <Common/EnumUpdateStatus.h>
#include <WiFiType.h>

extern std::vector<WiFiNetwork> cachedWifiNetworks;
extern UpdateStatus wifiScanStatus;
extern volatile bool wifiScanRequested;

bool connectToWiFi(const String &ssid, const String &password, WiFiMode_t mode);
void startScanning();
void scanNetworks();