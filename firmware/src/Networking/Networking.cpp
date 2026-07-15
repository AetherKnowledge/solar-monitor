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
                network.connected = WiFi.SSID() == network.ssid;

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

bool connectToWiFi(const String &ssid, const String &password, WiFiMode_t mode)
{
    Serial.printf("Connecting to WiFi network: %s\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    WiFi.mode(mode);
    WiFi.setSleep(false);

    unsigned long startAttemptTime = millis();

    // Keep trying to connect for 10 seconds
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
    {
        delay(100);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Connected!");

        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    else
    {
        Serial.println("Failed to connect to WiFi.");
        WiFi.mode(WIFI_AP_STA); // Reset to default mode
        return false;
    }
}