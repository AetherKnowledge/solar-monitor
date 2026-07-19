#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>

template <typename T>
inline void serializeVector(JsonObject parent, const char* key, const std::vector<T>& vec) {
    JsonArray array = parent[key].to<JsonArray>();

    for (const auto& item : vec) {
        item.toJson(array.add<JsonObject>());
    }
}

template <typename T>
inline void deserializeVector(JsonObject parent, const char* key, std::vector<T>& vec) {
    vec.clear();

    for (JsonObject obj : parent[key].as<JsonArray>()) {
        auto& item = vec.emplace_back();
        item.fromJson(obj);
    }
}