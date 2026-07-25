#pragma once

#include "RegisterTransform.h"
#include "WordOrder.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>
#include <Mqtt/MqttTypes.h>
#include <tinyexpr.h>
#include <cstdint>
#include <vector>
#include <Common/Json.h>

struct EntityBase {
    virtual ~EntityBase() = default;

    double value = 0;
    int8_t displayIndex = -1;

    virtual const String& getName() const = 0;
    virtual const String& getId() const = 0;

    virtual Discovery& getDiscovery() = 0;
    virtual const Discovery& getDiscovery() const = 0;

    virtual void toJson(JsonObject json) const = 0;
    virtual void fromJson(JsonObject json) = 0;
};

struct Entity : EntityBase {
    const String& getName() const override {
        return getDiscovery().name;
    }

    const String& getId() const override {
        return getDiscovery().uniqueId;
    }

    void toJson(JsonObject json) const override {
        json["displayIndex"] = this->displayIndex;
        JsonObject discoveryJson = json["discovery"].to<JsonObject>();
        getDiscovery().toJson(discoveryJson);
    }

    void fromJson(JsonObject json) override {
        displayIndex = json["displayIndex"] | -1;
        JsonObject discoveryJson = json["discovery"].as<JsonObject>();
        getDiscovery().fromJson(discoveryJson);
    }
};

struct Register : Entity {
    uint16_t address;

    void toJson(JsonObject json) const override {
        Entity::toJson(json);
        json["address"] = this->address;
    }

    void fromJson(JsonObject json) override {
        Entity::fromJson(json);
        address = json["address"].as<uint16_t>();
    }
};

struct ReadRegister : Register {
    uint8_t rounding = 0;
    RegisterTransform transform = RegisterTransform::None;
    float transformArgument = 0.0f;
    bool signedValue = false;

    SensorDiscovery discovery;

    void toJson(JsonObject json) const override {
        Register::toJson(json);
        json["rounding"] = rounding;
        json["transform"] = Enum::toString(transform);
        json["transformArgument"] = transformArgument;
        json["signedValue"] = signedValue;
    }

    void fromJson(JsonObject json) override {
        Register::fromJson(json);
        rounding = json["rounding"].as<uint8_t>();
        transform = Enum::fromString<RegisterTransform>(json["transform"] | "None");
        signedValue = json["signedValue"] | false;
        transformArgument = json["transformArgument"].as<float>();
    }

    SensorDiscovery& getDiscovery() override {
        return discovery;
    }

    const SensorDiscovery& getDiscovery() const override {
        return discovery;
    }
};

struct ReadGroup {
    uint16_t startAddress;
    uint16_t count;

    std::vector<ReadRegister*> registers;
};

struct VirtualSensor : Entity {
    uint8_t rounding = 0;
    String expression;
    te_expr* compiledExpression = nullptr;

    SensorDiscovery discovery;

    bool isPersistent = false;
    // only updates if persistence is true
    bool isDirty = false;

    void toJson(JsonObject json) const override {
        Entity::toJson(json);

        json["expression"] = expression;
        json["isPersistent"] = isPersistent;
        json["rounding"] = rounding;
    }

    void fromJson(JsonObject json) override {
        Entity::fromJson(json);

        expression = json["expression"].as<String>();
        isPersistent = json["isPersistent"] | false;
        rounding = json["rounding"] | 0;
    }

    SensorDiscovery& getDiscovery() override {
        return discovery;
    }

    const SensorDiscovery& getDiscovery() const override {
        return discovery;
    }
};

struct WriteRegister : Register {
    virtual ~WriteRegister() = default;

    virtual WriteDiscovery& getDiscovery() override = 0;
    virtual const WriteDiscovery& getDiscovery() const override = 0;
};

struct SelectWriteRegister : WriteRegister {
    SelectDiscovery discovery;

    const SelectDiscovery& getDiscovery() const override {
        return discovery;
    }

    SelectDiscovery& getDiscovery() override {
        return discovery;
    }
};

struct NumberWriteRegister : WriteRegister {
    NumberDiscovery discovery;

    const NumberDiscovery& getDiscovery() const override {
        return discovery;
    }

    NumberDiscovery& getDiscovery() override {
        return discovery;
    }
};

// Id rather not mutex this lol
struct DisplayData {
    struct EntityData {
        int8_t index;
        String name;
        double value;
    };

    String deviceName;
    bool modbusConnected = false;
    std::vector<EntityData> entities;
};

struct DisplayItem {
    EntityBase* entity;

    const String toString() const {
        return "DisplayItem: " + entity->getName() + " (ID: " + entity->getId() +
               ", Display Index: " + String(entity->displayIndex) + ")";
    }
};

struct ModbusDevice {
    uint8_t slaveId = 5;
    uint32_t timeout = 1000;
    uint32_t baudrate = 2400;
    uint8_t port = 1;
    bool swapBytes = false;

    DeviceDiscovery discovery;
    bool mqttEnabled = true;

    ModbusMaster modbus;
    bool modbusConnected = false;
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

    DisplayData createDisplayData() const {
        DisplayData data;
        data.deviceName = discovery.name;
        int16_t totalDisplayItems = readRegisters.size() + virtualSensors.size() +
                                    selectWriteRegisters.size() + numberWriteRegisters.size();

        data.entities.reserve(totalDisplayItems);
        data.modbusConnected = modbusConnected;

        for (auto& reg : readRegisters) {
            if (reg.displayIndex >= 0) {
                data.entities.push_back(
                    DisplayData::EntityData{reg.displayIndex, reg.getName(), reg.value});
            }
        }
        for (auto& sensor : virtualSensors) {
            if (sensor.displayIndex >= 0) {
                data.entities.push_back(
                    DisplayData::EntityData{sensor.displayIndex, sensor.getName(), sensor.value});
            }
        }
        for (auto& reg : selectWriteRegisters) {
            if (reg.displayIndex >= 0) {
                data.entities.push_back(
                    DisplayData::EntityData{reg.displayIndex, reg.getName(), reg.value});
            }
        }
        for (auto& reg : numberWriteRegisters) {
            if (reg.displayIndex >= 0) {
                data.entities.push_back(
                    DisplayData::EntityData{reg.displayIndex, reg.getName(), reg.value});
            }
        }

        return data;
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