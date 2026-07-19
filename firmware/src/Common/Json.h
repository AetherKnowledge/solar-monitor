#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

template <typename T>
inline void serializeVector(JsonObject parent, const char* key, const std::vector<T>& vec) {
    JsonArray array = parent[key].to<JsonArray>();

    for (const auto& item : vec) {
        if constexpr (std::is_same_v<T, String>) {
            array.add(item);
        } else if constexpr (std::is_same_v<T, const char*>) {
            array.add(item);
        } else {
            item.toJson(array.add<JsonObject>());
        }
    }
}

template <typename T>
inline void deserializeVector(JsonObject parent, const char* key, std::vector<T>& vec) {
    vec.clear();

    for (JsonVariant value : parent[key].as<JsonArray>()) {
        if constexpr (std::is_same_v<T, String>) {
            vec.emplace_back(value.as<String>());
        } else if constexpr (std::is_same_v<T, const char*>) {
            vec.emplace_back(value.as<const char*>());
        } else {
            auto& item = vec.emplace_back();
            item.fromJson(value.as<JsonObject>());
        }
    }
}