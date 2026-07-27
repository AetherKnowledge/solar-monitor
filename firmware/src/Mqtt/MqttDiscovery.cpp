#include "MqttDiscovery.h"
#include "MqttManager.h"
#include <Config/ConfigManager.h>
#include <Mqtt/MqttTypes.h>
#include <Common/Logger.h>

namespace MqttDiscovery {
    void publishDevice(const Device& device) {
        auto publishedDiscoveryCount = 0;

        device.forEachEntity([&](const Entity& entity) {
            if (publishDiscovery(device.discovery, entity.getDiscovery())) {
                publishedDiscoveryCount++;
            }
        });

        Log.printf("Published %d discovery messages for Device %s (%s)\n",
                   publishedDiscoveryCount,
                   device.discovery.name.c_str(),
                   device.discovery.identifier.c_str());
    }

    bool publishDiscovery(const DeviceDiscovery& deviceDiscovery, const Discovery& discovery) {
        if (discovery.name.isEmpty() || discovery.uniqueId.isEmpty()) {
            Log.printf("Skipping discovery for device %s (%s) due to missing name or uniqueId\n",
                       deviceDiscovery.name.c_str(),
                       deviceDiscovery.identifier.c_str());
            return false;
        }

        JsonDocument doc;
        JsonObject json = doc.to<JsonObject>();

        discovery.toJson(json);

        JsonObject deviceJson = json["device"].to<JsonObject>();
        deviceDiscovery.toJson(deviceJson);

        JsonArray identifiers = deviceJson["identifiers"].to<JsonArray>();
        identifiers.add(deviceDiscovery.identifier);
        deviceJson.remove("identifier");

        bool result =
            MqttManager::publish(generateDiscoveryTopic(deviceDiscovery, discovery), doc, true);

        return result;
    }

    String generateDiscoveryTopic(const DeviceDiscovery& deviceDiscovery,
                                  const Discovery& discovery) {
        return ConfigManager::config.mqtt.autoDiscoveryPrefix + "/" + discovery.component() + "/" +
               deviceDiscovery.identifier + "/" + discovery.uniqueId + "/config";
    }

}  // namespace MqttDiscovery