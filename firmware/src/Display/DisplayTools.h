#pragma once

#include "DisplayIcons.h"
#include "DisplayManager.h"
#include "DisplayTypes.h"

namespace DisplayManager {

    //--------------------------------------------------------------------------
    // Position Helpers
    //--------------------------------------------------------------------------

    inline int centerX(int width, int left = 0, int right = display.getDisplayWidth()) {
        return left + ((right - left) - width) / 2;
    }

    inline int centerY(int height, int top = 0, int bottom = display.getDisplayHeight()) {
        return top + ((bottom - top) - height) / 2;
    }

    inline int centerTextX(const char* text, int left = 0, int right = display.getDisplayWidth()) {
        return centerX(display.getStrWidth(text), left, right);
    }

    inline int centerTextY(const uint8_t* font,
                           int top = 0,
                           int bottom = display.getDisplayHeight()) {
        display.setFont(font);

        int textHeight = display.getAscent() - display.getDescent();

        return centerY(textHeight, top, bottom) + display.getAscent();
    }

    inline int centerIconX(Icon icon, int left = 0, int right = display.getDisplayWidth()) {
        return centerX(getIconSize(icon).width, left, right);
    }

    inline int centerIconY(Icon icon, int top = 0, int bottom = display.getDisplayHeight()) {
        return centerY(getIconSize(icon).height, top, bottom);
    }

    //--------------------------------------------------------------------------
    // Text
    //--------------------------------------------------------------------------

    inline void drawStr(int x,
                        int y,
                        const char* text,
                        TextAlignment alignment = TextAlignment::Left,
                        int right = display.getDisplayWidth()) {
        switch (alignment) {
            case TextAlignment::Center:
                x = centerTextX(text, x, right);
                break;

            case TextAlignment::Right:
                x = right - display.getStrWidth(text);
                break;

            case TextAlignment::Left:
            default:
                break;
        }

        display.drawStr(x, y, text);
    }

    //--------------------------------------------------------------------------
    // Icon
    //--------------------------------------------------------------------------

    inline void drawIconAligned(int x,
                                int y,
                                Icon icon,
                                TextAlignment alignment = TextAlignment::Left,
                                int right = display.getDisplayWidth()) {
        switch (alignment) {
            case TextAlignment::Center:
                x = centerIconX(icon, x, right);
                break;

            case TextAlignment::Right:
                x = right - getIconSize(icon).width;
                break;

            case TextAlignment::Left:
            default:
                break;
        }

        drawIcon(icon, x, y);
    }

    //--------------------------------------------------------------------------
    // Icon + Text
    //--------------------------------------------------------------------------

    inline void drawIconText(int x,
                             int y,
                             Icon icon,
                             const char* text,
                             const uint8_t* font = u8g2_font_6x10_tr,
                             TextAlignment alignment = TextAlignment::Left,
                             int spacing = 4,
                             int right = display.getDisplayWidth()) {
        display.setFont(font);

        auto iconSize = getIconSize(icon);

        int textWidth = display.getStrWidth(text);
        int totalWidth = iconSize.width + spacing + textWidth;

        switch (alignment) {
            case TextAlignment::Center:
                x = centerX(totalWidth, x, right);
                break;

            case TextAlignment::Right:
                x = right - totalWidth;
                break;

            case TextAlignment::Left:
            default:
                break;
        }

        drawIcon(icon, x, y);

        display.setFont(font);

        int ascent = display.getAscent();
        int descent = display.getDescent();  // Usually negative

        // Center of the icon
        int centerY = y + iconSize.height / 2;

        // Position the text baseline so the text's bounding box is centered
        int baseline = centerY + (ascent + descent) / 2;

        display.drawStr(x + iconSize.width + spacing, baseline, text);
    }

    //--------------------------------------------------------------------------
    // Convenience Overload
    //--------------------------------------------------------------------------

    inline void drawIconText(int x,
                             int y,
                             Icon icon,
                             const char* text,
                             TextAlignment alignment = TextAlignment::Left,
                             int spacing = 4,
                             int right = display.getDisplayWidth()) {
        drawIconText(x, y, icon, text, u8g2_font_6x10_tr, alignment, spacing, right);
    }

}  // namespace DisplayManager