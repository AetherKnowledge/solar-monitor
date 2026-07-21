#pragma once

#include "NetworkingTypes.h"
#include <Common/UpdateStatus.h>
#include <WiFiType.h>
#include <vector>
#include <Config/ConfigTypes.h>

namespace NetworkManager {
    extern std::vector<WiFiNetwork> cachedWifiNetworks;
    extern UpdateStatus scanStatus;

    extern volatile UpdateStatus updateStatus;
    extern NetworkConfig pendingConfig;

    bool connect(const String& ssid, const String& password, WiFiMode_t mode);
    void loop();
    void startScanning();
    void pollNetworkScan();
    void updateConfig();
}  // namespace NetworkManager