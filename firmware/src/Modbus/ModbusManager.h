#pragma once

#include "ModbusTypes.h"
#include <set>
#include <vector>

namespace ModbusManager
{
    extern bool hasStarted;
    extern std::set<int> portsInUse;

    enum class ReadResultSingle
    {
        Error,
        Unchanged,
        Changed
    };

    struct ReadResultGrouped
    {
        bool success = false;
        std::vector<ReadRegister *> changedRegisters;
    };

    void setup();
    void loop();
    void pollDevice(ModbusDevice &device);
    void setupDevice(ModbusDevice &device);

    ReadResultSingle readSingleRegister(ModbusDevice &device, ReadRegister &reg);
    ReadResultGrouped readGroupedRegisters(ModbusDevice &device, ReadGroup &group);

    bool processRegister(ModbusDevice &device, ReadRegister &reg, uint16_t rawValue);
    float transformRegisterValue(const ReadRegister &reg, float value);
    float applyRounding(float value, uint8_t decimals);

    void createReadGroups(ModbusDevice &device);
    void createNewGroup(ModbusDevice &device, ReadRegister &reg);
    void reset();

}