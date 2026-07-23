#pragma once

#include <Arduino.h>
#include "DisplayError.h"
#include "DisplayTypes.h"
#include <U8g2lib.h>

namespace DisplayManager {
    inline U8G2_ST7567_ENH_DG128064I_F_HW_I2C display(U8G2_R2, U8X8_PIN_NONE);

    void setup();
    void loop();
    void showLoadingProgress(const char* message, uint8_t percent);
    void showLoadingSpinner(const char* message, const SpinnerStyle& style = {});
    void showError(DisplayError error);
    void showError(const char* message);
}  // namespace DisplayManager