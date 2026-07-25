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

struct Discovery {
    virtual ~Discovery() = default;

    String name;
    String uniqueId;
    String stateTopic;
    String icon;

    virtual const char* component() const = 0;

    virtual void toJson(JsonObject) const = 0;
    virtual void fromJson(JsonObject) = 0;
};

struct SensorDiscovery : Discovery {
    String deviceClass;
    String stateClass;
    String unitOfMeasurement;

    const char* component() const override {
        return "sensor";
    }

    void toJson(JsonObject json) const override {
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

    void fromJson(JsonObject json) override {
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

    virtual const char* component() const override = 0;

    void toJson(JsonObject json) const override {
        SensorDiscovery::toJson(json);

        if (!valueTemplate.isEmpty())
            json["value_template"] = valueTemplate;

        if (!commandTemplate.isEmpty())
            json["command_template"] = commandTemplate;

        json["command_topic"] = commandTopic;
        json["qos"] = qos;
    }

    void fromJson(JsonObject json) override {
        SensorDiscovery::fromJson(json);

        valueTemplate = json["value_template"] | "";
        commandTemplate = json["command_template"] | "";
        qos = json["qos"] | 0;
    }
};

struct SelectDiscovery : WriteDiscovery {
    std::vector<String> options;

    const char* component() const override {
        return "select";
    }

    void toJson(JsonObject json) const override {
        WriteDiscovery::toJson(json);
        serializeVector(json["options"], options);
    }

    void fromJson(JsonObject json) override {
        WriteDiscovery::fromJson(json);
        deserializeVector(json["options"], options);
    }
};

struct NumberDiscovery : WriteDiscovery {
    double min = 0;
    double max = 100;
    double step = 1;
    String mode = "auto";

    const char* component() const override {
        return "number";
    }

    void toJson(JsonObject json) const override {
        WriteDiscovery::toJson(json);

        json["min"] = min;
        json["max"] = max;
        json["step"] = step;
        json["mode"] = mode;
    }

    void fromJson(JsonObject json) override {
        WriteDiscovery::fromJson(json);

        min = json["min"] | 0;
        max = json["max"] | 100;
        step = json["step"] | 1;
        mode = json["mode"] | "auto";
    }
};