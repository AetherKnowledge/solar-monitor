#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <optional>
#include <unordered_map>

namespace Response {
    inline esp_err_t send(PsychicResponse* response,
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

        return response->send(status, "application/json", json.c_str());
    }

    inline esp_err_t success(PsychicResponse* response,
                             int status = 200,
                             const String& message = "OK",
                             Print* out = nullptr) {
        return send(response, status, message, out);
    }

    inline esp_err_t error(PsychicResponse* response,
                           int status,
                           const String& message,
                           Print* out = nullptr) {
        return send(response, status, message, out);
    }

    inline esp_err_t sendJson(PsychicResponse* response,
                              const JsonVariantConst& value,
                              int status = 200) {
        String json;
        serializeJson(value, json);

        return response->send(status, "application/json", json.c_str());
    }

}  // namespace Response