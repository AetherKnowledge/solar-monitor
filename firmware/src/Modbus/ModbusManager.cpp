#include "ModbusManager.h"
#include "ReadRegisterManager.h"
#include "VirtualSensorManager.h"
#include <Config/ConfigManager.h>
#include <Mqtt/MqttDiscovery.h>
#include <Mqtt/MqttManager.h>
#include <Common/Logger.h>

namespace ModbusManager {
    bool hasStarted = false;
    std::set<int> portsInUse;

    volatile UpdateStatus updateStatus = UpdateStatus::NotStarted;
    std::vector<ModbusDevice> pendingDevices;

    void setup() {
        reset();

        Log.println();
        Log.println("Setting up Modbus Manager");

        ReadRegisterManager::setup(ConfigManager::config.modbusDevices);
        VirtualSensorManager::setup(ConfigManager::config.modbusDevices);
        setupDevices(ConfigManager::config.modbusDevices);

        hasStarted = true;
    }

    void setupDevices(std::vector<ModbusDevice>& devices) {
        for (auto& device : devices) {
            if (device.port != 1 && device.port != 2) {
                device.initialized = false;

                Log.printf("Invalid port number %d for Modbus device %s\n",
                           device.port,
                           device.discovery.name.c_str());
                return;
            }

            const auto [_, inserted] = portsInUse.insert(device.port);

            if (!inserted) {
                device.initialized = false;

                Log.printf("Port %d already in use. Skipping %s\n",
                           device.port,
                           device.discovery.name.c_str());
                return;
            }

            switch (device.port) {
                case 1:
                    Serial1.end();  // Ensure Serial1 is not already in use
                    Serial1.begin(device.baudrate, SERIAL_8N1, 16, 17);

                    device.modbus.begin(device.slaveId, Serial1);

                    break;
                case 2:
                    Serial2.end();  // Ensure Serial2 is not already in use
                    Serial2.begin(device.baudrate, SERIAL_8N1, 18, 19);

                    device.modbus.begin(device.slaveId, Serial2);
                    break;
            }

            device.initialized = true;
            Log.printf("Initialized Modbus device %s on port %d with baudrate %d\n",
                       device.discovery.name.c_str(),
                       device.port,
                       device.baudrate);
        }
    }

    void reset() {
        Log.println("Resetting Modbus Manager");

        hasStarted = false;
        portsInUse.clear();

        Serial1.end();
        Serial2.end();

        for (auto& device : ConfigManager::config.modbusDevices) {
            device.initialized = false;
            device.readGroups.clear();
            VirtualSensorManager::resetDevice(device);
        }
    }

    void requestUpdate(const std::vector<ModbusDevice>& devices) {
        pendingDevices = devices;
        updateStatus = UpdateStatus::Requested;

        Log.println("\nModbus configuration updating");
    }

    void updateConfig(const std::vector<ModbusDevice>& devices) {
        updateStatus = UpdateStatus::InProgress;

        ConfigManager::config.modbusDevices = std::move(pendingDevices);
        ConfigManager::save();
        MqttManager::reload();
        ModbusManager::setup();

        updateStatus = UpdateStatus::UpdateComplete;
        Log.print("Modbus configuration updated. New config: ");
        Log.println(ConfigManager::config.toString().c_str());
        return;
    }

    void loop() {
        if (updateStatus == UpdateStatus::Requested) {
            updateConfig(pendingDevices);
            return;
        }

        if (updateStatus == UpdateStatus::InProgress) {
            return;
        }

        static uint32_t lastPoll = 0;

        if (millis() - lastPoll < 1000)
            return;

        lastPoll = millis();

        if (!hasStarted)
            return;

        for (auto& device : ConfigManager::config.modbusDevices) {
            if (!device.initialized)
                continue;

            pollDevice(device);
        }

        VirtualSensorManager::loopPersistence();
    }

    void pollDevice(ModbusDevice& device) {
        // Log.println("Polling Modbus device " + device.name + " (" +
        // device.identifier + ")");

        for (auto& group : device.readGroups) {
            processReadRegisters(device, group);
            updateVirtualSensors(device);
        }
    }

    void processReadRegisters(ModbusDevice& device, ReadGroup& group) {
        ReadRegisterManager::Result result = ReadRegisterManager::readGroup(device, group);

        if (!result.success) {
            // Log.printf("Failed to read register group %u-%u\n",
            //               group.startAddress,
            //               group.startAddress + group.count - 1);
            return;
        }

        if (!result.changedRegisters.empty() && device.mqttEnabled && MqttManager::isConnected()) {
            for (const auto& reg : result.changedRegisters) {
                // Log.printf("Register %s (address: %d) value changed to %.2f\n",
                //               reg->discovery.name.c_str(),
                //               reg->address,
                //               reg->value);

                MqttManager::publish(reg->discovery.stateTopic, String(reg->value), true);
            }
        }
    }

    void updateVirtualSensors(ModbusDevice& device) {
        for (auto& virtualSensor : device.virtualSensors) {
            bool result = VirtualSensorManager::updateRegister(virtualSensor);

            if (result && device.mqttEnabled && MqttManager::isConnected()) {
                // Log.printf("Virtual Sensor %s value changed to %.2f\n",
                //               virtualSensor.discovery.name.c_str(),
                //               virtualSensor.value);

                MqttManager::publish(
                    virtualSensor.discovery.stateTopic, String(virtualSensor.value), true);
            }
        }
    }

    template <typename T>
    static void addValues(JsonObject& obj, const std::vector<T>& values) {
        for (const auto& value : values) {
            obj[value.discovery.uniqueId] = value.value;
        }
    }

    void getValues(JsonDocument& doc) {
        for (const auto& device : ConfigManager::config.modbusDevices) {
            JsonObject deviceJson = doc[device.discovery.identifier].to<JsonObject>();

            JsonObject read = deviceJson["readRegisters"].to<JsonObject>();
            addValues(read, device.readRegisters);

            JsonObject virtuals = deviceJson["virtualSensors"].to<JsonObject>();
            addValues(virtuals, device.virtualSensors);

            JsonObject selects = deviceJson["selectWriteRegisters"].to<JsonObject>();
            addValues(selects, device.selectWriteRegisters);

            JsonObject numbers = deviceJson["numberWriteRegisters"].to<JsonObject>();
            addValues(numbers, device.numberWriteRegisters);
        }
    }

}  // namespace ModbusManager