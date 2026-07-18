#include "ModbusManager.h"
#include <Config/Config.h>
#include <Mqtt/MqttManager.h>
#include <Mqtt/MqttDiscovery.h>
#include "ReadRegisterManager.h"
#include "CalculatedRegisterManager.h"

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
            ReadRegisterManager::createGroups(device);
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

        CalculatedRegisterManager::setupDevice(device);

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
            CalculatedRegisterManager::resetDevice(device);
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
        // Serial.println("Polling Modbus device " + device.name + " (" + device.identifier + ")");

        for (auto &group : device.readGroups)
        {
            processReadRegisters(device, group);
            updateCalculatedRegisters(device);
        }
    }

    void processReadRegisters(ModbusDevice &device, ReadGroup &group)
    {
        ReadRegisterManager::Result result = ReadRegisterManager::readGroup(device, group);

        if (!result.success)
        {
            Serial.printf(
                "Failed to read register group %u-%u\n",
                group.startAddress,
                group.startAddress + group.count - 1);
            return;
        }

        if (!result.changedRegisters.empty() && device.mqttEnabled && MqttManager::isConnected())
        {
            for (const auto &reg : result.changedRegisters)
            {
                Serial.printf(
                    "Register %s (address: %d) value changed to %.2f\n",
                    reg->discoveryConfig.name.c_str(),
                    reg->address,
                    reg->value);

                MqttManager::publish(
                    MqttDiscovery::getStateTopic(device.identifier, reg->discoveryConfig.uniqueId),
                    String(reg->value));
            }
        }
    }

    void updateCalculatedRegisters(ModbusDevice &device)
    {
        for (auto &calculatedRegister : device.calculatedRegisters)
        {
            bool result = CalculatedRegisterManager::updateRegister(calculatedRegister);

            if (result && device.mqttEnabled && MqttManager::isConnected())
            {
                Serial.printf(
                    "Calculated Register %s value changed to %.2f\n",
                    calculatedRegister.discoveryConfig.name.c_str(),
                    calculatedRegister.value);

                MqttManager::publish(
                    MqttDiscovery::getStateTopic(device.identifier, calculatedRegister.discoveryConfig.uniqueId),
                    String(calculatedRegister.value));
            }
        }
    }

}