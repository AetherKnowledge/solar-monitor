#pragma once

#include <Modbus/ModbusTypes.h>
#include <Mqtt/MqttTypes.h>
#include <Common/Json.h>
#include "ArduinoJson/Object/JsonObject.hpp"

namespace SystemManager {

    struct SystemSensor : Entity {
        SensorDiscovery discovery;

        void toJson(JsonObject json) const override {
            json[discovery.uniqueId] = value;
        }

        const SensorDiscovery& getDiscovery() const override {
            return discovery;
        }

        SensorDiscovery& getDiscovery() override {
            return discovery;
        }
    };

    struct SystemControl : ControlEntity {
        std::function<bool(const Device&, const String&)> callback;
        ButtonDiscovery discovery;

        bool execute(Device& device, const String& payload) override {
            if (callback) {
                return callback(device, payload);
            }
            return false;
        }

        const ButtonDiscovery& getDiscovery() const override {
            return discovery;
        }

        ButtonDiscovery& getDiscovery() override {
            return discovery;
        }
    };

    struct SystemDevice : Device {
        std::vector<std::reference_wrapper<SystemSensor>> sensors;
        std::vector<std::reference_wrapper<SystemControl>> controls;

        void toJson(JsonDocument& doc) const {
            JsonObject json = doc["sensors"].to<JsonObject>();
            addValues(json, sensors, discovery.identifier.c_str());
        }

        void forEachEntity(const std::function<void(Entity&)>& fn) override {
            for (auto& r : sensors) fn(r);

            for (auto& c : controls) fn(c);
        }

        void forEachEntity(const std::function<void(const Entity&)>& fn) const override {
            for (const auto& r : sensors) fn(r);

            for (const auto& c : controls) fn(c);
        }

        void forEachControlEntity(const std::function<void(ControlEntity&)>& fn) override {
            for (auto& c : controls) fn(c);
        }

        void forEachControlEntity(
            const std::function<void(const ControlEntity&)>& fn) const override {
            for (const auto& c : controls) fn(c);
        }

        SystemControl* findControlEntity(const String& topic) override {
            for (auto& c : controls) {
                auto& control = c.get();

                if (control.getDiscovery().commandTopic == topic) {
                    return &control;
                }
            }
            return nullptr;
        }
    };

    [[nodiscard]]
    inline SystemSensor makeSensor(const DeviceDiscovery& deviceDiscovery,
                                   String name,
                                   String id,
                                   String deviceClass = "",
                                   String unit = "") {
        SystemSensor e;
        e.discovery.name = std::move(name);
        e.discovery.uniqueId = std::move(deviceDiscovery.identifier + "_" + id);
        e.discovery.deviceClass = std::move(deviceClass);
        e.discovery.unitOfMeasurement = std::move(unit);
        return e;
    }

    [[nodiscard]]
    inline SystemDevice makeDevice(String identifier,
                                   String manufacturer,
                                   String model,
                                   String name) {
        SystemDevice e;
        e.discovery.identifier = std::move(identifier);
        e.discovery.manufacturer = std::move(manufacturer);
        e.discovery.model = std::move(model);
        e.discovery.name = std::move(name);
        return e;
    }

}  // namespace SystemManager