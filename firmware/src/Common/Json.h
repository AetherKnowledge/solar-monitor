#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

template <typename T>
concept JsonSerializable = requires(const T& value, JsonObject obj) {
    { value.toJson(obj) } -> std::same_as<void>;
};

template <typename T>
concept JsonDeserializable = requires(T& value, JsonObject obj) {
    { value.fromJson(obj) } -> std::same_as<void>;
};

template <typename T>
concept JsonPrimitive =
    std::integral<std::remove_cvref_t<T>> || std::floating_point<std::remove_cvref_t<T>> ||
    std::is_enum_v<std::remove_cvref_t<T>> || std::same_as<std::remove_cvref_t<T>, String> ||
    std::convertible_to<T, const char*>;

template <typename T>
concept JsonArrayContainer = requires(std::remove_reference_t<T>& value) {
    { value.template to<JsonArray>() } -> std::same_as<JsonArray>;
    { value.template as<JsonArray>() } -> std::same_as<JsonArray>;
};

// Serialize
template <typename T>
    requires(JsonPrimitive<T> || JsonSerializable<T>)
inline void serializeVector(JsonArray parent, const std::vector<T>& vec) {
    for (const auto& item : vec) {
        if constexpr (JsonPrimitive<T>) {
            parent.add(item);
        } else {
            item.toJson(parent.add<JsonObject>());
        }
    }
}

template <JsonArrayContainer V, typename T>
    requires(JsonPrimitive<T> || JsonSerializable<T>)
inline void serializeVector(V&& parent, const std::vector<T>& vec) {
    JsonArray array = parent.template to<JsonArray>();
    serializeVector(array, vec);
}

// Deserialize
template <typename T>
    requires(JsonPrimitive<T> || JsonDeserializable<T>)
inline void deserializeVector(JsonArray parent, std::vector<T>& vec) {
    vec.clear();

    for (JsonVariant value : parent) {
        if constexpr (JsonPrimitive<T>) {
            vec.emplace_back(value.as<T>());
        } else {
            auto& item = vec.emplace_back();
            item.fromJson(value.as<JsonObject>());
        }
    }
}

template <JsonArrayContainer V, typename T>
    requires(JsonPrimitive<T> || JsonDeserializable<T>)
inline void deserializeVector(V&& parent, std::vector<T>& vec) {
    JsonArray array = parent.template as<JsonArray>();
    deserializeVector(array, vec);
}
