#pragma once

#include <Modbus/ModbusTypes.h>

namespace MqttDiscovery
{
    void start();
    bool publishReadRegisterDiscovery(const ModbusDevice &device, const ReadRegister &readRegister);
    String getDiscoveryTopic(const String &deviceIdentifier, const String &uniqueId);
    String getStateTopic(const String &deviceTopicPrefix, const String &uniqueId);
}