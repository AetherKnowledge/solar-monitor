#include "DisplayError.h"
#include "DisplayManager.h"
#include "DisplayTools.h"
#include <Wire.h>
#include <U8g2lib.h>
#include <Networking/NetworkManager.h>
#include <Update/UpdateHandler.h>
#include <Mqtt/MqttManager.h>
#include <Common/Logger.h>

namespace DisplayManager {
    constexpr uint8_t SDA_PIN = 25;
    constexpr uint8_t SCL_PIN = 26;
    constexpr uint8_t I2C_ADDRESS = 0x3F;
    bool statusBarVisible = false;
    StatusBarStyle statusBarStyle;
    StatusBarState statusBarState;

    SemaphoreHandle_t displayMutex = nullptr;
    DisplayState displayState;

    class DisplayLock {
       public:
        DisplayLock() {
            if (displayMutex)
                xSemaphoreTake(displayMutex, portMAX_DELAY);
        }

        ~DisplayLock() {
            if (displayMutex)
                xSemaphoreGive(displayMutex);
        }
    };

    static void displayTask(void* parameter) {
        while (true) {
            renderLoop();
        }
    }

    void setup() {
        Wire.begin(SDA_PIN, SCL_PIN);

        display.setI2CAddress(I2C_ADDRESS << 1);
        display.begin();

        display.setFont(u8g2_font_ncenB08_tr);
        display.clearBuffer();

        displayMutex = xSemaphoreCreateMutex();
        if (displayMutex == nullptr) {
            Log.println("Failed to create display mutex");
            return;
        }

        xTaskCreatePinnedToCore(displayTask,  // Task function
                                "Display",    // Name
                                4096,         // Stack size
                                nullptr,      // Parameter
                                1,            // Priority
                                nullptr,      // Task handle
                                1             // Core (1 is usually best)
        );
    }

    void renderStatusBar() {
        display.setFont(statusBarStyle.font);

        //----------------------------------------
        // separator
        //----------------------------------------

        if (statusBarStyle.showBorder)
            display.drawHLine(0, statusBarStyle.height, display.getDisplayWidth());

        //----------------------------------------
        // title
        //----------------------------------------

        if (statusBarStyle.showTitle) {
            if (statusBarStyle.centerTitle) {
                drawStr(0, 8, statusBarState.title.c_str(), TextAlignment::Center);

            } else {
                drawStr(statusBarStyle.padding, 8, statusBarState.title.c_str());
            }
        }

        //----------------------------------------
        // icons
        //----------------------------------------

        int x = display.getDisplayWidth() - statusBarStyle.padding - 3;
        const auto mqttSize = getIconSize(Icon::MQTT);
        drawMqttIcon(x - mqttSize.width, 0);
        x -= mqttSize.width + statusBarStyle.iconSpacing;

        const auto wifiSize = getIconSize(Icon::Wifi);
        drawWifiIcon(x - wifiSize.width, 0, statusBarState.wifiStrength);
        x -= wifiSize.width + statusBarStyle.iconSpacing;
    }

    void finishBoot() {
        statusBarVisible = true;
    }

    void showLoadingProgress(const String& message, uint8_t progress) {
        DisplayLock lock;

        displayState.screen = DisplayScreen::LoadingProgress;
        displayState.message = message;
        displayState.progress = progress;
    }

    void showLoadingSpinner(const String& message, const SpinnerStyle& style) {
        DisplayLock lock;

        displayState.screen = DisplayScreen::LoadingSpinner;
        displayState.message = message;
        displayState.spinnerStyle = style;
    }

    void showError(const String& message) {
        DisplayLock lock;
        displayState.screen = DisplayScreen::Error;
        displayState.message = message;
    }

    void showSuccess(const String& message) {
        DisplayLock lock;
        displayState.screen = DisplayScreen::Success;
        displayState.message = message;
    }

    inline void beginFrame() {
        display.clearBuffer();

        if (statusBarVisible) {
            renderStatusBar();
        }
    }

    inline void endFrame() {
        display.sendBuffer();
    }

