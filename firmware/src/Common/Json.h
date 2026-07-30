#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>

// Unwrap std::reference_wrapper
template <typename T>
struct unwrap_ref {
    using type = T;
};

template <typename T>
struct unwrap_ref<std::reference_wrapper<T>> {
    using type = T;
};

template <typename T>
using unwrap_ref_t = typename unwrap_ref<std::remove_cvref_t<T>>::type;

template <typename T>
concept JsonSerializable = requires(const unwrap_ref_t<T>& value, JsonObject obj) {
    { value.toJson(obj) } -> std::same_as<void>;
};

template <typename T>
concept JsonDeserializable = requires(unwrap_ref_t<T>& value, JsonObject obj) {
    { value.fromJson(obj) } -> std::same_as<void>;
};

template <typename T>
concept JsonPrimitive = std::integral<unwrap_ref_t<T>> || std::floating_point<unwrap_ref_t<T>> ||
                        std::is_enum_v<unwrap_ref_t<T>> || std::same_as<unwrap_ref_t<T>, String> ||
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
        } else if constexpr (requires { item.get(); }) {
            item.get().toJson(parent.add<JsonObject>());
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
        } else if constexpr (requires(T t) { t.get(); }) {
            static_assert(!sizeof(T),
                          "deserializeVector() cannot create std::reference_wrapper objects.");
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

template <typename T>
concept JsonValueEntity = requires(const unwrap_ref_t<T>& t) {
    { t.discovery.uniqueId } -> std::convertible_to<String>;
    { t.value } -> std::convertible_to<double>;
};

template <JsonValueEntity T>
static void addValues(JsonObject obj,
                      const std::vector<T>& values,
                      const char* removePrefix = nullptr) {
    for (const auto& item : values) {
        const auto& value = [&]() -> const auto& {
            if constexpr (requires { item.get(); }) {
                return item.get();
            } else {
                return item;
            }
        }();

        const char* key = value.discovery.uniqueId.c_str();

        if (removePrefix) {
            const size_t len = strlen(removePrefix);

            if (strncmp(key, removePrefix, len) == 0) {
                key += len;

                // Skip an optional underscore after the prefix.
                if (*key == '_') {
                    ++key;
                }
            }
        }

        obj[key] = value.value;
    }
}