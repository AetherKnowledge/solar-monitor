#pragma once

#include "ModbusTypes.h"
#include <set>

namespace ModbusManager
{
    extern bool hasStarted;
    extern std::set<int> portsInUse;

    void setup();
    void loop();
    void pollDevice(ModbusDevice &device);
    void setupDevice(ModbusDevice &device);
    bool readRegister(ModbusDevice &device, ReadRegister &reg);
    float transformRegisterValue(const ReadRegister &reg, float value);
    void reset();
}