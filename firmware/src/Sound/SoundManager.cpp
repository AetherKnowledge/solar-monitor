#include "SoundManager.h"

namespace SoundManager {
    constexpr uint8_t BUZZER_PIN = 33;
    constexpr uint8_t BUZZER_CHANNEL = 0;
    constexpr uint8_t BUZZER_RESOLUTION = 8;

    uint32_t startTime = 0;
    uint32_t duration = 0;
    bool playing = false;

    void setup() {
        ledcSetup(BUZZER_CHANNEL, 2000, BUZZER_RESOLUTION);
        ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

        stop();
    }

    void loop() {
        if (playing && millis() - startTime >= duration) {
            stop();
        }
    }

    void playTone(uint32_t durationMs, uint8_t volume, uint32_t frequency) {
        ledcWriteTone(BUZZER_CHANNEL, frequency);
        ledcWrite(BUZZER_CHANNEL, volume);

        startTime = millis();
        duration = durationMs;
        playing = true;
    }

    void stop() {
        ledcWrite(BUZZER_CHANNEL, 0);
        playing = false;
    }
}  // namespace SoundManager