#pragma once

#include <Common/Enum.h>
#include <WiFiType.h>

template <>
inline const char* Enum::toString(WiFiMode_t value) {
    switch (value) {
        case WIFI_AP:
            return "wifi_ap";
        case WIFI_STA:
            return "wifi_sta";
        case WIFI_OFF:
            return "wifi_off";
        case WIFI_AP_STA:
            return "ap+sta";
    }

    return "ap+sta";
}

template <>
inline WiFiMode_t Enum::fromString<WiFiMode_t>(const char* value) {
    if (strcmp(value, "wifi_ap") == 0)
        return WIFI_AP;
    if (strcmp(value, "wifi_sta") == 0)
        return WIFI_STA;
    if (strcmp(value, "wifi_off") == 0)
        return WIFI_OFF;
    if (strcmp(value, "ap+sta") == 0)
        return WIFI_AP_STA;

    return WIFI_AP_STA;
}