#include "Base64.h"

#include <mbedtls/base64.h>
#include <cstring>

namespace Base64 {

    bool encode(const uint8_t* input, size_t inputLength, char* output, size_t outputSize) {
        size_t written = 0;

        int ret = mbedtls_base64_encode(
            reinterpret_cast<unsigned char*>(output), outputSize, &written, input, inputLength);

        if (ret != 0)
            return false;

        if (written >= outputSize)
            return false;

        output[written] = '\0';

        return true;
    }

    bool decode(const char* input, uint8_t* output, size_t outputSize, size_t& outputLength) {
        return mbedtls_base64_decode(output,
                                     outputSize,
                                     &outputLength,
                                     reinterpret_cast<const unsigned char*>(input),
                                     strlen(input)) == 0;
    }

    bool encodeUrl(const uint8_t* input, size_t inputLength, char* output, size_t outputSize) {
        if (!encode(input, inputLength, output, outputSize))
            return false;

        for (char* p = output; *p; ++p) {
            switch (*p) {
                case '+':
                    *p = '-';
                    break;

                case '/':
                    *p = '_';
                    break;

                case '=':
                    *p = '\0';
                    return true;
            }
        }

        return true;
    }

    bool decodeUrl(const char* input, uint8_t* output, size_t outputSize, size_t& outputLength) {
        char buffer[256];

        size_t len = strlen(input);

        if (len >= sizeof(buffer))
            return false;

        memcpy(buffer, input, len);
        buffer[len] = '\0';

        for (size_t i = 0; i < len; ++i) {
            switch (buffer[i]) {
                case '-':
                    buffer[i] = '+';
                    break;

                case '_':
                    buffer[i] = '/';
                    break;
            }
        }

        while (len % 4) buffer[len++] = '=';

        buffer[len] = '\0';

        return decode(buffer, output, outputSize, outputLength);
    }

}  // namespace Base64