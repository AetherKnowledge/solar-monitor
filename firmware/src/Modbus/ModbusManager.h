#pragma once

#include "ModbusTypes.h"
#include <set>
#include <vector>

namespace ModbusManager {
extern bool hasStarted;
extern std::set<int> portsInUse;

void setup();
void loop();
void pollDevice(ModbusDevice& device);
void setupDevices(std::vector<ModbusDevice>& devices);

void processReadRegisters(ModbusDevice& device, ReadGroup& group);
void updateVirtualSensors(ModbusDevice& device);

void reset();
}  // namespace ModbusManager