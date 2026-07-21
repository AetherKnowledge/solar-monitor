#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <Common/Json.h>

struct DeviceDiscovery {
    String identifier;
    String manufacturer;
    String model;
    String name;

    void toJson(JsonObject json) const {
        json["identifier"] = identifier;
        json["manufacturer"] = manufacturer;

        if (!model.isEmpty())
            json["model"] = model;

        if (!name.isEmpty())
            json["name"] = name;
    }

    void fromJson(JsonObject json) {
        identifier = json["identifier"] | "";
        manufacturer = json["manufacturer"] | "";
        model = json["model"] | "";
        name = json["name"] | "";
    }
};

struct SensorDiscovery {
    String name;
    String uniqueId;

    String stateTopic;
    String deviceClass;
    String stateClass;
    String unitOfMeasurement;
    String icon;

    static constexpr const char* Component = "sensor";

    void toJson(JsonObject json) const {
        json["name"] = name;
        json["unique_id"] = uniqueId;

        if (!stateClass.isEmpty())
            json["state_class"] = stateClass;

        if (!stateTopic.isEmpty())
            json["state_topic"] = stateTopic;

        if (!unitOfMeasurement.isEmpty())
            json["unit_of_measurement"] = unitOfMeasurement;

        if (!deviceClass.isEmpty())
            json["device_class"] = deviceClass;

        if (!icon.isEmpty())
            json["icon"] = icon;
    }

    void fromJson(JsonObject json) {
        uniqueId = json["unique_id"] | "";
        name = json["name"] | uniqueId;

        deviceClass = json["device_class"] | "";
        stateClass = json["state_class"] | "";
        unitOfMeasurement = json["unit_of_measurement"] | "";
        icon = json["icon"] | "";
    }
};

struct WriteDiscovery : SensorDiscovery {
    String commandTemplate;
    String commandTopic;
    String valueTemplate;
    uint8_t qos = 0;

    void toJson(JsonObject json) const {
        SensorDiscovery::toJson(json);

        if (!valueTemplate.isEmpty())
            json["value_template"] = valueTemplate;

        if (!commandTemplate.isEmpty())
            json["command_template"] = commandTemplate;

        json["command_topic"] = commandTopic;
        json["qos"] = qos;
    }

    void fromJson(JsonObject json) {
        SensorDiscovery::fromJson(json);

        valueTemplate = json["value_template"] | "";
        commandTemplate = json["command_template"] | "";
        qos = json["qos"] | 0;
    }
};

struct SelectDiscovery : WriteDiscovery {
    std::vector<String> options;

    static constexpr const char* Component = "select";

    void toJson(JsonObject json) const {
        WriteDiscovery::toJson(json);
        serializeVector(json["options"], options);
    }

    void fromJson(JsonObject json) {
        WriteDiscovery::fromJson(json);
        deserializeVector(json["options"], options);
    }
};

struct NumberDiscovery : WriteDiscovery {
    double min = 0;
    double max = 100;
    double step = 1;
    String mode = "auto";

    static constexpr const char* Component = "number";

    void toJson(JsonObject json) const {
        WriteDiscovery::toJson(json);

        json["min"] = min;
        json["max"] = max;
        json["step"] = step;
        json["mode"] = mode;
    }

    void fromJson(JsonObject json) {
        WriteDiscovery::fromJson(json);

        min = json["min"] | 0;
        max = json["max"] | 100;
        step = json["step"] | 1;
        mode = json["mode"] | "auto";
    }
};