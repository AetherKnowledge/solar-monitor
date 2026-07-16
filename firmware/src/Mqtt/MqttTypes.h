#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>

struct MqttDiscoveryDevice
{
    String identifier;
    String manufacturer;
    String model;
    String name;

    void toJson(JsonObject json) const
    {
        JsonArray ids = json["identifiers"].to<JsonArray>();
        ids.add(identifier);

        json["manufacturer"] = manufacturer;

        if (!model.isEmpty())
            json["model"] = model;

        if (!name.isEmpty())
            json["name"] = name;
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
    MqttDiscoveryDevice device;
    String icon;

    void toJson(JsonDocument &doc) const
    {
        JsonObject json = doc.to<JsonObject>();

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

        device.toJson(json["device"].to<JsonObject>());
    }
};