#include "DisplayError.h"
#include "DisplayManager.h"
#include "DisplayTools.h"
#include <Wire.h>
#include <U8g2lib.h>
#include <Networking/NetworkService.h>
#include <Update/UpdateHandler.h>
#include <Mqtt/MqttManager.h>
#include <Common/Logger.h>
#include <Modbus/ModbusManager.h>
#include <optional>
#include "Pins.h"

namespace DisplayManager {
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

    static void displayTask(void*) {
        TickType_t lastRender = xTaskGetTickCount();
        TickType_t lastInput = xTaskGetTickCount();

        while (true) {
            TickType_t now = xTaskGetTickCount();

            // Poll buttons every 10 ms
            if (now - lastInput >= pdMS_TO_TICKS(10)) {
                inputLoop();
                lastInput = now;
            }

            // Render at different rates depending on screen
            DisplayScreen screen;
            {
                DisplayLock lock;
                screen = displayState.screen;
            }

            TickType_t renderInterval = (screen == DisplayScreen::LoadingSpinner ||
                                         screen == DisplayScreen::LoadingProgress)
                                            ? pdMS_TO_TICKS(33)    // ~30 FPS
                                            : pdMS_TO_TICKS(100);  // 10 FPS

            if (now - lastRender >= renderInterval) {
                renderLoop();
                lastRender = now;
            }

            // Yield to other tasks
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    void setup() {
        pinMode(Pins::PREVIOUS_BUTTON, INPUT_PULLUP);
        pinMode(Pins::NEXT_BUTTON, INPUT_PULLUP);

        Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);

        display.setI2CAddress(I2C_ADDRESS << 1);
        display.begin();
        display.setDisplayRotation(U8G2_R2);
        display.setPowerSave(0);
        display.clearDisplay();
        display.sendBuffer();

        displayMutex = xSemaphoreCreateMutex();
        if (displayMutex == nullptr) {
            Log.println("Failed to create display mutex");
            return;
        }

        // xTaskCreatePinnedToCore(displayTask,  // Task function
        //                         "Display",    // Name
        //                         4096,         // Stack size
        //                         nullptr,      // Parameter
        //                         1,            // Priority
        //                         nullptr,      // Task handle
        //                         1             // Core (1 is usually best)
        // );
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

        String title = displayState.screen == DisplayScreen::DeviceInfo ? statusBarState.deviceName
                                                                        : statusBarState.title;
        if (statusBarStyle.showTitle) {
            if (statusBarStyle.centerTitle) {
                drawStr(0, 8, title.c_str(), TextAlignment::Center);

            } else {
                drawStr(statusBarStyle.padding, 8, title.c_str());
            }
        }

        //----------------------------------------
        // icons
        //----------------------------------------

        int x = display.getDisplayWidth() - statusBarStyle.padding - 3;
        const auto mqttSize = getIconSize(Icon::MQTT);
        drawMqttIcon(x - mqttSize.width, 0, statusBarState.mqttConnected);
        x -= mqttSize.width + statusBarStyle.iconSpacing;

        const auto wifiSize = getIconSize(Icon::Wifi);
        drawWifiIcon(x - wifiSize.width, 0, statusBarState.wifiStrength);
        x -= wifiSize.width + statusBarStyle.iconSpacing;

        if (statusBarState.modbusVisible) {
            const auto modbusSize = getIconSize(Icon::Modbus);
            drawModbusIcon(x - modbusSize.width, 0, statusBarState.modbusConnected);
            x -= modbusSize.width + statusBarStyle.iconSpacing;
        }
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

    void showDeviceInfo() {
        DisplayLock lock;
        displayState.screen = DisplayScreen::DeviceInfo;
    }

    void updateDeviceInfo(const std::vector<DisplayData>& devices) {
        DisplayLock lock;
        displayState.displayDevices = devices;
    }

    void showNextDevice() {
        DisplayLock lock;

        const size_t count = displayState.displayDevices.size();
        if (count == 0)
            return;

        displayState.deviceInfoIndex = (displayState.deviceInfoIndex + 1) % count;

        displayState.screen = DisplayScreen::DeviceInfo;
    }

    void showPreviousDevice() {
        DisplayLock lock;

        const size_t count = displayState.displayDevices.size();
        if (count == 0)
            return;

        displayState.deviceInfoIndex = (displayState.deviceInfoIndex + count - 1) % count;

        displayState.screen = DisplayScreen::DeviceInfo;
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

    static std::optional<DisplayData> getCurrentDevice() {
        DisplayLock lock;

        const auto& devices = displayState.displayDevices;

        if (devices.empty())
            return std::nullopt;

        size_t index = displayState.deviceInfoIndex % devices.size();
        return devices[index];  // copy while locked
    }

    static void renderSensorInfo(const DisplayData::EntityData& entity) {
        constexpr uint8_t COLS = 2;
        constexpr uint8_t ROWS = 2;

        constexpr uint8_t TOP = 12;  // below status bar
        uint8_t CELL_W = display.getDisplayWidth() / COLS;
        uint8_t CELL_H = display.getDisplayHeight() / ROWS;

        uint8_t row = entity.index / COLS;
        uint8_t col = entity.index % COLS;

        if (entity.index >= COLS * ROWS) {
            return;
        }

        if (row >= ROWS)
            return;

        int x = col * CELL_W;
        int y = TOP + row * CELL_H;

        display.setFont(u8g2_font_5x8_tr);

        // Name
        drawStr(x, y + 6, entity.name.c_str(), TextAlignment::Center, x + CELL_W);

        // Value
        display.setFont(u8g2_font_6x10_tr);
        drawStr(x, y + 16, String(entity.value).c_str(), TextAlignment::Center, x + CELL_W);
    }

    static void renderDeviceInfo() {
        beginFrame();

        auto device = getCurrentDevice();

        display.setFont(u8g2_font_ncenB08_tr);
        if (!device) {
            drawStr(0, 20, "No device configured", TextAlignment::Center);
            endFrame();
            return;
        }

        const auto& deviceData = device.value();

        if (deviceData.entities.empty()) {
            drawStr(0, 20, "No display items", TextAlignment::Center);
            endFrame();
            return;
        }

        for (auto& entity : deviceData.entities) {
            renderSensorInfo(entity);
        }

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
        const auto& networkStatus = NetworkService::getNetworkStatus();

        DisplayLock lock;
        if (networkStatus.connected) {
            statusBarState.wifiStrength = networkStatus.wifiStrength;
        } else {
            statusBarState.wifiStrength = 0;
        }
    }

    static void pollMqttStatus() {
        const auto& mqttStatus = MqttManager::isConnected();

        DisplayLock lock;
        statusBarState.mqttConnected = mqttStatus;
    }

    static void pollCurrentDeviceStatus() {
        const auto deviceData = getCurrentDevice();

        if (deviceData.has_value()) {
            DisplayLock lock;
            statusBarState.modbusVisible = true;
            statusBarState.modbusConnected = deviceData->modbusConnected;
            statusBarState.deviceName = deviceData->deviceName;
        } else {
            DisplayLock lock;
            statusBarState.modbusVisible = false;
            statusBarState.modbusConnected = false;
            statusBarState.deviceName = "No Device";
        }
    }

    void inputLoop() {
        static bool prevNextButtonState = HIGH;
        static bool prevPrevButtonState = HIGH;

        static uint32_t lastNextPress = 0;
        static uint32_t lastPrevPress = 0;

        constexpr uint32_t DEBOUNCE_MS = 50;

        const uint32_t now = millis();

        bool nextButtonState = digitalRead(Pins::NEXT_BUTTON);
        bool prevButtonState = digitalRead(Pins::PREVIOUS_BUTTON);

        // Next button (falling edge)
        if (nextButtonState == LOW && prevNextButtonState == HIGH &&
            (now - lastNextPress) >= DEBOUNCE_MS) {
            lastNextPress = now;
            showNextDevice();
        }

        // Previous button (falling edge)
        if (prevButtonState == LOW && prevPrevButtonState == HIGH &&
            (now - lastPrevPress) >= DEBOUNCE_MS) {
            lastPrevPress = now;
            showPreviousDevice();
        }

        prevNextButtonState = nextButtonState;
        prevPrevButtonState = prevButtonState;
    }

    void renderLoop() {
        pollUpdateProgress();
        pollNetworkStatus();
        pollMqttStatus();
        pollCurrentDeviceStatus();

        DisplayState state;
        {
            DisplayLock lock;
            state = displayState;
        }

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

            case DisplayScreen::DeviceInfo:
                renderDeviceInfo();
                break;
        }
    }
}  // namespace DisplayManager