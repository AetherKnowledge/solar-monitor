#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <optional>
#include <unordered_map>

namespace Response {
    inline esp_err_t send(PsychicResponse* response,
                          const JsonVariantConst& value,
                          int status = 200,
                          Print* out = nullptr) {
        PsychicStreamResponse stream(response, "application/json");
        stream.setCode(status);

        const esp_err_t err = stream.beginSend();
        if (err != ESP_OK) {
            return err;
        }

        serializeJson(value, stream);
        const esp_err_t result = stream.endSend();

        if (out) {
            out->printf("Response: %d ", status);
            serializeJson(value, *out);
            out->println();
        }

        return result;
    }

    inline esp_err_t send(PsychicResponse* response,
                          const String& message,
                          int status,
                          Print* out = nullptr) {
        JsonDocument doc;
        doc["message"] = message;

        return send(response, doc, status, out);
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

}  // namespace Response