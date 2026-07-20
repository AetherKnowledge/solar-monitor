#pragma once

#include "ModbusTypes.h"
#include <set>
#include <vector>
#include <Common/UpdateStatus.h>

namespace ModbusManager {
    extern bool hasStarted;
    extern std::set<int> portsInUse;

    extern volatile UpdateStatus updateStatus;
    extern std::vector<ModbusDevice> pendingDevices;

    void setup();
    void loop();
    void pollDevice(ModbusDevice& device);
    void reload();
    void requestUpdate(const std::vector<ModbusDevice>& devices);
    void updateConfig(const std::vector<ModbusDevice>& devices);
    void setupDevices(std::vector<ModbusDevice>& devices);

    void processReadRegisters(ModbusDevice& device, ReadGroup& group);
    void updateVirtualSensors(ModbusDevice& device);

    void reset();
}  // namespace ModbusManager