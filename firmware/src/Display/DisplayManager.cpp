#include "DisplayManager.h"
#include <U8g2lib.h>
#include <Wire.h>

namespace DisplayManager {

    constexpr uint8_t SDA_PIN = 25;
    constexpr uint8_t SCL_PIN = 26;
    constexpr uint8_t I2C_ADDRESS = 0x3F;

    U8G2_ST7567_ENH_DG128064I_F_HW_I2C display(U8G2_R2, U8X8_PIN_NONE);

    void setup() {
        Wire.begin(SDA_PIN, SCL_PIN);

        display.setI2CAddress(I2C_ADDRESS << 1);
        display.begin();

        display.clearBuffer();
        display.setFont(u8g2_font_ncenB08_tr);

        display.drawStr(0, 15, "Solar Monitor");

        display.sendBuffer();
    }

    void loop() {
        // Handle display tasks
    }
}  // namespace DisplayManager