#pragma once

#include "NetworkingTypes.h"
#include <Common/UpdateStatus.h>
#include <WiFiType.h>
#include <vector>
#include <Config/ConfigTypes.h>

namespace NetworkManager {
    struct NetworkStatus {
        bool connected;
        String ssid;
        String ipAddress;
        uint8_t wifiStrength;
    };

    extern std::vector<WiFiNetwork> cachedWifiNetworks;
    extern UpdateStatus scanStatus;

    extern volatile UpdateStatus updateStatus;
    extern NetworkConfig pendingConfig;

    bool connect(const String& ssid, const String& password, WiFiMode_t mode);
    void loop();
    void startScanning();
    void pollNetworkScan();
    void requestUpdate(NetworkConfig& newConfig);
    uint8_t rssiToWifiStrength(int8_t rssi);
    const NetworkStatus& getNetworkStatus();
}  // namespace NetworkManager