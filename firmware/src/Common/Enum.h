#pragma once

#include <Arduino.h>

namespace Enum
{
    template <typename T>
    const char *toString(T value);

    template <typename T>
    inline T fromString(const char *value);
}