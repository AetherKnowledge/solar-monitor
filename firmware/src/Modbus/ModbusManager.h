#pragma once

#include "ModbusTypes.h"
#include <set>
#include <vector>
#include <Common/UpdateStatus.h>
#include <optional>

namespace ModbusManager {

    struct PendingUpdate {
        String identifier;
        JsonDocument patch;
    };

    extern bool hasStarted;
    extern std::set<int> portsInUse;

    extern volatile UpdateStatus updateStatus;
    extern std::optional<PendingUpdate> pendingUpdate;
    extern double pollDeltaSeconds;

    extern DisplayData currentDisplayData;

    void setup();
    void loop();
    void pollDevice(ModbusDevice& device);
    void publishDisplayData();
    void reload();
    void requestUpdate(const String& id, const JsonVariantConst& json);
    bool updateConfig(PendingUpdate& update);
    void setupDevices(std::vector<ModbusDevice>& devices);

    bool processReadRegisters(ModbusDevice& device, ReadGroup& group);
    void updateVirtualSensors(ModbusDevice& device);
    void getValues(JsonDocument& doc, String& id);

    void reset();
}  // namespace ModbusManager