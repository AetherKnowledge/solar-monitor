#include "WriteRegisterManager.h"

namespace WriteRegisterManager {
    bool writeRegister(ModbusDevice& device, WriteRegister& reg, double value) {
        uint16_t transformedValue = device.swapBytes
                                        ? __builtin_bswap16(static_cast<uint16_t>(value))
                                        : static_cast<uint16_t>(value);

        uint8_t result = device.modbus.writeSingleRegister(reg.address, transformedValue);

        if (result == ModbusMaster::ku8MBSuccess) {
            reg.value = value;

            Log.printf("Wrote value %f to register %u of device %s (%s)\n",
                       value,
                       reg.address,
                       device.discovery.name.c_str(),
                       device.discovery.identifier.c_str());

            return true;
        }

        Log.printf("Failed to write value %f to register %u of device %s (%s). Error: %u\n",
                   value,
                   reg.address,
                   device.discovery.name.c_str(),
                   device.discovery.identifier.c_str(),
                   result);

        return false;
    }

}  // namespace WriteRegisterManager

bool WriteRegister::execute(Device& device, const String& payload) {
    auto& modbusDevice = static_cast<ModbusDevice&>(device);

    return WriteRegisterManager::writeRegister(modbusDevice, *this, payload.toDouble());
}