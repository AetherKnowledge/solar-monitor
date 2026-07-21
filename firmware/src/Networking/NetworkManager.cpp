#include <Networking/NetworkManager.h>
#include <WiFi.h>
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>

namespace NetworkManager {
    std::vector<WiFiNetwork> cachedWifiNetworks;
    UpdateStatus scanStatus = UpdateStatus::NotStarted;

    volatile UpdateStatus updateStatus = UpdateStatus::NotStarted;
    NetworkConfig pendingConfig;

    void startScanning() {
        Serial.println("Starting WiFi scan");
        if (scanStatus != UpdateStatus::InProgress) {
            scanStatus = UpdateStatus::InProgress;
            WiFi.scanNetworks(true);
        }
    }

    void pollNetworkScan() {
        int n = WiFi.scanComplete();

        if (n == WIFI_SCAN_FAILED) {
            Serial.println("WiFi scan failed");
            scanStatus = UpdateStatus::UpdateFailed;
            return;
        }

        if (n >= 0) {
            Serial.println("WiFi scan complete");
            cachedWifiNetworks.clear();
            for (int i = 0; i < n; ++i) {
                WiFiNetwork network;
                network.ssid = WiFi.SSID(i);
                network.rssi = WiFi.RSSI(i);
                network.encryptionType = WiFi.encryptionType(i);
                network.saved = false;
                network.connected = WiFi.SSID() == network.ssid;

                cachedWifiNetworks.push_back(network);
            }
            WiFi.scanDelete();
            scanStatus = UpdateStatus::UpdateComplete;
        }
    }

    void loop() {
        if (scanStatus == UpdateStatus::Requested) {
            startScanning();
        }

        if (scanStatus == UpdateStatus::InProgress) {
            pollNetworkScan();
        }

        if (updateStatus == UpdateStatus::Requested) {
            updateConfig();
        }
    }

    bool connect(const String& ssid, const String& password, WiFiMode_t mode) {
        Serial.printf("Connecting to WiFi network: %s\n", ssid.c_str());
        WiFi.begin(ssid.c_str(), password.c_str());
        WiFi.mode(mode);
        WiFi.setSleep(false);

        unsigned long startAttemptTime = millis();

        // Keep trying to connect for 10 seconds
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            delay(100);
            Serial.print(".");
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected!");

            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            return true;
        } else {
            Serial.println("Failed to connect to WiFi.");
            WiFi.mode(WIFI_AP_STA);  // Reset to default mode
            return false;
        }
    }

    void updateConfig() {
        updateStatus = UpdateStatus::InProgress;

        bool isValidConnection =
            connect(pendingConfig.ssid, pendingConfig.password, pendingConfig.mode);

        if (!isValidConnection) {
            Serial.println();
            Serial.println("Failed to connect to the new network. Keeping the old configuration.");

            connect(ConfigManager::config.network.ssid,
                    ConfigManager::config.network.password,
                    ConfigManager::config.network.mode);

            updateStatus = UpdateStatus::UpdateFailed;
            return;
        }

        ConfigManager::config.network = pendingConfig;
        ConfigManager::save();
        MqttManager::reload();

        Serial.println();
        Serial.println("Network configuration updated");
        Serial.println(ConfigManager::config.network.toString().c_str());

        updateStatus = UpdateStatus::UpdateComplete;
    }

}  // namespace NetworkManager