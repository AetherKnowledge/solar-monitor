#include "DisplayError.h"
#include "DisplayManager.h"
#include "DisplayTools.h"
#include <Wire.h>
#include <U8g2lib.h>

namespace DisplayManager {
    constexpr uint8_t SDA_PIN = 25;
    constexpr uint8_t SCL_PIN = 26;
    constexpr uint8_t I2C_ADDRESS = 0x3F;

    void setup() {
        Wire.begin(SDA_PIN, SCL_PIN);

        display.setI2CAddress(I2C_ADDRESS << 1);
        display.begin();

        display.setFont(u8g2_font_ncenB08_tr);
        display.clearBuffer();
    }

    void showLoadingProgress(const char* message, uint8_t percent) {
        constexpr uint8_t BAR_X = 10;
        constexpr uint8_t BAR_Y = 48;
        constexpr uint8_t BAR_WIDTH = 108;
        constexpr uint8_t BAR_HEIGHT = 10;
        constexpr uint8_t BAR_PADDING = 2;

        display.clearBuffer();

        display.setFont(u8g2_font_ncenB08_tr);
        drawStr(0, 18, "Solar Monitor", TextAlignment::Center);

        display.setFont(u8g2_font_6x10_tr);
        drawStr(0, 35, message, TextAlignment::Center);

        display.drawFrame(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT);

        uint8_t fill = ((BAR_WIDTH - BAR_PADDING * 2) * percent) / 100;

        display.drawBox(
            BAR_X + BAR_PADDING, BAR_Y + BAR_PADDING, fill, BAR_HEIGHT - BAR_PADDING * 2);

        display.sendBuffer();
    }

    void showLoadingSpinner(const char* message, const SpinnerStyle& style) {
        static uint8_t frame = 0;

        display.clearBuffer();

        // Title
        display.setFont(style.titleFont);
        drawStr(0, style.titleY, style.title, TextAlignment::Center);

        // Message
        display.setFont(style.messageFont);
        drawStr(0, style.messageY, message, TextAlignment::Center);

        constexpr int POINTS = 16;

        // Calculate spacing based on desired radius.
        int step = std::max(1, style.radius / 2);

        // 16 positions around a square.
        static constexpr int8_t positions[POINTS][2] = {// Top (5)
                                                        {-2, -2},
                                                        {-1, -2},
                                                        {0, -2},
                                                        {1, -2},
                                                        {2, -2},

                                                        // Right (3)
                                                        {2, -1},
                                                        {2, 0},
                                                        {2, 1},

                                                        // Bottom (5)
                                                        {2, 2},
                                                        {1, 2},
                                                        {0, 2},
                                                        {-1, 2},
                                                        {-2, 2},

                                                        // Left (3)
                                                        {-2, 1},
                                                        {-2, 0},
                                                        {-2, -1}};

        for (int i = 0; i < style.trailLength; i++) {
            int idx = (frame + POINTS - i) % POINTS;

            display.drawBox(style.centerX + positions[idx][0] * step,
                            style.centerY + positions[idx][1] * step,
                            style.squareSize,
                            style.squareSize);
        }

        display.sendBuffer();

        frame = (frame + 1) % POINTS;
    }

    void showError(const char* message) {
        display.clearBuffer();

        // Title
        display.setFont(u8g2_font_ncenB08_tr);
        drawStr(0, 18, "Error", TextAlignment::Center);

        // Icon + message
        display.setFont(u8g2_font_ncenB08_tr);

        drawGlyphText(0,
                      22,
                      u8g2_font_twelvedings_t_all,
                      static_cast<uint16_t>(Icon::Warning),
                      16,
                      16,
                      u8g2_font_6x10_tr,
                      message,
                      TextAlignment::Center);

        // Footer

        display.setFont(u8g2_font_6x10_tr);
        display.drawHLine(0, 54, 128);
        drawStr(0, 63, "Please restart", TextAlignment::Center);

        display.sendBuffer();
    }

    void showError(DisplayError error) {
        const char* message = getErrorMessage(error);
        showError(message);
    }

    void loop() {
        // Handle display tasks
    }
}  // namespace DisplayManager