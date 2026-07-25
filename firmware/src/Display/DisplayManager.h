#pragma once

#include <Arduino.h>
#include "DisplayError.h"
#include "DisplayTypes.h"
#include <U8g2lib.h>

namespace DisplayManager {
    inline U8G2_ST7567_ENH_DG128064I_F_HW_I2C display(U8G2_R2, U8X8_PIN_NONE);

    void setup();
    void renderStatusBar();
    void finishBoot();
    void pollWifiStrength();

    void renderLoop();
    void inputLoop();

    void updateDeviceInfo(const std::vector<DisplayData>& devices);
    void showLoadingProgress(const String& message, uint8_t percent);
    void showLoadingSpinner(const String& message, const SpinnerStyle& style = {});
    void showError(const String& message);
    void showSuccess(const String& message);
    void showDeviceInfo();
    void nextDevice();
    void previousDevice();
}  // namespace DisplayManager