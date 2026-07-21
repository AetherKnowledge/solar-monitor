#pragma once

#include <Arduino.h>

namespace Numbers {
    inline double applyRounding(double value, uint8_t decimals) {
        static constexpr double factors[] = {1.0, 10.0, 100.0, 1000.0};

        if (decimals >= std::size(factors))
            return value;

        return roundf(value * factors[decimals]) / factors[decimals];
    }
}  // namespace Numbers