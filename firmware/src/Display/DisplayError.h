#pragma once

enum class DisplayError { LittleFS, Wifi, MQTT, Config, Modbus, Unknown };
static const char* getErrorMessage(DisplayError error) {
    switch (error) {
        case DisplayError::LittleFS:
            return "LittleFS Error";
        case DisplayError::Wifi:
            return "WiFi Error";
        case DisplayError::MQTT:
            return "MQTT Error";
        case DisplayError::Config:
            return "Config Error";
        case DisplayError::Modbus:
            return "Modbus Error";
        default:
            return "Unknown Error";
    }
}