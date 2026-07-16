#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <Mqtt/MqttTypes.h>
#include "EnumTransform.h"

struct ReadRegister
{
    uint16_t address;
    uint8_t rounding = 0;
    ReadRegisterTransform transform = ReadRegisterTransform::None;
    float transformArgument = 0.0f;

    MqttDiscoveryConfig discoveryConfig;

    void toJson(JsonObject json) const
    {
        json["address"] = address;
        json["rounding"] = rounding;
        json["transform"] = Enum::toString(transform);
        json["transformArgument"] = transformArgument;

        JsonObject discoveryJson = json["discoveryConfig"].to<JsonObject>();
        discoveryConfig.toJson(discoveryJson);
    }

    void fromJson(JsonObject json)
    {
        address = json["address"].as<uint16_t>();
        rounding = json["rounding"].as<uint8_t>();
        transform = Enum::fromString<ReadRegisterTransform>(json["transform"] | "None");
        transformArgument = json["transformArgument"].as<float>();

        JsonObject discoveryJson = json["discoveryConfig"].as<JsonObject>();
        discoveryConfig.fromJson(discoveryJson);
    }
};

struct ModbusDevice
{
    String name;
    String identifier;
    uint8_t slaveId = 5;
    uint32_t timeout = 1000;
    uint32_t baudrate = 2400;
    uint8_t port = 1;
    WordOrder wordOrder = WordOrder::BADC;

    MqttDiscoveryDevice discoveryDevice;

    std::vector<ReadRegister> readRegisters;

    void toJson(JsonObject json) const
    {
        json["name"] = name;
        json["identifier"] = identifier;
        json["slaveId"] = slaveId;
        json["timeout"] = timeout;
        json["baudrate"] = baudrate;
        json["port"] = port;
        json["wordOrder"] = Enum::toString(wordOrder);

        JsonArray readRegistersArray = json["readRegisters"].to<JsonArray>();
        for (const auto &readRegister : readRegisters)
        {
            JsonObject readRegisterJson = readRegistersArray.add<JsonObject>();
            readRegister.toJson(readRegisterJson);
        }

        auto device = discoveryDevice;
        device.setDeviceInfo(name, identifier);

        JsonObject discoveryDeviceJson = json["discoveryDevice"].to<JsonObject>();
        device.toJson(discoveryDeviceJson);
    }

    void fromJson(JsonObject json)
    {
        name = json["name"].as<String>();
        identifier = json["identifier"].as<String>();
        slaveId = json["slaveId"].as<uint8_t>();
        timeout = json["timeout"].as<uint32_t>();
        baudrate = json["baudrate"].as<uint32_t>();
        port = json["port"].as<uint8_t>();
        wordOrder = Enum::fromString<WordOrder>(json["wordOrder"] | "BADC");

        JsonObject discoveryDeviceJson = json["discoveryDevice"].as<JsonObject>();
        discoveryDevice.fromJson(discoveryDeviceJson);
        discoveryDevice.setDeviceInfo(name, identifier);

        JsonArray readRegistersArray = json["readRegisters"].as<JsonArray>();
        readRegisters.clear();
        for (const auto &readRegisterJson : readRegistersArray)
        {
            ReadRegister readRegister;
            readRegister.fromJson(readRegisterJson);
            readRegisters.push_back(readRegister);
        }
    }
};