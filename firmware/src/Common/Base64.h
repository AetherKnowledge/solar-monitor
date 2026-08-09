#pragma once

#include <Arduino.h>

namespace Base64 {

    bool encode(const uint8_t* input, size_t inputLength, char* output, size_t outputSize);

    bool decode(const char* input, uint8_t* output, size_t outputSize, size_t& outputLength);

    bool encodeUrl(const uint8_t* input, size_t inputLength, char* output, size_t outputSize);

    bool decodeUrl(const char* input, uint8_t* output, size_t outputSize, size_t& outputLength);

}  // namespace Base64