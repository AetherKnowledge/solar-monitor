#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

namespace DisplayManager {
    struct SpinnerStyle {
        int centerX = 64;
        int centerY = 53;

        // Controls overall size.
        int radius = 4;

        // Size of each square.
        int squareSize = 2;

        // Number of lit squares.
        int trailLength = 4;

        int titleY = 16;
        int messageY = 34;

        const char* title = "Solar Monitor";

        const uint8_t* titleFont = u8g2_font_ncenB08_tr;
        const uint8_t* messageFont = u8g2_font_6x10_tr;
    };
}  // namespace DisplayManager