#include "SystemManager.h"
#include <Arduino.h>
#include <esp_system.h>
#include <Common/Logger.h>

namespace SystemManager {
    volatile bool requestRestart = false;
    unsigned long lastBuzz = 0;
    bool buzzerActive = false;

    void setup() {
        // Initialize system manager
    }

    void loop() {
        // Handle system tasks
        if (requestRestart) {
            // Perform restart logic
            requestRestart = false;

            Log.println("Restarting...");

            delay(1000);
            ESP.restart();
        }
    }
}  // namespace SystemManager