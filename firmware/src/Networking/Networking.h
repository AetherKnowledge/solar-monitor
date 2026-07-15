#pragma once

#include <vector>
#include "NetworkingTypes.h"
#include <WiFiType.h>

extern std::vector<WiFiNetwork> cachedWifiNetworks;
extern ScanStatus wifiScanStatus;
extern volatile bool wifiScanRequested;

bool connectToWiFi(const String &ssid, const String &password, WiFiMode_t mode);
bool changeWiFiNetwork(const String &ssid, const String &password, WiFiMode_t mode);
void startScanning();
void scanNetworksAsync();
const char *scanStatusToString(ScanStatus status);