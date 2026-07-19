#include "MqttDiscovery.h"
#include "MqttManager.h"
#include <Config/Config.h>
#include <Mqtt/MqttTypes.h>

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

        for (const auto& device : config.modbusDevices) {
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

            Serial.printf("Published %d discovery messages for Modbus device %s (%s)\n",
                          publishedDiscoveryCount,
                          device.name.c_str(),
                          device.identifier.c_str());
        }
    }

    template <std::derived_from<SensorDiscovery> TDiscovery>
    bool publishDiscovery(const ModbusDevice& device, const TDiscovery& discovery) {
        JsonDocument doc;
        JsonObject json = doc.to<JsonObject>();

        discovery.toJson(json);

        JsonObject deviceJson = json["device"].to<JsonObject>();
        device.discovery.toJson(deviceJson);

        bool result = MqttManager::publish(
            generateDiscoveryTopic(device.identifier, discovery.uniqueId, TDiscovery::Component),
            doc,
            true);

        Serial.printf("%s to publish discovery for %s (%s) of device %s (%s)\n",
                      result ? "Successfully" : "Failed",
                      discovery.name.c_str(),
                      discovery.uniqueId.c_str(),
                      device.name.c_str(),
                      device.identifier.c_str());
        Serial.printf("Discovery JSON: %s\n", doc.as<String>().c_str());

        return result;
    }

    String generateDiscoveryTopic(const String& deviceIdentifier,
                                  const String& uniqueId,
                                  const String& component) {
        return config.mqtt.autoDiscoveryPrefix + "/" + component + "/" + deviceIdentifier + "/" +
               uniqueId + "/config";
    }

    String generateStateTopic(const String& deviceTopicPrefix, const String& uniqueId) {
        return deviceTopicPrefix + "/" + uniqueId;
    }

    String generateCommandTopic(const String& deviceTopicPrefix, const String& uniqueId) {
        return deviceTopicPrefix + "/" + uniqueId + "/set";
    }

}  // namespace MqttDiscovery