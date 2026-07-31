#pragma once

#include <Arduino.h>

namespace UpdateHandler {
    bool verifySignature(const String& payload, const String& signature);
}  // namespace UpdateHandler
