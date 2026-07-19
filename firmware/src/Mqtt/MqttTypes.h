#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>

struct DeviceDiscovery
{
    String identifier;
    String manufacturer;
    String model;
    String name;

    void setDeviceInfo(const String &deviceName, const String &deviceIdentifier)
    {
        name = deviceName;
        identifier = deviceIdentifier;
    }

    void toJson(JsonObject json) const
    {
        JsonArray identifiers = json["identifiers"].to<JsonArray>();
        identifiers.add(identifier);

        json["manufacturer"] = manufacturer;

        if (!model.isEmpty())
            json["model"] = model;

        if (!name.isEmpty())
            json["name"] = name;
    }

    void fromJson(JsonObject json)
    {
        JsonArray identifiers = json["identifiers"].as<JsonArray>();
        if (!identifiers.isNull() && identifiers.size() > 0)
        {
            identifier = identifiers[0].as<String>();
        }
        else
        {
            identifier = "";
        }

        manufacturer = json["manufacturer"] | "";
        model = json["model"] | "";
        name = json["name"] | "";
    }
};

struct SensorDiscovery
{
    String name;
    String uniqueId;

    String deviceClass;
    String stateClass;
    String unitOfMeasurement;
    String icon;

    void toJson(JsonObject json) const
    {
        json["name"] = name;
        json["unique_id"] = uniqueId;

        if (!stateClass.isEmpty())
            json["state_class"] = stateClass;

        if (!unitOfMeasurement.isEmpty())
            json["unit_of_measurement"] = unitOfMeasurement;

        if (!deviceClass.isEmpty())
            json["device_class"] = deviceClass;

        if (!icon.isEmpty())
            json["icon"] = icon;
    }

    void fromJson(JsonObject json)
    {
        uniqueId = json["unique_id"] | "";
        name = json["name"] | uniqueId;

        deviceClass = json["device_class"] | "";
        stateClass = json["state_class"] | "";
        unitOfMeasurement = json["unit_of_measurement"] | "";
        icon = json["icon"] | "";
    }
};

struct WriteDiscovery : SensorDiscovery
{
    String commandTopic;
    String commandTemplate;
    uint8_t qos = 0;

    void toJson(JsonObject json) const
    {
        SensorDiscovery::toJson(json);

        json["command_topic"] = commandTopic;
        json["command_template"] = commandTemplate;
        json["qos"] = qos;
    }

    void fromJson(JsonObject json)
    {
        SensorDiscovery::fromJson(json);

        commandTopic = json["command_topic"] | "";
        commandTemplate = json["command_template"] | "";
        qos = json["qos"] | 0;
    }
};

struct SelectDiscovery : WriteDiscovery
{
    std::vector<String> options;

    void toJson(JsonObject json) const
    {
        WriteDiscovery::toJson(json);

        JsonArray optionsArray = json["options"].to<JsonArray>();
        for (const auto &option : options)
        {
            optionsArray.add(option);
        }
    }

    void fromJson(JsonObject json)
    {
        WriteDiscovery::fromJson(json);

        JsonArray optionsArray = json["options"].as<JsonArray>();
        options.clear();
        for (const auto &optionJson : optionsArray)
        {
            String option = optionJson.as<String>();
            options.push_back(option);
        }
    }
};

struct NumberDiscovery : WriteDiscovery
{
    double min = 0;
    double max = 100;
    double step = 1;

    void toJson(JsonObject json) const
    {
        WriteDiscovery::toJson(json);

        json["min"] = min;
        json["max"] = max;
        json["step"] = step;
    }

    void fromJson(JsonObject json)
    {
        WriteDiscovery::fromJson(json);

        min = json["min"] | 0;
        max = json["max"] | 100;
        step = json["step"] | 1;
    }
};