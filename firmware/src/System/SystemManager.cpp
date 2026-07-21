#include "SystemManager.h"
#include <Arduino.h>

namespace SystemManager {
    volatile bool requestRestart = false;

    void setup() {
        // Initialize system manager
    }

    void loop() {
        // Handle system tasks
        if (requestRestart) {
            // Perform restart logic
            requestRestart = false;

            Serial.println("Restarting...");

            delay(1000);
            ESP.restart();
        }
    }
}  // namespace SystemManager