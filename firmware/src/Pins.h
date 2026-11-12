#pragma once

#include <Arduino.h>

namespace Pins {

#ifdef CONFIG_IDF_TARGET_ESP32S3

    // ESP32-S3 pins to avoid:
    // 19,20  - Native USB
    // 35-37  - Internal Flash/PSRAM
    // 43,44  - UART0
    // 0,3,45,46 - Boot strapping

    // UART
    constexpr gpio_num_t UART1_RX = GPIO_NUM_15;
    constexpr gpio_num_t UART1_TX = GPIO_NUM_16;
    constexpr gpio_num_t UART2_RX = GPIO_NUM_17;
    constexpr gpio_num_t UART2_TX = GPIO_NUM_18;

    // I2C
    constexpr gpio_num_t I2C_SDA = GPIO_NUM_38;
    constexpr gpio_num_t I2C_SCL = GPIO_NUM_39;

    // Inputs
    constexpr gpio_num_t PREVIOUS_BUTTON = GPIO_NUM_10;
    constexpr gpio_num_t NEXT_BUTTON = GPIO_NUM_11;

    // Outputs
    constexpr gpio_num_t STATUS_LED = GPIO_NUM_6;
    constexpr gpio_num_t BUZZER = GPIO_NUM_40;

#else  // Original ESP32

    // UART
    constexpr gpio_num_t UART1_RX = GPIO_NUM_16;
    constexpr gpio_num_t UART1_TX = GPIO_NUM_17;
    constexpr gpio_num_t UART2_RX = GPIO_NUM_18;
    constexpr gpio_num_t UART2_TX = GPIO_NUM_19;

    // I2C
    constexpr gpio_num_t I2C_SDA = GPIO_NUM_25;
    constexpr gpio_num_t I2C_SCL = GPIO_NUM_26;

    // Inputs
    constexpr gpio_num_t PREVIOUS_BUTTON = GPIO_NUM_27;
    constexpr gpio_num_t NEXT_BUTTON = GPIO_NUM_14;

    // Outputs
    constexpr gpio_num_t STATUS_LED = GPIO_NUM_32;
    constexpr gpio_num_t BUZZER = GPIO_NUM_33;

#endif

}  // namespace Pins