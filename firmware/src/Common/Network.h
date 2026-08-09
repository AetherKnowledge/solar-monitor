#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <optional>
#include <unordered_map>

namespace Response {
    inline esp_err_t send(PsychicResponse* response,
                          const String& message,
                          int status,
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
                             const String& message = "OK",
                             int status = 200,
                             Print* out = nullptr) {
        return send(response, message, status, out);
    }

    inline esp_err_t error(PsychicResponse* response,
                           const String& message,
                           int status = 400,
                           Print* out = nullptr) {
        return send(response, message, status, out);
    }

    inline esp_err_t sendJson(PsychicResponse* response,
                              const JsonVariantConst& value,
                              int status = 200) {
        String json;
        serializeJson(value, json);

        return response->send(status, "application/json", json.c_str());
    }

}  // namespace Response