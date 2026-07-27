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
#include <Common/Logger.h>

struct Device;
struct ModbusDevice;

struct Entity {
    virtual ~Entity() = default;

    double value = 0;
    int8_t displayIndex = -1;

    virtual Discovery& getDiscovery() = 0;
    virtual const Discovery& getDiscovery() const = 0;

    const String& getName() const {
        return getDiscovery().name;
    }

    const String& getId() const {
        return getDiscovery().uniqueId;
    }

    virtual void toJson(JsonObject json) const {
        json["displayIndex"] = displayIndex;

        JsonObject discoveryJson = json["discovery"].to<JsonObject>();
        getDiscovery().toJson(discoveryJson);
    }

    virtual void fromJson(JsonObject json) {
        displayIndex = json["displayIndex"] | -1;

        JsonObject discoveryJson = json["discovery"].as<JsonObject>();
        getDiscovery().fromJson(discoveryJson);
    }
};

struct ControlEntity : Entity {
    virtual ControlDiscovery& getDiscovery() override = 0;
    virtual const ControlDiscovery& getDiscovery() const override = 0;

    virtual bool execute(Device& device, const String& payload) = 0;
};

struct Register {
    uint16_t address;

    void toJson(JsonObject json) const {
        json["address"] = this->address;
    }

    void fromJson(JsonObject json) {
        address = json["address"].as<uint16_t>();
    }
};

struct ReadRegister : Register, Entity {
    uint8_t rounding = 0;
    RegisterTransform transform = RegisterTransform::None;
    float transformArgument = 0.0f;
    bool signedValue = false;

    SensorDiscovery discovery;

    void toJson(JsonObject json) const override {
        Register::toJson(json);
        Entity::toJson(json);

        json["rounding"] = rounding;
        json["transform"] = Enum::toString(transform);
        json["transformArgument"] = transformArgument;
        json["signedValue"] = signedValue;
    }

    void fromJson(JsonObject json) override {
        Register::fromJson(json);
        Entity::fromJson(json);

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

struct WriteRegister : Register, ControlEntity {
    virtual ControlDiscovery& getDiscovery() override = 0;
    virtual const ControlDiscovery& getDiscovery() const override = 0;

    bool execute(Device&, const String&) override;

    void toJson(JsonObject json) const override {
        Register::toJson(json);
        ControlEntity::toJson(json);
    }

    void fromJson(JsonObject json) override {
        Register::fromJson(json);
        ControlEntity::fromJson(json);
    }
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

struct Device {
    virtual ~Device() = default;

    DeviceDiscovery discovery;
    bool mqttEnabled = true;
    bool initialized = false;

    virtual void forEachEntity(const std::function<void(Entity&)>& fn) = 0;
    virtual void forEachControlEntity(const std::function<void(ControlEntity&)>& fn) = 0;

    virtual void forEachEntity(const std::function<void(const Entity&)>& fn) const = 0;
    virtual void forEachControlEntity(
        const std::function<void(const ControlEntity&)>& fn) const = 0;

    virtual ControlEntity* findControlEntity(const String& topic) = 0;

    void execute(char* topic, byte* payload, unsigned int length) {
        String value((char*)payload, length);

        auto* entity = findControlEntity(topic);
        if (entity) {
            entity->execute(*this, value);
            return;
        }
    }

    void generateTopics() {
        forEachEntity([&](Entity& entity) {
            entity.getDiscovery().stateTopic =
                discovery.identifier + "/" + entity.getDiscovery().uniqueId;
        });

        forEachControlEntity([&](ControlEntity& entity) {
            entity.getDiscovery().commandTopic =
                discovery.identifier + "/" + entity.getDiscovery().uniqueId + "/set";
        });
    }
};

struct ModbusDevice : Device {
    uint8_t slaveId = 5;
    uint32_t timeout = 1000;
    uint32_t baudrate = 2400;
    uint8_t port = 1;
    bool swapBytes = false;

    ModbusMaster modbus;
    bool modbusConnected = false;

    std::vector<te_variable> vars;

    std::vector<ReadRegister> readRegisters;
    std::vector<ReadGroup> readGroups;
    std::vector<VirtualSensor> virtualSensors;
    std::vector<SelectWriteRegister> selectWriteRegisters;
    std::vector<NumberWriteRegister> numberWriteRegisters;

    void forEachEntity(const std::function<void(Entity&)>& fn) override {
        for (auto& r : readRegisters) fn(r);
        for (auto& v : virtualSensors) fn(v);
        for (auto& s : selectWriteRegisters) fn(s);
        for (auto& n : numberWriteRegisters) fn(n);
    }

    void forEachEntity(const std::function<void(const Entity&)>& fn) const override {
        for (const auto& reg : readRegisters) fn(reg);
        for (const auto& sensor : virtualSensors) fn(sensor);
        for (const auto& reg : selectWriteRegisters) fn(reg);
        for (const auto& reg : numberWriteRegisters) fn(reg);
    }

    void forEachControlEntity(const std::function<void(ControlEntity&)>& fn) override {
        for (auto& s : selectWriteRegisters) fn(s);
        for (auto& n : numberWriteRegisters) fn(n);
    }

    void forEachControlEntity(const std::function<void(const ControlEntity&)>& fn) const override {
        for (const auto& s : selectWriteRegisters) fn(s);
        for (const auto& n : numberWriteRegisters) fn(n);
    }

    ControlEntity* findControlEntity(const String& topic) override {
        for (auto& s : selectWriteRegisters) {
            if (s.getDiscovery().commandTopic == topic) {
                return &s;
            }
        }
        for (auto& n : numberWriteRegisters) {
            if (n.getDiscovery().commandTopic == topic) {
                return &n;
            }
        }
        return nullptr;
    }

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

        generateTopics();
    }

    DisplayData createDisplayData() const {
        DisplayData data;
        data.entities.reserve(readRegisters.size() + virtualSensors.size() +
                              selectWriteRegisters.size() + numberWriteRegisters.size());

        data.deviceName = discovery.name;
        data.modbusConnected = modbusConnected;

        forEachEntity([&](const Entity& entity) {
            if (entity.displayIndex >= 0) {
                data.entities.push_back({entity.displayIndex, entity.getName(), entity.value});
            }
        });

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