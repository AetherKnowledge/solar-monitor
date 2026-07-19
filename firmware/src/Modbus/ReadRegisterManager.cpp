#include "ReadRegisterManager.h"

namespace ReadRegisterManager {
    void setup(std::vector<ModbusDevice>& devices) {
        for (auto& device : devices) {
            ReadRegisterManager::createGroups(device);
        }
    }

    Result readGroup(ModbusDevice& device, ReadGroup& group) {
        auto result = device.modbus.readHoldingRegisters(group.startAddress, group.count);

        if (result != device.modbus.ku8MBSuccess) {
            // Serial.printf("Modbus error: %02X\n", result);
            return Result{false, {}};
        }

        std::vector<ReadRegister*> changedRegisters;
        for (ReadRegister* reg : group.registers) {
            uint16_t offset = reg->address - group.startAddress;

            bool changed = processRegister(device, *reg, device.modbus.getResponseBuffer(offset));

            if (changed) {
                changedRegisters.push_back(reg);
            }
        }
        return Result{true, changedRegisters};
    }

    bool processRegister(ModbusDevice& device, ReadRegister& reg, uint16_t raw) {
        if (device.swapBytes) {
            raw = __builtin_bswap16(raw);
        }

        double value = reg.signedValue ? static_cast<double>(static_cast<int16_t>(raw))
                                       : static_cast<double>(raw);

        value = transformValue(reg, value);

        if (reg.rounding > 0) {
            value = applyRounding(value, reg.rounding);
        }

        if (value != reg.value) {
            reg.value = value;
            return true;
        }
        return false;
    }

    double transformValue(const ReadRegister& reg, double value) {
        switch (reg.transform) {
            case RegisterTransform::None:
                return value;
            case RegisterTransform::Multiply:
                return value * reg.transformArgument;
            case RegisterTransform::Divide:
                return value / reg.transformArgument;
            default:
                return value;
        }
    }

    double applyRounding(double value, uint8_t decimals) {
        static constexpr double factors[] = {1.0, 10.0, 100.0, 1000.0};

        if (decimals >= std::size(factors))
            return value;

        return roundf(value * factors[decimals]) / factors[decimals];
    }

    void createGroups(ModbusDevice& device) {
        device.readGroups.clear();

        std::sort(
            device.readRegisters.begin(),
            device.readRegisters.end(),
            [](const ReadRegister& a, const ReadRegister& b) { return a.address < b.address; });

        ReadRegister* previousRegister = nullptr;
        for (auto& reg : device.readRegisters) {
            if (device.readGroups.empty()) {
                createNewGroup(device, reg);
                previousRegister = &reg;
                continue;
            }

            if (previousRegister && reg.address == previousRegister->address) {
                Serial.printf(
                    "Duplicate register %u found in device %s. Only the first "
                    "occurrence will be used.\n",
                    reg.address,
                    device.name.c_str());
                continue;
            }

            ReadGroup& lastGroup = device.readGroups.back();

            uint16_t newCount = reg.address - lastGroup.startAddress + 1;
            if (reg.address <= lastGroup.startAddress + lastGroup.count + MAX_GAP &&
                newCount < MAX_REGISTERS_PER_REQUEST) {
                lastGroup.registers.push_back(&reg);
                lastGroup.count = newCount;
            } else {
                Serial.printf("Read group for device %s: %u-%u (%u registers)\n",
                              device.name.c_str(),
                              lastGroup.startAddress,
                              lastGroup.startAddress + lastGroup.count - 1,
                              lastGroup.count);

                createNewGroup(device, reg);
            }

            previousRegister = &reg;
        }
    }

    void createNewGroup(ModbusDevice& device, ReadRegister& reg) {
        device.readGroups.push_back({.startAddress = reg.address, .count = 1, .registers = {&reg}});
    }
}  // namespace ReadRegisterManager