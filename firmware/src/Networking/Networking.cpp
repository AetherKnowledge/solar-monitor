#include <Networking/Networking.h>
#include <WiFi.h>
#include <Config/Config.h>

std::vector<WiFiNetwork> cachedWifiNetworks;
ScanStatus wifiScanStatus = SCAN_STATUS_NOT_STARTED;

volatile bool wifiScanRequested = false;

void startScanning()
{
    Serial.println("Starting WiFi scan");

    if (wifiScanStatus != SCAN_STATUS_IN_PROGRESS)
    {
        wifiScanStatus = SCAN_STATUS_IN_PROGRESS;
        int r = WiFi.scanNetworks(true);
    }
}

void scanNetworksAsync()
{
    if (wifiScanStatus == SCAN_STATUS_IN_PROGRESS)
    {
        int n = WiFi.scanComplete();

        if (n == WIFI_SCAN_FAILED)
        {
            Serial.println("WiFi scan failed");
            wifiScanStatus = SCAN_STATUS_FAILED;
            return;
        }

        if (n >= 0)
        {
            Serial.println("WiFi scan complete");
            cachedWifiNetworks.clear();
            for (int i = 0; i < n; ++i)
            {
                WiFiNetwork network;
                network.ssid = WiFi.SSID(i);
                network.rssi = WiFi.RSSI(i);
                network.encryptionType = WiFi.encryptionType(i);
                network.saved = false;
                network.connected = config.network.ssid == network.ssid;

                cachedWifiNetworks.push_back(network);
            }
            WiFi.scanDelete();
            wifiScanStatus = SCAN_STATUS_COMPLETE;
        }
    }
}

const char *scanStatusToString(ScanStatus status)
{
    switch (status)
    {
    case SCAN_STATUS_NOT_STARTED:
        return "not_started";
    case SCAN_STATUS_IN_PROGRESS:
        return "in_progress";
    case SCAN_STATUS_COMPLETE:
        return "complete";
    case SCAN_STATUS_FAILED:
        return "failed";
    }

    return "unknown";
}