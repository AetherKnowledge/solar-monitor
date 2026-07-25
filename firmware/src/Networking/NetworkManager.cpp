#include <Networking/NetworkManager.h>
#include <WiFi.h>
#include <Config/ConfigManager.h>
#include <Mqtt/MqttManager.h>
#include <Common/Logger.h>

namespace NetworkManager {
    std::vector<WiFiNetwork> cachedWifiNetworks;
    UpdateStatus scanStatus = UpdateStatus::NotStarted;

    volatile UpdateStatus updateStatus = UpdateStatus::NotStarted;
    NetworkConfig pendingConfig;

    NetworkStatus status;

    static void updateConfig() {
        updateStatus = UpdateStatus::InProgress;

        bool isValidConnection = connect(pendingConfig);

        if (!isValidConnection) {
            Log.println();
            Log.println("Failed to connect to the new network. Keeping the old configuration.");

            connect(ConfigManager::config.network);

            updateStatus = UpdateStatus::UpdateFailed;
            return;
        }

        ConfigManager::config.network = pendingConfig;
        ConfigManager::save();
        MqttManager::reload();

        Log.println();
        Log.println("Network configuration updated");
        Log.println(ConfigManager::config.network.toString().c_str());

        updateStatus = UpdateStatus::UpdateComplete;
    }

    void startScanning() {
        Log.println("Starting WiFi scan");
        if (scanStatus != UpdateStatus::InProgress) {
            scanStatus = UpdateStatus::InProgress;
            WiFi.scanNetworks(true);
        }
    }

    void pollNetworkScan() {
        int n = WiFi.scanComplete();

        if (n == WIFI_SCAN_FAILED) {
            Log.println("WiFi scan failed");
            scanStatus = UpdateStatus::UpdateFailed;
            return;
        }

        if (n >= 0) {
            Log.println("WiFi scan complete");
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

    static void pollNetworkStatus() {
        static uint32_t lastPoll = 0;

        if (millis() - lastPoll < 5000)
            return;

        lastPoll = millis();

        status.connected = WiFi.status() == WL_CONNECTED;
        status.ssid = WiFi.SSID();
        status.ipAddress = WiFi.localIP().toString();
        status.wifiStrength = rssiToWifiStrength(WiFi.RSSI());
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

        pollNetworkStatus();
    }

    bool connect(NetworkConfig& config) {
        Log.printf("Connecting to WiFi network: %s\n", config.wifiSsid.c_str());
        WiFi.mode(config.mode);

        if (config.mode == WIFI_AP_STA && !config.apSsid.isEmpty()) {
            Log.printf("Starting Access Point with SSID: %s\n", config.apSsid.c_str());
            if (config.apPasswordEnabled) {
                WiFi.softAP(config.apSsid.c_str(), config.apPassword.c_str());
            } else {
                WiFi.softAP(config.apSsid.c_str());
            }
        }

        WiFi.begin(config.wifiSsid.c_str(), config.wifiPassword.c_str());
        WiFi.setSleep(false);

        unsigned long startAttemptTime = millis();

        // Keep trying to connect for 10 seconds
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            delay(100);
            Log.print(".");
        }
        Log.println();

        if (WiFi.status() == WL_CONNECTED) {
            status.connected = true;
            status.ssid = config.wifiSsid;
            status.ipAddress = WiFi.localIP().toString();
            status.wifiStrength = rssiToWifiStrength(WiFi.RSSI());

            Log.println("Connected!");

            Log.print("IP Address: ");
            Log.println(WiFi.localIP());
            return true;
        } else {
            status.connected = false;

            Log.println("Failed to connect to WiFi.");
            WiFi.mode(WIFI_AP_STA);  // Reset to default mode
            return false;
        }
    }

    uint8_t rssiToWifiStrength(int8_t rssi) {
        if (rssi >= -50) {
            return 5;  // Excellent
        } else if (rssi >= -60) {
            return 4;  // Good
        } else if (rssi >= -70) {
            return 3;  // Fair
        } else if (rssi >= -80) {
            return 2;  // Weak
        } else {
            return 1;  // Very weak
        }
    }

    const NetworkStatus& getNetworkStatus() {
        return status;
    }

    void requestUpdate(NetworkConfig& newConfig) {
        pendingConfig = newConfig;
        ConfigManager::save();
        updateStatus = UpdateStatus::Requested;

        Log.println("\nNetwork configuration updating");
    }

}  // namespace NetworkManager