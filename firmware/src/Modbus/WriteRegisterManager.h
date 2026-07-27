#pragma once

#include "ModbusTypes.h"
#include <Common/Logger.h>

namespace WriteRegisterManager {

    bool writeRegister(ModbusDevice& device, WriteRegister& reg, double value);

}  // namespace WriteRegisterManager
