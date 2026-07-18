#pragma once

#include <Arduino.h>
#include "ModbusTypes.h"
#include <unordered_set>

namespace CalculatedRegisterManager
{
    using unordered_set_t = std::unordered_set<std::string>;

    void setupDevice(ModbusDevice &device);
    bool updateRegister(CalculatedRegister &calculatedRegister);
    void addVariable(ModbusDevice &device, Register &reg, unordered_set_t &usedIds);
    void resetDevice(ModbusDevice &device);
    void compileExpressions(ModbusDevice &device);
}