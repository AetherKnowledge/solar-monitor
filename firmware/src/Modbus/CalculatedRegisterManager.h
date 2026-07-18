#pragma once

#include <Arduino.h>
#include "ModbusTypes.h"
#include <unordered_set>
#include <vector>
#include <Common/File.h>

namespace CalculatedRegisterManager
{
    using unordered_set_t = std::unordered_set<std::string>;

    inline constexpr char PERSISTENCE_LOCATION[] = "/persistence.json";
    inline constexpr char PERSISTENCE_BAK_LOCATION[] = "/persistence.json.bak";

    void setup(std::vector<ModbusDevice> &devices);
    void setupDevice(ModbusDevice &device);
    bool updateRegister(CalculatedRegister &calculatedRegister);
    void addVariable(ModbusDevice &device, Register &reg, unordered_set_t &usedIds);
    void resetDevice(ModbusDevice &device);
    void compileExpressions(ModbusDevice &device);

    bool loadFile(File &file, std::vector<ModbusDevice> &devices);
    bool loadPersistence(std::vector<ModbusDevice> &devices);
    bool savePersistence(std::vector<ModbusDevice> &devices);
    void loopPersistence();

    void clearDirty(std::vector<ModbusDevice> &devices);
    bool findDirty(const std::vector<ModbusDevice> &devices);
}