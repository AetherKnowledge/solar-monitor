#pragma once

#include "ModbusTypes.h"

namespace WriteRegisterManager {

    template <WriteRegisterType T>
    bool writeRegister(ModbusDevice& device, T& reg, double value) {
        uint16_t transformedValue = device.swapBytes
                                        ? __builtin_bswap16(static_cast<uint16_t>(value))
                                        : static_cast<uint16_t>(value);

        uint8_t result = device.modbus.writeSingleRegister(reg.address, transformedValue);

        if (result == ModbusMaster::ku8MBSuccess) {
            reg.value = value;

            Serial.printf("Wrote value %f to register %u of device %s (%s)\n",
                          value,
                          reg.address,
                          device.name.c_str(),
                          device.identifier.c_str());

            return true;
        }

        return false;
    }

}  // namespace WriteRegisterManager