#pragma once

#include "DisplayManager.h"
#include "DisplayTypes.h"

namespace DisplayManager {

    //--------------------------------------------------------------------------
    // Icon Size
    //--------------------------------------------------------------------------

    struct IconSize {
        uint8_t width;
        uint8_t height;
    };

    //--------------------------------------------------------------------------
    // Icon Drawing
    //--------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    // Glyph Helper
    //------------------------------------------------------------------------------

    inline void drawGlyphIcon(
        int x, int y, const uint8_t* font, uint16_t glyph, const IconSize& size) {
        display.setFont(font);

        int baseline = y + (size.height - display.getMaxCharHeight()) / 2 + display.getAscent();

        display.drawGlyph(x, baseline, glyph);
    }

    //------------------------------------------------------------------------------
    // Warning
    //------------------------------------------------------------------------------

    inline void drawWarningIcon(int x, int y) {
        drawGlyphIcon(x, y, u8g2_font_twelvedings_t_all, 0x0021, {16, 16});
    }

    //------------------------------------------------------------------------------
    // Info
    //------------------------------------------------------------------------------

    inline void drawInfoIcon(int x, int y) {
        drawGlyphIcon(x, y, u8g2_font_open_iconic_embedded_1x_t, 0x0049, {8, 8});
    }

    //------------------------------------------------------------------------------
    // Check
    //------------------------------------------------------------------------------

    inline void drawCheckIcon(int x, int y) {
        drawGlyphIcon(x, y, u8g2_font_open_iconic_check_1x_t, 0x0040, {8, 8});
    }

    //------------------------------------------------------------------------------
    // Error
    //------------------------------------------------------------------------------

    inline void drawErrorIcon(int x, int y) {
        drawGlyphIcon(x, y, u8g2_font_open_iconic_check_1x_t, 0x0048, {8, 8});
    }

    //------------------------------------------------------------------------------
    // Upload
    //------------------------------------------------------------------------------

    inline void drawUploadIcon(int x, int y) {
        drawGlyphIcon(x, y, u8g2_font_open_iconic_arrow_1x_t, 0x0041, {8, 8});
    }

    //------------------------------------------------------------------------------
    // Download
    //------------------------------------------------------------------------------

    inline void drawDownloadIcon(int x, int y) {
        drawGlyphIcon(x, y, u8g2_font_open_iconic_arrow_1x_t, 0x0040, {8, 8});
    }

    //------------------------------------------------------------------------------
    // WiFi
    //------------------------------------------------------------------------------

    inline void drawWifiIcon(int x, int y, uint8_t strength = 4) {
        strength = min<uint8_t>(strength, 4);

        // Disconnected: first bar + X
        if (strength == 0) {
            // First (smallest) bar
            display.drawVLine(x, y + 6, 2);

            // X
            display.drawLine(x + 2, y + 2, x + 7, y + 7);
            display.drawLine(x + 7, y + 2, x + 2, y + 7);
            return;
        }

        constexpr uint8_t heights[4] = {2, 4, 6, 8};

        for (uint8_t i = 0; i < strength; i++) {
            int barX = x + i * 2;
            int h = heights[i];

            display.drawVLine(barX, y + 8 - h, h);
        }
    }

    //------------------------------------------------------------------------------
    // MQTT
    //------------------------------------------------------------------------------

    inline void drawMqttIcon(int x, int y, bool connected = true) {
        if (connected) {
            drawGlyphIcon(x, y, u8g2_font_open_iconic_all_1x_t, 0x00F8, {8, 8});
            return;
        }

        // Small MQTT ring
        display.drawCircle(x + 3, y + 4, 2);

        // Center dot
        display.drawPixel(x + 3, y + 4);

        // X
        display.drawLine(x + 5, y + 1, x + 7, y + 3);
        display.drawLine(x + 5, y + 3, x + 7, y + 1);
    }

    inline void drawUnknownIcon(int x, int y) {
        display.setFont(u8g2_font_6x10_tr);
        display.drawStr(x, y + 8, "?");
    }

    //--------------------------------------------------------------------------
    // Size Lookup
    //--------------------------------------------------------------------------

    inline IconSize getIconSize(Icon icon) {
        switch (icon) {
            case Icon::Warning:
                return {16, 16};

            case Icon::Info:
            case Icon::Check:
            case Icon::Error:
            case Icon::Upload:
            case Icon::Download:
            case Icon::Wifi:
            case Icon::MQTT:
                return {8, 8};

            default:
                return {8, 8};
        }
    }

    //--------------------------------------------------------------------------
    // Draw Dispatcher
    //--------------------------------------------------------------------------

    inline void drawIcon(Icon icon, int x, int y) {
        switch (icon) {
            case Icon::Warning:
                drawWarningIcon(x, y);
                break;

            case Icon::Info:
                drawInfoIcon(x, y);
                break;

            case Icon::Check:
                drawCheckIcon(x, y);
                break;

            case Icon::Error:
                drawErrorIcon(x, y);
                break;

            case Icon::Upload:
                drawUploadIcon(x, y);
                break;

            case Icon::Download:
                drawDownloadIcon(x, y);
                break;

            case Icon::MQTT:
                drawMqttIcon(x, y);
                break;
            case Icon::Wifi:
                drawWifiIcon(x, y);
                break;

            default:
                drawUnknownIcon(x, y);
                return;
        }
    }

}  // namespace DisplayManager