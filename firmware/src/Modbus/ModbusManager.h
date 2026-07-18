#pragma once

#include "ModbusTypes.h"
#include <set>
#include <vector>

namespace ModbusManager
{
    extern bool hasStarted;
    extern std::set<int> portsInUse;

    void setup();
    void loop();
    void pollDevice(ModbusDevice &device);
    void setupDevice(ModbusDevice &device);

    void processReadRegisters(ModbusDevice &device, ReadGroup &group);
    void updateCalculatedRegisters(ModbusDevice &device);

    void reset();
}