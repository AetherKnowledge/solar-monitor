#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>
#include <vector>
#include <Mqtt/MqttTypes.h>
#include "RegisterTransform.h"
#include "WordOrder.h"
#include <ModbusMaster.h>
#include <tinyexpr.h>

template <typename TDiscovery>
struct Entity
{
    static_assert(std::is_base_of_v<SensorDiscovery, TDiscovery>,
                  "TDiscovery must derive from SensorDiscovery");

    double value = 0.0;
    TDiscovery discovery;

    String &getName()
    {
        return discovery.name;
    }

    String &getId()
    {
        return discovery.uniqueId;
    }

    void toJson(JsonObject json) const
    {
        JsonObject discoveryJson = json["discovery"].to<JsonObject>();
        discovery.toJson(discoveryJson);
    }

    void fromJson(JsonObject json)
    {
        JsonObject discoveryJson = json["discovery"].as<JsonObject>();
        discovery.fromJson(discoveryJson);
    }
};

template <typename TDiscovery>
struct Register : Entity<TDiscovery>
{
    uint16_t address;

    void toJson(JsonObject json) const
    {
        Entity<TDiscovery>::toJson(json);
        json["address"] = this->address;
    }

    void fromJson(JsonObject json)
    {
        Entity<TDiscovery>::fromJson(json);
        address = json["address"].as<uint16_t>();
    }
};

struct ReadRegister : Register<SensorDiscovery>
{
    uint8_t rounding = 0;
    RegisterTransform transform = RegisterTransform::None;
    float transformArgument = 0.0f;
    bool signedValue = false;

    void toJson(JsonObject json) const
    {
        Register<SensorDiscovery>::toJson(json);
        json["rounding"] = rounding;
        json["transform"] = Enum::toString(transform);
        json["transformArgument"] = transformArgument;
        json["signedValue"] = signedValue;
    }

    void fromJson(JsonObject json)
    {
        Register<SensorDiscovery>::fromJson(json);
        rounding = json["rounding"].as<uint8_t>();
        transform = Enum::fromString<RegisterTransform>(json["transform"] | "None");
        signedValue = json["signedValue"] | false;
        transformArgument = json["transformArgument"].as<float>();
    }
};

struct ReadGroup
{
    uint16_t startAddress;
    uint16_t count;

    std::vector<ReadRegister *> registers;
};

struct VirtualSensor : Entity<SensorDiscovery>
{
    String expression;
    te_expr *compiledExpression = nullptr;

    bool isPersistent = false;
    // only updates if persistence is true
    bool isDirty = false;

    void toJson(JsonObject json) const
    {
        Entity<SensorDiscovery>::toJson(json);

        json["expression"] = expression;
        json["isPersistent"] = false;
    }

    void fromJson(JsonObject json)
    {
        Entity<SensorDiscovery>::fromJson(json);

        expression = json["expression"].as<String>();
        isPersistent = json["isPersistent"] | false;
    }
};

struct SelectWriteRegister : Register<SelectDiscovery>
{
};

struct NumberWriteRegister : Register<NumberDiscovery>
{
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

    DeviceDiscovery discovery;
    bool mqttEnabled = true;

    ModbusMaster modbus;
    bool initialized = false;

    std::vector<ReadRegister> readRegisters;
    std::vector<ReadGroup> readGroups;

    std::vector<VirtualSensor> virtualSensors;
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

        JsonArray virtualSensorsArray = json["virtualSensors"].to<JsonArray>();
        for (const auto &virtualSensor : virtualSensors)
        {
            JsonObject virtualSensorJson = virtualSensorsArray.add<JsonObject>();
            virtualSensor.toJson(virtualSensorJson);
        }

        auto device = discovery;
        device.setDeviceInfo(name, identifier);

        JsonObject discoveryJson = json["discovery"].to<JsonObject>();
        device.toJson(discoveryJson);
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

        JsonObject discoveryJson = json["discovery"].as<JsonObject>();
        discovery.fromJson(discoveryJson);
        discovery.setDeviceInfo(name, identifier);

        JsonArray readRegistersArray = json["readRegisters"].as<JsonArray>();
        readRegisters.clear();
        for (const auto &readRegisterJson : readRegistersArray)
        {
            ReadRegister readRegister;
            readRegister.fromJson(readRegisterJson);
            readRegisters.push_back(readRegister);
        }

        JsonArray virtualSensorsArray = json["virtualSensors"].as<JsonArray>();
        virtualSensors.clear();
        for (const auto &virtualSensorJson : virtualSensorsArray)
        {
            VirtualSensor virtualSensor;
            virtualSensor.fromJson(virtualSensorJson);
            virtualSensors.push_back(virtualSensor);
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
        result += "Virtual Sensors Count: " + String(virtualSensors.size()) + "\n";
        return result;
    }
};