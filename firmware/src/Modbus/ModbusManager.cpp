#include "ModbusManager.h"
#include <Config/Config.h>
#include <Mqtt/MqttManager.h>
#include <Mqtt/MqttDiscovery.h>

namespace ModbusManager
{
    bool hasStarted = false;
    std::set<int> portsInUse;

    void setup()
    {
        reset();

        Serial.println();
        Serial.println("Setting up Modbus Manager");

        for (auto &device : config.modbusDevices)
        {
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

        for (auto &reg : device.readRegisters)
        {
            ReadResult result = readRegister(device, reg);

            if (result == ReadResult::Error)
            {
                Serial.printf(
                    "Failed to read register %u\n",
                    reg.address);
                continue;
            }

            if (result == ReadResult::Changed && device.mqttEnabled)
            {
                MqttManager::publish(
                    MqttDiscovery::getStateTopic(device.identifier, reg.discoveryConfig.uniqueId),
                    String(reg.value));
            }
        }
    }

    ReadResult readRegister(ModbusDevice &device, ReadRegister &reg)
    {
        auto result =
            device.modbus.readHoldingRegisters(reg.address, 1);

        if (result != device.modbus.ku8MBSuccess)
        {
            Serial.printf("Modbus error: %02X\n", result);
            return ReadResult::Error;
        }

        uint16_t raw = device.modbus.getResponseBuffer(0);

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
            float factor = pow(10, reg.rounding);
            value = round(value * factor) / factor;
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
            return ReadResult::Changed;
        }
        return ReadResult::Unchanged;
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
}