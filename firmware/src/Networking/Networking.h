#pragma once

#include <vector>
#include "NetworkingTypes.h"

extern std::vector<WiFiNetwork> cachedWifiNetworks;
extern ScanStatus wifiScanStatus;
extern volatile bool wifiScanRequested;

void startScanning();
void scanNetworksAsync();
const char *scanStatusToString(ScanStatus status);