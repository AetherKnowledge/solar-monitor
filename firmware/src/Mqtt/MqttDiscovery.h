#pragma once

#include <Modbus/ModbusTypes.h>

namespace MqttDiscovery {
    void publishDevice(const Device& device);

    bool publishDiscovery(const DeviceDiscovery& deviceDiscovery, const Discovery& discovery);

    String generateDiscoveryTopic(const DeviceDiscovery& deviceDiscovery,
                                  const Discovery& discovery);
}  // namespace MqttDiscovery