#pragma once

#include <Modbus/ModbusTypes.h>
#include <concepts>

namespace MqttDiscovery {
    void start();

    template <std::derived_from<SensorDiscovery> TDiscovery>
    bool publishDiscovery(const ModbusDevice& device, const TDiscovery& discovery);

    String generateDiscoveryTopic(const String& deviceIdentifier,
                                  const String& uniqueId,
                                  const String& component = "sensor");

    String generateStateTopic(const String& deviceTopicPrefix, const String& uniqueId);
    String generateCommandTopic(const String& deviceTopicPrefix, const String& uniqueId);
}  // namespace MqttDiscovery