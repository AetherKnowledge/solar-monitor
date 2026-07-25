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
    extern double pollDeltaSeconds;

    extern DisplayData currentDisplayData;

    void setup();
    void loop();
    void pollDevice(ModbusDevice& device);
    void publishDisplayData();
    void reload();
    void requestUpdate(const std::vector<ModbusDevice>& devices);
    void updateConfig(const std::vector<ModbusDevice>& devices);
    void setupDevices(std::vector<ModbusDevice>& devices);

    bool processReadRegisters(ModbusDevice& device, ReadGroup& group);
    void updateVirtualSensors(ModbusDevice& device);
    void getValues(JsonDocument& doc);

    void reset();
}  // namespace ModbusManager