#include "SoundManager.h"
#include "Pins.h"

namespace SoundManager {
    constexpr uint8_t BUZZER_RESOLUTION = 8;

    uint32_t startTime = 0;
    uint32_t duration = 0;
    bool playing = false;

    void setup() {
        ledcAttach(Pins::BUZZER, 2000, BUZZER_RESOLUTION);

        stop();
    }

    void loop() {
        if (playing && millis() - startTime >= duration) {
            stop();
        }
    }

    void playTone(uint32_t durationMs, uint8_t volume, uint32_t frequency) {
        ledcWriteTone(Pins::BUZZER, frequency);
        ledcWrite(Pins::BUZZER, volume);

        startTime = millis();
        duration = durationMs;
        playing = true;
    }

    void stop() {
        ledcWrite(Pins::BUZZER, 0);
        playing = false;
    }
}  // namespace SoundManager