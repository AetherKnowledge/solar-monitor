#pragma once

#include "NetworkingTypes.h"
#include "UpdateStatus.h"
#include <WiFiType.h>
#include <vector>

extern std::vector<WiFiNetwork> cachedWifiNetworks;
extern UpdateStatus wifiScanStatus;
extern volatile bool wifiScanRequested;

bool connectToWiFi(const String& ssid, const String& password, WiFiMode_t mode);
void startScanning();
void scanNetworks();