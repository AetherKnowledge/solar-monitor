#include <Networking/Networking.h>
#include <WiFi.h>

std::vector<WiFiNetwork> cachedWifiNetworks;
UpdateStatus wifiScanStatus = UpdateStatus::NotStarted;

volatile bool wifiScanRequested = false;

void startScanning()
{
    Serial.println("Starting WiFi scan");

    if (wifiScanStatus != UpdateStatus::InProgress)
    {
        wifiScanStatus = UpdateStatus::InProgress;
        int r = WiFi.scanNetworks(true);
    }
}

void scanNetworks()
{
    int n = WiFi.scanComplete();

    if (n == WIFI_SCAN_FAILED)
    {
        Serial.println("WiFi scan failed");
        wifiScanStatus = UpdateStatus::UpdateFailed;
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
        wifiScanStatus = UpdateStatus::UpdateComplete;
    }
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