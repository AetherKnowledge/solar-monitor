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

    json["state_topic"] = getStateTopic(device.identifier, discovery.uniqueId);

    return MqttManager::publish(
        getDiscoveryTopic(device.identifier, discovery.uniqueId), doc, true);
}

String getDiscoveryTopic(const String& deviceIdentifier, const String& uniqueId) {
    return config.mqtt.autoDiscoveryPrefix + "/sensor/" + deviceIdentifier + "/" + uniqueId +
           "/config";
}

String getStateTopic(const String& deviceTopicPrefix, const String& uniqueId) {
    return deviceTopicPrefix + "/" + uniqueId;
}
}  // namespace MqttDiscovery