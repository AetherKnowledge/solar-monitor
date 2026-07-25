#pragma once

#include <Modbus/ModbusTypes.h>
#include <concepts>

namespace MqttDiscovery {
    void start();

    bool publishDiscovery(const ModbusDevice& device, const Discovery& discovery);

    String generateDiscoveryTopic(const ModbusDevice& device, const Discovery& discovery);

    String generateStateTopic(const ModbusDevice& device, const Discovery& discovery);
    String generateCommandTopic(const ModbusDevice& device, const WriteDiscovery& discovery);
}  // namespace MqttDiscovery