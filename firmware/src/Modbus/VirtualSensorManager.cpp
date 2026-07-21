#include "VirtualSensorManager.h"
#include <Config/ConfigManager.h>
#include <tinyexpr.h>
#include <Common/Numbers.h>
#include <Common/Logger.h>

namespace VirtualSensorManager {
    bool loadFile(File& file, std::vector<ModbusDevice>& devices) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            Log.printf("Failed to deserialize persistence: %s\n", error.c_str());
            return false;
        }

        for (auto& device : devices) {
            JsonObject deviceJson = doc[device.discovery.identifier].as<JsonObject>();

            if (deviceJson.isNull())
                continue;

            for (auto& reg : device.virtualSensors) {
                if (!reg.isPersistent)
                    continue;

                reg.value = deviceJson[reg.getId()] | reg.value;
            }
        }

        return true;
    }

    bool loadPersistence(std::vector<ModbusDevice>& devices) {
        Log.println();
        Log.println("Loading persistence");

        if (!ConfigManager::mountFS()) {
            Log.println("Failed to mount ConfigFS");
            return false;
        }

        File persistence = ConfigManager::ConfigFS.open(PERSISTENCE_LOCATION, "r");
        File backup = ConfigManager::ConfigFS.open(PERSISTENCE_BAK_LOCATION, "r");

        if (persistence && loadFile(persistence, devices)) {
            backup.close();
            Log.println("Loaded default persistence successfully");
            return true;
        } else if (backup && loadFile(backup, devices)) {
            persistence.close();

            Log.println("Loaded backup persistence successfully");
            if (!copyFile(
                    ConfigManager::ConfigFS, PERSISTENCE_BAK_LOCATION, PERSISTENCE_LOCATION)) {
                Log.println("Failed to restore persistence from backup.");
            }
            return true;
        }

        backup.close();
        persistence.close();

        Log.println("Failed to load default and backup persistence!");
        return false;
    }

    bool savePersistence(std::vector<ModbusDevice>& devices) {
        Log.println();
        Log.println("Saving persistence");

        JsonDocument doc;

        for (auto& device : devices) {
            JsonObject deviceJson = doc[device.discovery.identifier].to<JsonObject>();

            for (auto& reg : device.virtualSensors) {
                if (reg.isPersistent)
                    deviceJson[reg.getId()] = reg.value;
            }
        }

        if (ConfigManager::ConfigFS.exists(PERSISTENCE_LOCATION) &&
            !copyFile(ConfigManager::ConfigFS, PERSISTENCE_LOCATION, PERSISTENCE_BAK_LOCATION)) {
            Log.println("Backing up persistence failed!");
        }

        File persistence = ConfigManager::ConfigFS.open(PERSISTENCE_LOCATION, "w");

        if (!persistence) {
            Log.println("Failed to open persistence for writing.");
            return false;
        }

        if (serializeJson(doc, persistence) == 0) {
            Log.println("Failed to write persistence.");
            persistence.close();
            return false;
        }

        persistence.close();
        clearDirty(devices);

        Log.println("Persistence saved");
        return true;
    }

    void clearDirty(std::vector<ModbusDevice>& devices) {
        for (auto& device : devices) {
            for (auto& reg : device.virtualSensors) {
                if (reg.isPersistent && reg.isDirty) {
                    reg.isDirty = false;
                }
            }
        }
    }

    bool findDirty(const std::vector<ModbusDevice>& devices) {
        for (const auto& device : devices) {
            for (const auto& reg : device.virtualSensors) {
                if (reg.isPersistent && reg.isDirty) {
                    return true;
                }
            }
        }

        return false;
    }

    void loopPersistence() {
        static uint32_t lastPoll = 0;

        // 60 seconds
        if (millis() - lastPoll < 1000 * 60 * 15)
            return;

        lastPoll = millis();

        if (findDirty(ConfigManager::config.modbusDevices)) {
            savePersistence(ConfigManager::config.modbusDevices);
        }
    }

    void setup(std::vector<ModbusDevice>& devices) {
        for (auto& device : devices) {
            setupDevice(device);
        }

        loadPersistence(devices);
    }

    void setupDevice(ModbusDevice& device) {
        Log.printf("Setting up virtual sensors for device %s (%s)\n",
                   device.discovery.name.c_str(),
                   device.discovery.identifier.c_str());

        resetDevice(device);

        device.vars.clear();
        device.vars.reserve(device.readRegisters.size() + device.virtualSensors.size());

        unordered_set_t usedIds;
        for (auto& readRegister : device.readRegisters) {
            addVariable(device, readRegister, usedIds);
        }

        // virtual sensors are evaluated in order
        for (auto& virtualSensor : device.virtualSensors) {
            addVariable(device, virtualSensor, usedIds);
        }

        compileExpressions(device);
    }

    template <std::derived_from<Entity<SensorDiscovery>> T>
    void addVariable(ModbusDevice& device, T& reg, unordered_set_t& usedIds) {
        using RegisterType = std::decay_t<T>;

        static_assert(std::is_same_v<RegisterType, ReadRegister> ||
                          std::is_same_v<RegisterType, VirtualSensor>,
                      "Only ReadRegister and VirtualSensor are supported");

        if (reg.getId().isEmpty()) {
            Log.printf("Register '%s' has no unique ID.\n", reg.getName().c_str());
            return;
        }

        auto [_, inserted] = usedIds.insert(reg.getId().c_str());

        if (!inserted) {
            Log.printf("Duplicate unique ID '%s' found for register '%s'.\n",
                       reg.getId().c_str(),
                       reg.getName().c_str());
            return;
        }

        te_variable var;
        var.name = reg.getId().c_str();
        var.address = &reg.value;
        var.type = TE_VARIABLE;
        var.context = nullptr;

        device.vars.push_back(var);

        Log.printf("Added variable for virtual sensors: %s (name: %s)\n",
                   reg.getId().c_str(),
                   reg.getName().c_str());
    }

    void compileExpressions(ModbusDevice& device) {
        Log.printf("Compiling expressions for device %s (%s)\n",
                   device.discovery.name.c_str(),
                   device.discovery.identifier.c_str());
        for (auto& reg : device.virtualSensors) {
            int err;
            reg.compiledExpression =
                te_compile(reg.expression.c_str(), device.vars.data(), device.vars.size(), &err);

            if (!reg.compiledExpression) {
                Log.printf("Failed to compile '%s': %s (position %d)\n",
                           reg.getName().c_str(),
                           reg.expression.c_str(),
                           err);
            }
        }
    }

    // Returns true if the value of the virtual sensor has changed, false otherwise
    bool updateRegister(VirtualSensor& reg) {
        if (!reg.compiledExpression) {
            return false;
        }

        double result = te_eval(reg.compiledExpression);

        constexpr double EPSILON = 1e-9;
        bool changed = fabs(result - reg.value) > EPSILON;

        if (reg.rounding > 0) {
            result = Numbers::applyRounding(result, reg.rounding);
        }

        reg.value = result;

        if (reg.isPersistent && changed) {
            reg.isDirty = true;
        }

        return changed;
    }

    void resetDevice(ModbusDevice& device) {
        device.vars.clear();

        for (auto& virtualSensor : device.virtualSensors) {
            if (virtualSensor.compiledExpression) {
                te_free(virtualSensor.compiledExpression);
                virtualSensor.compiledExpression = nullptr;
            }
        }
    }

}  // namespace VirtualSensorManager