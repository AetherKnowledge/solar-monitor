#pragma once

#include "ModbusTypes.h"

namespace ReadRegisterManager {
    constexpr uint16_t MAX_GAP = 2;
    constexpr uint16_t MAX_REGISTERS_PER_REQUEST = 125;

    struct Result {
        bool success = false;
        std::vector<ReadRegister*> changedRegisters;
    };

    void setup(std::vector<ModbusDevice>& devices);
    Result readGroup(ModbusDevice& device, ReadGroup& group);

    bool processRegister(ModbusDevice& device, ReadRegister& reg, uint16_t rawValue);
    double transformValue(const ReadRegister& reg, double value);

    void createGroups(ModbusDevice& device);
    void createNewGroup(ModbusDevice& device, ReadRegister& reg);
}  // namespace ReadRegisterManager