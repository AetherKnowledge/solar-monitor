#pragma once
#include "DisplayManager.h"

namespace DisplayManager {

    enum class TextAlignment { Left, Center, Right };
    enum class Icon : uint16_t { Warning = 0x0022 };

    inline int centerX(int width, int left = 0, int right = display.getDisplayWidth()) {
        return left + ((right - left) - width) / 2;
    }

    inline int centerY(int height, int top = 0, int bottom = display.getDisplayHeight()) {
        return top + ((bottom - top) - height) / 2;
    }

    inline int centerTextX(const char* text, int left = 0, int right = display.getDisplayWidth()) {
        return left + ((right - left) - display.getStrWidth(text)) / 2;
    }

    inline void drawStr(int x,
                        int y,
                        const char* text,
                        TextAlignment alignment = TextAlignment::Left) {
        switch (alignment) {
            case TextAlignment::Left:
                display.drawStr(x, y, text);
                break;
            case TextAlignment::Center:
                display.drawStr(centerTextX(text, x), y, text);
                break;
            case TextAlignment::Right:
                display.drawStr(x - display.getStrWidth(text), y, text);
                break;
        }
    }

    inline void drawGlyphText(int x,
                              int y,
                              const uint8_t* glyphFont,
                              uint16_t glyph,
                              int glyphWidth,
                              int glyphHeight,
                              const uint8_t* textFont,
                              const char* text,
                              TextAlignment alignment = TextAlignment::Left,
                              int spacing = 4,
                              int right = display.getDisplayWidth()) {
        // Measure text
        display.setFont(textFont);
        int textWidth = display.getStrWidth(text);
        int textHeight = display.getAscent() - display.getDescent();

        int totalWidth = glyphWidth + spacing + textWidth;

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

        // Draw glyph
        display.setFont(glyphFont);

        int glyphBaseline =
            y + (glyphHeight - display.getMaxCharHeight()) / 2 + display.getAscent();

        display.drawGlyph(x, glyphBaseline, glyph);

        // Draw text
        display.setFont(textFont);

        int textBaseline = y + (glyphHeight - textHeight) / 2 + display.getAscent();

        display.drawStr(x + glyphWidth + spacing, textBaseline, text);
    }

}  // namespace DisplayManager