#include "Hmac.h"

#include <mbedtls/md.h>

namespace Hmac {

    bool sha256(const uint8_t* key,
                size_t keyLength,
                const uint8_t* data,
                size_t dataLength,
                uint8_t output[SHA256_SIZE]) {
        const mbedtls_md_info_t* info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

        if (info == nullptr)
            return false;

        return mbedtls_md_hmac(info, key, keyLength, data, dataLength, output) == 0;
    }

    bool compare(const uint8_t* a, const uint8_t* b, size_t length) {
        uint8_t diff = 0;

        for (size_t i = 0; i < length; ++i) diff |= a[i] ^ b[i];

        return diff == 0;
    }

}  // namespace Hmac