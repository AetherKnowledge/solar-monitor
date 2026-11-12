#pragma once

#include <cstddef>
#include <cstdint>

namespace Hmac {

    constexpr size_t SHA256_SIZE = 32;

    bool sha256(const uint8_t* key,
                size_t keyLength,
                const uint8_t* data,
                size_t dataLength,
                uint8_t output[SHA256_SIZE]);

    bool compare(const uint8_t* a, const uint8_t* b, size_t length);

}  // namespace Hmac