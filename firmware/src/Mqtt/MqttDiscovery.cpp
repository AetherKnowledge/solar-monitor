#include "MqttDiscovery.h"
#include "MqttManager.h"
#include <Mqtt/MqttTypes.h>
#include <Config/Config.h>

namespace MqttDiscovery
{
    void start()
    {
        MqttDiscoveryConfig discoveryConfig;
        discoveryConfig.name = "Solar Monitor Uptime";
        discoveryConfig.uniqueId = "solar-monitor-uptime";
        discoveryConfig.unitOfMeasurement = "s";
        discoveryConfig.stateClass = "measurement";

        JsonDocument doc;
        JsonObject json = doc.to<JsonObject>();
        discoveryConfig.toJson(json);
        json["state_topic"] = "solar-monitor/uptime";
        JsonObject device = json["device"].to<JsonObject>();
        device["name"] = "Solar Monitor";
        device["manufacturer"] = "Solar Monitor";
        JsonArray identifiers = device["identifiers"].to<JsonArray>();
        identifiers.add("solar-monitor");

        MqttManager::publish(
            "homeassistant/sensor/solar-monitor/uptime/config",
            doc);

        for (const auto &device : config.modbusDevices)
        {
            auto publishedDiscoveryCount = 0;

            for (const auto &readRegister : device.readRegisters)
            {
                if (publishReadRegisterDiscovery(device, readRegister))
                {
                    publishedDiscoveryCount++;
                }
            }

            Serial.printf(
                "Published %d discovery messages for Modbus device %s (%s)\n",
                publishedDiscoveryCount,
                device.name.c_str(),
                device.identifier.c_str());
        }
    }

    bool publishReadRegisterDiscovery(const ModbusDevice &device, const ReadRegister &readRegister)
    {
        JsonDocument doc;
        JsonObject json = doc.to<JsonObject>();
        readRegister.discoveryConfig.toJson(json);

        JsonObject deviceJson = json["device"].to<JsonObject>();
        device.discoveryDevice.toJson(deviceJson);

        json["state_topic"] = getStateTopic(
            device.identifier,
            readRegister.discoveryConfig.uniqueId);

        return MqttManager::publish(
            getDiscoveryTopic(
                device.identifier,
                readRegister.discoveryConfig.uniqueId),
            doc,
            true);
    }

    String getDiscoveryTopic(const String &deviceIdentifier, const String &uniqueId)
    {
        return config.mqtt.autoDiscoveryPrefix + "/sensor/" + deviceIdentifier + "/" + uniqueId + "/config";
    }

    String getStateTopic(const String &deviceTopicPrefix, const String &uniqueId)
    {
        return deviceTopicPrefix + "/" + uniqueId;
    }
}