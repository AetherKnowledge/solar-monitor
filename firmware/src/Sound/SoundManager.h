#pragma once

#include <Arduino.h>

namespace SoundManager {
    void setup();
    void loop();
    void playTone(uint32_t durationMs = 500, uint8_t volume = 30, uint32_t frequency = 2000);
    void stop();
}  // namespace SoundManager