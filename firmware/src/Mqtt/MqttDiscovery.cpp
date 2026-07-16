#include "MqttDiscovery.h"
#include "MqttManager.h"
#include <Mqtt/MqttTypes.h>

namespace MqttDiscovery
{
    void setup()
    {
        MqttDiscoveryConfig discoveryConfig;
        discoveryConfig.name = "Solar Monitor Uptime";
        discoveryConfig.uniqueId = "solar-monitor-uptime";
        discoveryConfig.stateTopic = "solar-monitor/uptime";
        discoveryConfig.unitOfMeasurement = "s";
        discoveryConfig.stateClass = "measurement";
        discoveryConfig.name = "Solar Monitor";

        JsonDocument doc;
        JsonObject json = doc.to<JsonObject>();
        discoveryConfig.toJson(json);

        bool ok = MqttManager::publish(
            "homeassistant/sensor/solar-monitor/uptime/config",
            doc);

        Serial.printf("Discovery publish: %s\n", ok ? "OK" : "FAILED");
    }
}