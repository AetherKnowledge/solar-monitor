#pragma once

#include "RegisterTransform.h"
#include "WordOrder.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>
#include <Mqtt/MqttTypes.h>
#include <tinyexpr.h>
#include <vector>
#include <Common/Json.h>

template <typename T>
concept Discovery = std::derived_from<T, SensorDiscovery>;

template <Discovery TDiscovery>
struct Entity {
    double value = 0.0;
    TDiscovery discovery;

    String& getName() {
        return discovery.name;
    }

    String& getId() {
        return discovery.uniqueId;
    }

    void toJson(JsonObject json) const {
        JsonObject discoveryJson = json["discovery"].to<JsonObject>();
        discovery.toJson(discoveryJson);
    }

    void fromJson(JsonObject json) {
        JsonObject discoveryJson = json["discovery"].as<JsonObject>();
        discovery.fromJson(discoveryJson);
    }
};

template <typename TDiscovery>
struct Register : Entity<TDiscovery> {
    uint16_t address;

    void toJson(JsonObject json) const {
        Entity<TDiscovery>::toJson(json);
        json["address"] = this->address;
    }

    void fromJson(JsonObject json) {
        Entity<TDiscovery>::fromJson(json);
        address = json["address"].as<uint16_t>();
    }
};

struct ReadRegister : Register<SensorDiscovery> {
    uint8_t rounding = 0;
    RegisterTransform transform = RegisterTransform::None;
    float transformArgument = 0.0f;
    bool signedValue = false;

    void toJson(JsonObject json) const {
        Register<SensorDiscovery>::toJson(json);
        json["rounding"] = rounding;
        json["transform"] = Enum::toString(transform);
        json["transformArgument"] = transformArgument;
        json["signedValue"] = signedValue;
    }

    void fromJson(JsonObject json) {
        Register<SensorDiscovery>::fromJson(json);
        rounding = json["rounding"].as<uint8_t>();
        transform = Enum::fromString<RegisterTransform>(json["transform"] | "None");
        signedValue = json["signedValue"] | false;
        transformArgument = json["transformArgument"].as<float>();
    }
};

struct ReadGroup {
    uint16_t startAddress;
    uint16_t count;

    std::vector<ReadRegister*> registers;
};

struct VirtualSensor : Entity<SensorDiscovery> {
    String expression;
    te_expr* compiledExpression = nullptr;

    bool isPersistent = false;
    // only updates if persistence is true
    bool isDirty = false;

    void toJson(JsonObject json) const {
        Entity<SensorDiscovery>::toJson(json);

        json["expression"] = expression;
        json["isPersistent"] = false;
    }

    void fromJson(JsonObject json) {
        Entity<SensorDiscovery>::fromJson(json);

        expression = json["expression"].as<String>();
        isPersistent = json["isPersistent"] | false;
    }
};

template <typename T>
concept WriteRegisterType = requires(T reg) {
    reg.discovery.commandTopic;
    reg.value;
};

struct SelectWriteRegister : Register<SelectDiscovery> {};

struct NumberWriteRegister : Register<NumberDiscovery> {};

struct ModbusDevice {
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

    std::vector<SelectWriteRegister> selectWriteRegisters;
    std::vector<NumberWriteRegister> numberWriteRegisters;

    void toJson(JsonObject json) const {
        json["slaveId"] = slaveId;
        json["timeout"] = timeout;
        json["baudrate"] = baudrate;
        json["port"] = port;
        json["swapBytes"] = swapBytes;
        json["mqttEnabled"] = mqttEnabled;

        serializeVector(json["readRegisters"], readRegisters);
        serializeVector(json["virtualSensors"], virtualSensors);
        serializeVector(json["selectWriteRegisters"], selectWriteRegisters);
        serializeVector(json["numberWriteRegisters"], numberWriteRegisters);

        JsonObject discoveryJson = json["discovery"].to<JsonObject>();
        discovery.toJson(discoveryJson);
    }

    void fromJson(JsonObject json) {
        slaveId = json["slaveId"].as<uint8_t>();
        timeout = json["timeout"].as<uint32_t>();
        baudrate = json["baudrate"].as<uint32_t>();
        port = json["port"].as<uint8_t>();
        swapBytes = json["swapBytes"] | false;
        mqttEnabled = json["mqttEnabled"] | true;

        JsonObject discoveryJson = json["discovery"].as<JsonObject>();
        discovery.fromJson(discoveryJson);

        deserializeVector(json["readRegisters"], readRegisters);
        deserializeVector(json["virtualSensors"], virtualSensors);
        deserializeVector(json["selectWriteRegisters"], selectWriteRegisters);
        deserializeVector(json["numberWriteRegisters"], numberWriteRegisters);
    }

    String toString() const {
        String result = "Name: " + discovery.name + "\n";
        result += "Identifier: " + discovery.identifier + "\n";
        result += "Slave ID: " + String(slaveId) + "\n";
        result += "Timeout: " + String(timeout) + "\n";
        result += "Baudrate: " + String(baudrate) + "\n";
        result += "Port: " + String(port) + "\n";
        result += "Swap Bytes: " + String(swapBytes ? "true" : "false") + "\n";
        result += "Read Registers Count: " + String(readRegisters.size()) + "\n";
        result += "Virtual Sensors Count: " + String(virtualSensors.size()) + "\n";
        result += "Select Write Registers Count: " + String(selectWriteRegisters.size()) + "\n";
        result += "Number Write Registers Count: " + String(numberWriteRegisters.size()) + "\n";
        return result;
    }
};