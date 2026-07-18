#include "CalculatedRegisterManager.h"
#include <tinyexpr.h>
#include <unordered_set>

namespace CalculatedRegisterManager
{
    using unordered_set_t = std::unordered_set<std::string>;

    void setupDevice(ModbusDevice &device)
    {
        Serial.printf(
            "Setting up calculated registers for device %s (%s)\n",
            device.name.c_str(),
            device.identifier.c_str());

        resetDevice(device);

        device.vars.clear();
        device.vars.reserve(device.readRegisters.size());

        unordered_set_t usedIds;
        for (auto &readRegister : device.readRegisters)
        {
            addVariable(device, readRegister, usedIds);
        }

        // calculated registers are evaluated in order
        for (auto &calculatedRegister : device.calculatedRegisters)
        {
            addVariable(device, calculatedRegister, usedIds);
        }

        compileExpressions(device);
    }

    void addVariable(ModbusDevice &device, Register &reg, unordered_set_t &usedIds)
    {
        if (reg.getId().isEmpty())
        {
            Serial.printf(
                "Register '%s' has no unique ID.\n",
                reg.getName().c_str());
            return;
        }

        auto [_, inserted] = usedIds.insert(
            reg.getId().c_str());

        if (!inserted)
        {
            Serial.printf(
                "Duplicate unique ID '%s' found for register '%s'.\n",
                reg.getId().c_str(),
                reg.getName().c_str());
            return;
        }

        te_variable var;
        var.name = reg.getId().c_str();
        var.address = &reg.value;
        var.type = TE_VARIABLE;
        var.context = nullptr;

        device.vars.push_back({reg.getId().c_str(),
                               &reg.value,
                               TE_VARIABLE,
                               nullptr});

        Serial.printf(
            "Added variable for calculated registers: %s (name: %s)\n",
            reg.getId(),
            reg.getName());
    }

    void compileExpressions(ModbusDevice &device)
    {
        Serial.printf("Compiling expressions for device %s (%s)\n", device.name.c_str(), device.identifier.c_str());
        for (auto &reg : device.calculatedRegisters)
        {
            int err;
            reg.compiledExpression = te_compile(reg.expression.c_str(),
                                                device.vars.data(),
                                                device.vars.size(),
                                                &err);

            if (!reg.compiledExpression)
            {
                Serial.printf(
                    "Failed to compile '%s': %s (position %d)\n",
                    reg.getName().c_str(),
                    reg.expression.c_str(),
                    err);
            }
        }
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

        for (auto &calculatedRegister : device.calculatedRegisters)
        {
            if (calculatedRegister.compiledExpression)
            {
                te_free(calculatedRegister.compiledExpression);
                calculatedRegister.compiledExpression = nullptr;
            }
        }
    }

}