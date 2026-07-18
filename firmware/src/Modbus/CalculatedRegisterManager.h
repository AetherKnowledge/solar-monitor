#pragma once

#include <Arduino.h>
#include "ModbusTypes.h"

namespace CalculatedRegisterManager
{
    void setupDevice(ModbusDevice &device);
    bool updateRegister(CalculatedRegister &calculatedRegister);
    void resetDevice(ModbusDevice &device);
    void compileExpressions(ModbusDevice &device);
}