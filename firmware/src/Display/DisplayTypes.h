#pragma once

#include <Arduino.h>
#include <U8g2lib.h>
#include <Modbus/ModbusTypes.h>
#include <optional>

namespace DisplayManager {

    //--------------------------------------------------------------------------
    // Text
    //--------------------------------------------------------------------------

    enum class TextAlignment : uint8_t { Left, Center, Right };

    //--------------------------------------------------------------------------
    // Built-in Icons
    //--------------------------------------------------------------------------

    enum class Icon : uint8_t {
        Warning,   // glyph
        Info,      // glyph
        Check,     // glyph
        Error,     // glyph
        Upload,    // glyph
        Download,  // glyph
        MQTT,      // glyph

        Wifi,     // custom
        Modbus,   // custom
        Battery,  // custom
        Power     // custom
    };

    //--------------------------------------------------------------------------
    // Spinner
    //--------------------------------------------------------------------------

    struct SpinnerStyle {
        int centerX = 64;
        int centerY = 53;

        // Distance between each square.
        int spacing = 2;

        // Width/height of each square.
        int squareSize = 2;

        // Number of visible squares.
        int trailLength = 4;

        int titleY = 20;
        int messageY = 34;

        const char* title = "Solar Monitor";

        const uint8_t* titleFont = u8g2_font_ncenB08_tr;
        const uint8_t* messageFont = u8g2_font_6x10_tr;
    };

    //--------------------------------------------------------------------------
    // Status Bar Style
    //--------------------------------------------------------------------------

    struct StatusBarStyle {
        int height = 9;

        int padding = 2;

        int iconSpacing = 2;

        bool showBorder = true;
        bool showTitle = true;
        bool centerTitle = false;

        const uint8_t* font = u8g2_font_4x6_mf;
    };

    //--------------------------------------------------------------------------
    // Status Bar State
    //--------------------------------------------------------------------------

    struct StatusBarState {
        String title = "Solar Monitor";
        String deviceName = "No Device";

        uint8_t wifiStrength = 0;
        bool mqttConnected = false;

        bool modbusVisible = false;
        bool modbusConnected = false;

        bool websocket = false;
        bool activity = false;
    };

    enum class DisplayScreen { LoadingProgress, LoadingSpinner, Error, Success, DeviceInfo };

    struct DisplayState {
        DisplayScreen screen = DisplayScreen::LoadingSpinner;

        uint8_t deviceInfoIndex = 0;
        std::vector<DisplayData> displayDevices;
        String message = "Loading...";
        uint8_t progress = 0;

        SpinnerStyle spinnerStyle{};
    };

}  // namespace DisplayManager