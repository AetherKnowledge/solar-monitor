#include "ModbusManager.h"
#include <Config/Config.h>
#include <Mqtt/MqttManager.h>
#include <Mqtt/MqttDiscovery.h>

namespace ModbusManager
{
    bool hasStarted = false;
    std::set<int> portsInUse;
    constexpr uint16_t MAX_GAP = 2;

    void setup()
    {
        reset();

        Serial.println();
        Serial.println("Setting up Modbus Manager");

        for (auto &device : config.modbusDevices)
        {
            createReadGroups(device);
            for (const auto &group : device.readGroups)
            {
                Serial.printf(
                    "Read group for device %s: %u-%u (%u registers)\n",
                    device.name.c_str(),
                    group.startAddress,
                    group.startAddress + group.count - 1,
                    group.count);
            }

            setupDevice(device);
        }

        hasStarted = true;
    }

    void setupDevice(ModbusDevice &device)
    {
        if (device.port != 1 && device.port != 2)
        {
            device.initialized = false;

            Serial.printf(
                "Invalid port number %d for Modbus device %s\n",
                device.port,
                device.name.c_str());
            return;
        }

        const auto [_, inserted] = portsInUse.insert(device.port);

        if (!inserted)
        {
            device.initialized = false;

            Serial.printf(
                "Port %d already in use. Skipping %s\n",
                device.port,
                device.name.c_str());
            return;
        }

        switch (device.port)
        {
        case 1:
            Serial1.end(); // Ensure Serial1 is not already in use
            Serial1.begin(device.baudrate, SERIAL_8N1, 16, 17);

            device.modbus.begin(device.slaveId, Serial1);

            break;
        case 2:
            Serial2.end(); // Ensure Serial2 is not already in use
            Serial2.begin(device.baudrate, SERIAL_8N1, 18, 19);

            device.modbus.begin(device.slaveId, Serial2);
            break;
        }

        device.initialized = true;
        Serial.printf(
            "Initialized Modbus device %s on port %d with baudrate %d\n",
            device.name.c_str(),
            device.port,
            device.baudrate);
    }

    void reset()
    {
        Serial.println("Resetting Modbus Manager");

        hasStarted = false;
        portsInUse.clear();

        Serial1.end();
        Serial2.end();

        for (auto &device : config.modbusDevices)
        {
            device.initialized = false;
            device.readGroups.clear();
        }
    }

    void loop()
    {
        static uint32_t lastPoll = 0;

        if (millis() - lastPoll < 1000)
            return;

        lastPoll = millis();

        if (!hasStarted)
            return;

        for (auto &device : config.modbusDevices)
        {
            if (!device.initialized)
                continue;

            pollDevice(device);
        }
    }

    void pollDevice(ModbusDevice &device)
    {
        Serial.println("Polling Modbus device " + device.name + " (" + device.identifier + ")");

        for (auto &group : device.readGroups)
        {
            ReadResultGrouped result = readGroupedRegisters(device, group);

            if (!result.success)
            {
                Serial.printf(
                    "Failed to read register group %u-%u\n",
                    group.startAddress,
                    group.startAddress + group.count - 1);
                continue;
            }

            if (!result.changedRegisters.empty() && device.mqttEnabled)
            {
                for (const auto &reg : result.changedRegisters)
                {
                    MqttManager::publish(
                        MqttDiscovery::getStateTopic(device.identifier, reg->discoveryConfig.uniqueId),
                        String(reg->value));
                }
            }
        }
    }

    ReadResultSingle readSingleRegister(ModbusDevice &device, ReadRegister &reg)
    {
        auto result =
            device.modbus.readHoldingRegisters(reg.address, 1);

        if (result != device.modbus.ku8MBSuccess)
        {
            Serial.printf("Modbus error: %02X\n", result);
            return ReadResultSingle::Error;
        }

        bool changed = processRegister(device, reg, device.modbus.getResponseBuffer(0));
        return changed ? ReadResultSingle::Changed : ReadResultSingle::Unchanged;
    }

    ReadResultGrouped readGroupedRegisters(ModbusDevice &device, ReadGroup &group)
    {
        auto result =
            device.modbus.readHoldingRegisters(group.startAddress, group.count);

        if (result != device.modbus.ku8MBSuccess)
        {
            Serial.printf("Modbus error: %02X\n", result);
            return ReadResultGrouped{false, {}};
        }

        std::vector<ReadRegister *> changedRegisters;

        for (ReadRegister *reg : group.registers)
        {
            uint16_t offset = reg->address - group.startAddress;

            bool changed = processRegister(device, *reg, device.modbus.getResponseBuffer(offset));

            if (changed)
            {
                changedRegisters.push_back(reg);
            }
        }
        return ReadResultGrouped{true, changedRegisters};
    }

    bool processRegister(ModbusDevice &device, ReadRegister &reg, uint16_t raw)
    {
        if (device.swapBytes)
        {
            raw = __builtin_bswap16(raw);
        }

        float value = reg.signedValue
                          ? static_cast<float>(static_cast<int16_t>(raw))
                          : static_cast<float>(raw);

        value = transformRegisterValue(reg, value);

        if (reg.rounding > 0)
        {
            value = applyRounding(value, reg.rounding);
        }

        if (value != reg.value)
        {
            Serial.printf(
                "Register %s (address: %d) value changed from %.2f to %.2f\n",
                reg.discoveryConfig.name.c_str(),
                reg.address,
                reg.value,
                value);

            reg.value = value;
            return true;
        }
        return false;
    }

    float transformRegisterValue(const ReadRegister &reg, float value)
    {
        switch (reg.transform)
        {
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

    float applyRounding(float value, uint8_t decimals)
    {
        static constexpr float factors[] =
            {
                1.0f,
                10.0f,
                100.0f,
                1000.0f};

        if (decimals >= std::size(factors))
            return value;

        return roundf(value * factors[decimals]) / factors[decimals];
    }

    void createReadGroups(ModbusDevice &device)
    {
        device.readGroups.clear();

        std::sort(device.readRegisters.begin(), device.readRegisters.end(),
                  [](const ReadRegister &a, const ReadRegister &b)
                  {
                      return a.address < b.address;
                  });

        for (auto &reg : device.readRegisters)
        {
            if (device.readGroups.empty())
            {
                createNewGroup(device, reg);
                continue;
            }

            ReadGroup &lastGroup = device.readGroups.back();

            if (reg.address <= lastGroup.startAddress + lastGroup.count + MAX_GAP)
            {
                lastGroup.registers.push_back(&reg);
                lastGroup.count = reg.address - lastGroup.startAddress + 1;
            }
            else
            {
                createNewGroup(device, reg);
            }
        }
    }

    void createNewGroup(ModbusDevice &device, ReadRegister &reg)
    {
        device.readGroups.push_back({.startAddress = reg.address,
                                     .count = 1,
                                     .registers = {&reg}});
    }

}