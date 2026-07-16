#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>

struct MqttDiscoveryDevice
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

        manufacturer = json["manufacturer"].as<String>();
        model = json["model"].as<String>();
        name = json["name"].as<String>();
    }
};

struct MqttDiscoveryConfig
{
    String name;
    String uniqueId;
    String stateTopic;

    String deviceClass;
    String stateClass;
    String unitOfMeasurement;
    String icon;

    void toJson(JsonObject json) const
    {
        json["name"] = name;
        json["unique_id"] = uniqueId;
        json["state_topic"] = stateTopic;

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
        name = json["name"].as<String>();
        uniqueId = json["unique_id"].as<String>();
        stateTopic = json["state_topic"].as<String>();

        deviceClass = json["device_class"].as<String>();
        stateClass = json["state_class"].as<String>();
        unitOfMeasurement = json["unit_of_measurement"].as<String>();
        icon = json["icon"].as<String>();
    }
};