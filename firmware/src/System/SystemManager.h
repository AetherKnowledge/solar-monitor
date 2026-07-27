#pragma once

#include "SystemTypes.h"

namespace SystemManager {

    extern SystemDevice systemDevice;

    void setup();
    void loop();
    void requestRestart();

    void setupSensors();
    void setupControls();

}  // namespace SystemManager