#include "CalculatedRegisterManager.h"
#include <tinyexpr.h>
#include <unordered_set>

namespace CalculatedRegisterManager
{
    void setupDevice(ModbusDevice &device)
    {
        Serial.printf(
            "Setting up calculated registers for device %s (%s)\n",
            device.name.c_str(),
            device.identifier.c_str());

        resetDevice(device);

        device.vars.reserve(device.readRegisters.size());

        std::unordered_set<std::string> usedIds;
        for (auto &readRegister : device.readRegisters)
        {
            if (readRegister.discoveryConfig.uniqueId.isEmpty())
            {
                Serial.printf("Read register at address %u has no unique ID. Skipping for calculated registers.\n", readRegister.address);
                continue;
            }

            auto [_, inserted] = usedIds.insert(
                readRegister.discoveryConfig.uniqueId.c_str());

            if (!inserted)
            {
                Serial.printf("Duplicate unique ID '%s' found for read register at address %u. Skipping for calculated registers.\n", readRegister.discoveryConfig.uniqueId.c_str(), readRegister.address);
                continue;
            }

            te_variable var;
            var.name = readRegister.discoveryConfig.uniqueId.c_str();
            var.address = &readRegister.value;
            var.type = TE_VARIABLE;
            var.context = nullptr;

            device.vars.push_back(var);

            Serial.printf(
                "Added variable for calculated registers: %s (address: %d)\n",
                var.name,
                readRegister.address);
        }

        compileExpressions(device);
    }

    // Returns true if the value of the calculated register has changed, false otherwise
    bool updateRegister(CalculatedRegister &calculatedRegister)
    {
        if (!calculatedRegister.compiledExpression)
        {
            return false;
        }

        double result = te_eval(calculatedRegister.compiledExpression);
        bool changed = (result != calculatedRegister.value);
        calculatedRegister.value = result;

        return changed;
    }

    void resetDevice(ModbusDevice &device)
    {
        device.vars.clear();
        device.vars.shrink_to_fit();

        for (auto &calculatedRegister : device.calculatedRegisters)
        {
            if (calculatedRegister.compiledExpression)
            {
                te_free(calculatedRegister.compiledExpression);
                calculatedRegister.compiledExpression = nullptr;
            }
        }
    }

    void compileExpressions(ModbusDevice &device)
    {
        Serial.printf("Compiling expressions for device %s (%s)\n", device.name.c_str(), device.identifier.c_str());
        for (auto &calculatedRegister : device.calculatedRegisters)
        {
            int err;
            te_expr *expr = te_compile(calculatedRegister.expression.c_str(),
                                       device.vars.data(),
                                       device.vars.size(),
                                       &err);

            if (!expr)
            {
                Serial.printf("Failed to compile expression: %s, error at position: %d\n", calculatedRegister.expression.c_str(), err);
                continue;
            }

            calculatedRegister.compiledExpression = expr;
        }
    }

}