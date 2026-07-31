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

    void fromJson(JsonObject json, bool update = false) {
        if (update) {
            identifier = json["new_identifier"] | identifier;
        } else {
            identifier = json["identifier"] | identifier;
        }
        manufacturer = json["manufacturer"] | manufacturer;
        model = json["model"] | model;
        name = json["name"] | name;
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
    virtual void fromJson(JsonObject, bool) = 0;
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

    void fromJson(JsonObject json, bool update = false) override {
        if (update) {
            uniqueId = json["new_unique_id"] | uniqueId;
        } else {
            uniqueId = json["unique_id"] | uniqueId;
        }
        name = json["name"] | uniqueId;

        deviceClass = json["device_class"] | deviceClass;
        stateClass = json["state_class"] | stateClass;
        unitOfMeasurement = json["unit_of_measurement"] | unitOfMeasurement;
        icon = json["icon"] | icon;
    }
};

struct ControlDiscovery : SensorDiscovery {
    String commandTemplate;
    String commandTopic;
    uint8_t qos = 0;

    virtual const char* component() const override = 0;

    void toJson(JsonObject json) const override {
        SensorDiscovery::toJson(json);

        if (!commandTemplate.isEmpty())
            json["command_template"] = commandTemplate;

        json["command_topic"] = commandTopic;
        json["qos"] = qos;
    }

    void fromJson(JsonObject json, bool update = false) override {
        SensorDiscovery::fromJson(json, update);

        commandTemplate = json["command_template"] | commandTemplate;
        qos = json["qos"] | qos;
    }
};

struct ValueDiscovery : ControlDiscovery {
    String valueTemplate;

    virtual const char* component() const override = 0;

    void toJson(JsonObject json) const override {
        ControlDiscovery::toJson(json);
        if (!valueTemplate.isEmpty())
            json["value_template"] = valueTemplate;
    }

    void fromJson(JsonObject json, bool update = false) override {
        ControlDiscovery::fromJson(json, update);
        valueTemplate = json["value_template"] | valueTemplate;
    }
};

struct SelectDiscovery : ValueDiscovery {
    std::vector<String> options;

    const char* component() const override {
        return "select";
    }

    void toJson(JsonObject json) const override {
        ValueDiscovery::toJson(json);
        serializeVector(json["options"], options);
    }

    void fromJson(JsonObject json, bool update = false) override {
        ValueDiscovery::fromJson(json, update);
        deserializeVector(json["options"], options);
    }
};

struct NumberDiscovery : ValueDiscovery {
    double min = 0;
    double max = 100;
    double step = 1;
    String mode = "auto";

    const char* component() const override {
        return "number";
    }

    void toJson(JsonObject json) const override {
        ValueDiscovery::toJson(json);

        json["min"] = min;
        json["max"] = max;
        json["step"] = step;
        json["mode"] = mode;
    }

    void fromJson(JsonObject json, bool update = false) override {
        ValueDiscovery::fromJson(json, update);

        min = json["min"] | min;
        max = json["max"] | max;
        step = json["step"] | step;
        mode = json["mode"] | mode;
    }
};

struct ButtonDiscovery : ControlDiscovery {
    const char* component() const override {
        return "button";
    }

    void toJson(JsonObject json) const override {
        ControlDiscovery::toJson(json);
    }

    void fromJson(JsonObject json, bool update = false) override {
        ControlDiscovery::fromJson(json, update);
    }
};