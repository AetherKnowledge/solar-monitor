#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

struct WiFiNetwork
{
    String ssid;
    int32_t rssi;
    uint8_t encryptionType;
    bool saved;
    bool connected;

    String toString() const
    {
        return "SSID: " + ssid + "\nRSSI: " + String(rssi) + "\nEncryption Type: " + String(encryptionType) + "\nSaved: " + String(saved) + "\nConnected: " + String(connected);
    }

    void toJson(JsonObject json) const
    {
        json["ssid"] = ssid;
        json["rssi"] = rssi;
        json["encryptionType"] = encryptionType;
        json["saved"] = saved;
        json["connected"] = connected;
    }
};

enum ScanStatus
{
    SCAN_STATUS_IN_PROGRESS,
    SCAN_STATUS_COMPLETE,
    SCAN_STATUS_NOT_STARTED,
    SCAN_STATUS_FAILED
};