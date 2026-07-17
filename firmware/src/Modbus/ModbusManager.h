#pragma once

#include "ModbusTypes.h"
#include <set>

namespace ModbusManager
{
    extern bool hasStarted;
    extern std::set<int> portsInUse;

    enum class ReadResult
    {
        Error,
        Unchanged,
        Changed
    };

    void setup();
    void loop();
    void pollDevice(ModbusDevice &device);
    void setupDevice(ModbusDevice &device);
    ReadResult readRegister(ModbusDevice &device, ReadRegister &reg);
    float transformRegisterValue(const ReadRegister &reg, float value);
    void reset();

}