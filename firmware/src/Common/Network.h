#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <optional>
#include <unordered_map>

namespace Response {
    inline void send(AsyncWebServerRequest* request, int status, const String& message) {
        JsonDocument doc;
        doc["message"] = message;

        String json;
        serializeJson(doc, json);

        request->send(status, "application/json", json);
    }

    inline void success(AsyncWebServerRequest* request,
                        int status = 200,
                        const String& message = "OK") {
        send(request, status, message);
    }

    inline void error(AsyncWebServerRequest* request, int status, const String& message) {
        send(request, status, message);
    }
}  // namespace Response