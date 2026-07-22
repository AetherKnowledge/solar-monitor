#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <optional>
#include <unordered_map>

namespace Response {
    inline void send(AsyncWebServerRequest* request,
                     int status,
                     const String& message,
                     Print* out = nullptr) {
        JsonDocument doc;
        doc["message"] = message;

        String json;
        serializeJson(doc, json);

        if (out) {
            out->printf("Response: %d %s\n", status, message.c_str());
        }

        request->send(status, "application/json", json);
    }

    inline void success(AsyncWebServerRequest* request,
                        int status = 200,
                        const String& message = "OK",
                        Print* out = nullptr) {
        send(request, status, message, out);
    }

    inline void error(AsyncWebServerRequest* request,
                      int status,
                      const String& message,
                      Print* out = nullptr) {
        send(request, status, message, out);
    }
}  // namespace Response