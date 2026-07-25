#include "MqttDiscovery.h"
#include "MqttManager.h"
#include <Config/ConfigManager.h>
#include <Mqtt/MqttTypes.h>
#include <Common/Logger.h>

namespace MqttDiscovery {
    void start() {
        SensorDiscovery discovery;
        discovery.name = "Solar Monitor Uptime";
        discovery.uniqueId = "solar-monitor-uptime";
        discovery.unitOfMeasurement = "s";
        discovery.stateClass = "measurement";

        JsonDocument doc;
        JsonObject json = doc.to<JsonObject>();
        discovery.toJson(json);
        json["state_topic"] = "solar-monitor/uptime";
        JsonObject device = json["device"].to<JsonObject>();
        device["name"] = "Solar Monitor";
        device["manufacturer"] = "Solar Monitor";
        JsonArray identifiers = device["identifiers"].to<JsonArray>();
        identifiers.add("solar-monitor");

        MqttManager::publish("homeassistant/sensor/solar-monitor/uptime/config", doc);

        for (const auto& device : ConfigManager::config.modbusDevices) {
            auto publishedDiscoveryCount = 0;

            for (const auto& readRegister : device.readRegisters) {
                if (publishDiscovery(device, readRegister.discovery)) {
                    publishedDiscoveryCount++;
                }
            }

            for (const auto& virtualSensor : device.virtualSensors) {
                if (publishDiscovery(device, virtualSensor.discovery)) {
                    publishedDiscoveryCount++;
                }
            }

            for (const auto& writeRegister : device.numberWriteRegisters) {
                if (publishDiscovery(device, writeRegister.discovery)) {
                    publishedDiscoveryCount++;
                }
            }

            for (const auto& writeRegister : device.selectWriteRegisters) {
                if (publishDiscovery(device, writeRegister.discovery)) {
                    publishedDiscoveryCount++;
                }
            }

            Log.printf("Published %d discovery messages for Modbus device %s (%s)\n",
                       publishedDiscoveryCount,
                       device.discovery.name.c_str(),
                       device.discovery.identifier.c_str());
        }
    }

    bool publishDiscovery(const ModbusDevice& device, const Discovery& discovery) {
        if (discovery.name.isEmpty() || discovery.uniqueId.isEmpty()) {
            Log.printf("Skipping discovery for device %s (%s) due to missing name or uniqueId\n",
                       device.discovery.name.c_str(),
                       device.discovery.identifier.c_str());
            return false;
        }

        JsonDocument doc;
        JsonObject json = doc.to<JsonObject>();

        discovery.toJson(json);

        JsonObject deviceJson = json["device"].to<JsonObject>();
        device.discovery.toJson(deviceJson);

        JsonArray identifiers = deviceJson["identifiers"].to<JsonArray>();
        identifiers.add(device.discovery.identifier);
        deviceJson.remove("identifier");

        bool result = MqttManager::publish(generateDiscoveryTopic(device, discovery), doc, true);

        return result;
    }

    String generateDiscoveryTopic(const ModbusDevice& device, const Discovery& discovery) {
        return ConfigManager::config.mqtt.autoDiscoveryPrefix + "/" + discovery.component() + "/" +
               device.discovery.identifier + "/" + discovery.uniqueId + "/config";
    }

    String generateStateTopic(const ModbusDevice& device, const Discovery& discovery) {
        return device.discovery.identifier + "/" + discovery.uniqueId;
    }

    String generateCommandTopic(const ModbusDevice& device, const WriteDiscovery& discovery) {
        return device.discovery.identifier + "/" + discovery.uniqueId + "/set";
    }

}  // namespace MqttDiscovery