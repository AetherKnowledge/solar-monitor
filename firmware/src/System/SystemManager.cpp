#include "SystemManager.h"
#include <Arduino.h>
#include <esp_system.h>
#include <Common/Logger.h>
#include <Display/DisplayManager.h>
#include <Modbus/VirtualSensorManager.h>
#include <Config/ConfigManager.h>

namespace SystemManager {
    volatile bool requestedRestart = false;
    unsigned long lastBuzz = 0;
    bool buzzerActive = false;

    void setup() {
        // Initialize system manager
    }

    void loop() {
        // Handle system tasks
        if (requestedRestart) {
            DisplayManager::showLoadingSpinner("Restarting...");
            // Perform restart logic
            requestedRestart = false;

            VirtualSensorManager::savePersistence(ConfigManager::config.modbusDevices);
            Log.println("Restarting...");

            delay(1000);
            ESP.restart();
        }
    }

    void requestRestart() {
        requestedRestart = true;
    }
}  // namespace SystemManager