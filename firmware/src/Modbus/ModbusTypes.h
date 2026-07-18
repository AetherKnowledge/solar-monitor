#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <Mqtt/MqttTypes.h>
#include "RegisterTransform.h"
#include "WordOrder.h"
#include <ModbusMaster.h>
#include <tinyexpr.h>

struct Register
{
    double value = 0.0;
    MqttSensorDiscoveryConfig discoveryConfig;

    String &getName()
    {
        return discoveryConfig.name;
    }

    String &getId()
    {
        return discoveryConfig.uniqueId;
    }
};

struct ReadRegister : Register
{
    uint16_t address;
    uint8_t rounding = 0;
    RegisterTransform transform = RegisterTransform::None;
    float transformArgument = 0.0f;
    bool signedValue = false;

    void toJson(JsonObject json) const
    {
        json["address"] = address;
        json["rounding"] = rounding;
        json["transform"] = Enum::toString(transform);
        json["transformArgument"] = transformArgument;
        json["signedValue"] = signedValue;

        JsonObject discoveryJson = json["discoveryConfig"].to<JsonObject>();
        discoveryConfig.toJson(discoveryJson);
    }

    void fromJson(JsonObject json)
    {
        address = json["address"].as<uint16_t>();
        rounding = json["rounding"].as<uint8_t>();
        transform = Enum::fromString<RegisterTransform>(json["transform"] | "None");
        signedValue = json["signedValue"] | false;
        transformArgument = json["transformArgument"].as<float>();

        JsonObject discoveryJson = json["discoveryConfig"].as<JsonObject>();
        discoveryConfig.fromJson(discoveryJson);
    }
};

struct ReadGroup
{
    uint16_t startAddress;
    uint16_t count;

    std::vector<ReadRegister *> registers;
};

struct CalculatedRegister : Register
{
    String expression;
    te_expr *compiledExpression = nullptr;

    bool isPersistent = false;
    // only updates if persistence is true
    bool isDirty = false;

    void toJson(JsonObject json) const
    {
        json["expression"] = expression;
        json["isPersistent"] = false;

        JsonObject discoveryJson = json["discoveryConfig"].to<JsonObject>();
        discoveryConfig.toJson(discoveryJson);
    }

    void fromJson(JsonObject json)
    {
        expression = json["expression"].as<String>();
        isPersistent = json["isPersistent"] | false;

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
    bool swapBytes = false;

    MqttDiscoveryDevice discoveryDevice;
    bool mqttEnabled = true;

    ModbusMaster modbus;
    bool initialized = false;

    std::vector<ReadRegister> readRegisters;
    std::vector<ReadGroup> readGroups;

    std::vector<CalculatedRegister> calculatedRegisters;
    std::vector<te_variable> vars;

    void toJson(JsonObject json) const
    {
        json["name"] = name;
        json["identifier"] = identifier;
        json["slaveId"] = slaveId;
        json["timeout"] = timeout;
        json["baudrate"] = baudrate;
        json["port"] = port;
        json["swapBytes"] = swapBytes;
        json["mqttEnabled"] = mqttEnabled;

        JsonArray readRegistersArray = json["readRegisters"].to<JsonArray>();
        for (const auto &readRegister : readRegisters)
        {
            JsonObject readRegisterJson = readRegistersArray.add<JsonObject>();
            readRegister.toJson(readRegisterJson);
        }

        JsonArray calculatedRegistersArray = json["calculatedRegisters"].to<JsonArray>();
        for (const auto &calculatedRegister : calculatedRegisters)
        {
            JsonObject calculatedRegisterJson = calculatedRegistersArray.add<JsonObject>();
            calculatedRegister.toJson(calculatedRegisterJson);
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
        swapBytes = json["swapBytes"] | false;
        mqttEnabled = json["mqttEnabled"] | true;

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

        JsonArray calculatedRegistersArray = json["calculatedRegisters"].as<JsonArray>();
        calculatedRegisters.clear();
        for (const auto &calculatedRegisterJson : calculatedRegistersArray)
        {
            CalculatedRegister calculatedRegister;
            calculatedRegister.fromJson(calculatedRegisterJson);
            calculatedRegisters.push_back(calculatedRegister);
        }
    }

    String toString() const
    {
        String result = "Name: " + name + "\n";
        result += "Identifier: " + identifier + "\n";
        result += "Slave ID: " + String(slaveId) + "\n";
        result += "Timeout: " + String(timeout) + "\n";
        result += "Baudrate: " + String(baudrate) + "\n";
        result += "Port: " + String(port) + "\n";
        result += "Swap Bytes: " + String(swapBytes ? "true" : "false") + "\n";
        result += "Read Registers Count: " + String(readRegisters.size()) + "\n";
        result += "Calculated Registers Count: " + String(calculatedRegisters.size()) + "\n";
        return result;
    }
};