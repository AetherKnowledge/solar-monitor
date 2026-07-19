#pragma once

#include <Modbus/ModbusTypes.h>
#include <concepts>

namespace MqttDiscovery {
    void start();

    template <std::derived_from<SensorDiscovery> TDiscovery>
    bool publishDiscovery(const ModbusDevice& device, const TDiscovery& discovery);

    String getDiscoveryTopic(const String& deviceIdentifier, const String& uniqueId);
    String getStateTopic(const String& deviceTopicPrefix, const String& uniqueId);
}  // namespace MqttDiscovery