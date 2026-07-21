#pragma once

namespace SystemManager {
    extern volatile bool requestRestart;

    void setup();
    void loop();
}  // namespace SystemManager