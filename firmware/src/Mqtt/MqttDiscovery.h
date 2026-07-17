#pragma once

namespace MqttDiscovery
{
    void start();
    void publishReadRegisterDiscovery(const ModbusDevice &device, const ReadRegister &readRegister);
    String getDiscoveryTopic(const String &deviceIdentifier, const String &uniqueId);
    String getStateTopic(const String &deviceTopicPrefix, const String &uniqueId);
}