    static void renderLoadingProgress(const String& message, uint8_t percent) {
        constexpr uint8_t BAR_X = 10;
        constexpr uint8_t BAR_Y = 48;
        constexpr uint8_t BAR_WIDTH = 108;
        constexpr uint8_t BAR_HEIGHT = 10;
        constexpr uint8_t BAR_PADDING = 2;

        beginFrame();

        display.setFont(u8g2_font_ncenB08_tr);
        drawStr(0, 20, "Solar Monitor", TextAlignment::Center);

        display.setFont(u8g2_font_6x10_tr);
        drawStr(0, 35, message.c_str(), TextAlignment::Center);

        display.drawFrame(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT);

        uint8_t fill = ((BAR_WIDTH - BAR_PADDING * 2) * percent) / 100;

        display.drawBox(
            BAR_X + BAR_PADDING, BAR_Y + BAR_PADDING, fill, BAR_HEIGHT - BAR_PADDING * 2);

        endFrame();
    }

    static void renderLoadingSpinner(const String& message, const SpinnerStyle& style) {
        static uint8_t frame = 0;

        beginFrame();

        // Title
        display.setFont(style.titleFont);
        drawStr(0, style.titleY, style.title, TextAlignment::Center);

        // Message
        display.setFont(style.messageFont);
        drawStr(0, style.messageY, message.c_str(), TextAlignment::Center);

        constexpr int POINTS = 16;

        // Calculate spacing based on desired radius.
        int step = std::max(1, style.spacing);

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

        endFrame();

        frame = (frame + 1) % POINTS;
    }

    static void renderError(const String& message) {
        beginFrame();

        // Title
        display.setFont(u8g2_font_ncenB08_tr);
        drawStr(0, 20, "Error", TextAlignment::Center);

        // Icon + message
        drawIconText(0, 26, Icon::Warning, message.c_str(), TextAlignment::Center);
        // Footer

        display.setFont(u8g2_font_6x10_tr);
        display.drawHLine(0, 54, 128);
        drawStr(0, 63, "Please restart", TextAlignment::Center);

        endFrame();
    }

    static void renderSuccess(const String& message) {
        beginFrame();

        // Title
        display.setFont(u8g2_font_ncenB08_tr);
        drawStr(0, 20, "Success", TextAlignment::Center);

        // Icon + message
        drawIconText(0, 26, Icon::Check, message.c_str(), TextAlignment::Center);

        endFrame();
    }

    static void pollUpdateProgress() {
        const auto& progress = UpdateHandler::getUpdateProgress();

        if (progress.status == UpdateStatus::InProgress && progress.hasTotalSize) {
            showLoadingProgress("Updating...", progress.progress);
        } else if (progress.status == UpdateStatus::InProgress && !progress.hasTotalSize) {
            showLoadingSpinner("Updating...");
        } else if (progress.status == UpdateStatus::UpdateComplete) {
            showSuccess("Update Complete");
        } else if (progress.status == UpdateStatus::UpdateFailed) {
            showError("Update Failed");
        }
    }

    static void pollNetworkStatus() {
        const auto& networkStatus = NetworkManager::getNetworkStatus();

        DisplayLock lock;
        statusBarState.wifiStrength = networkStatus.wifiStrength;
    }

    static void pollMqttStatus() {
        const auto& mqttStatus = MqttManager::isConnected();

        DisplayLock lock;
        statusBarState.mqttConnected = mqttStatus;
    }

    void renderLoop() {
        pollUpdateProgress();
        pollNetworkStatus();
        pollMqttStatus();

        DisplayState state;
        {
            DisplayLock lock;
            state = displayState;
        }  // mutex released immediately

        switch (state.screen) {
            case DisplayScreen::LoadingProgress:
                renderLoadingProgress(state.message, state.progress);
                break;

            case DisplayScreen::LoadingSpinner:
                renderLoadingSpinner(state.message, state.spinnerStyle);
                break;

            case DisplayScreen::Error:
                renderError(state.message);
                break;

            case DisplayScreen::Success:
                renderSuccess(state.message);
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(state.screen == DisplayScreen::LoadingSpinner ||
                                         state.screen == DisplayScreen::LoadingProgress
                                     ? 33
                                     : 100));
    }
}  // namespace DisplayManager