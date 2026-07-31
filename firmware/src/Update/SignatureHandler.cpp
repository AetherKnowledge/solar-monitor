#include "UpdatePublicKey.h"

#include <mbedtls/base64.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <Common/Logger.h>

namespace UpdateHandler {
    bool verifySignature(const String& payload, const String& signatureBase64) {
        bool success = false;

        // -------------------------------------------------------------------------
        // Decode Base64 signature
        // -------------------------------------------------------------------------

        Log.printf("Verifying signature for payload:\n%s\n with signature:\n%s\n",
                   payload.c_str(),
                   signatureBase64.c_str());

        uint8_t signature[128];
        size_t signatureLength = 0;

        int ret = mbedtls_base64_decode(signature,
                                        sizeof(signature),
                                        &signatureLength,
                                        reinterpret_cast<const uint8_t*>(signatureBase64.c_str()),
                                        signatureBase64.length());

        if (ret != 0) {
            Log.printf("Failed to decode Base64 signature (%d)\n", ret);
            return false;
        }

        // -------------------------------------------------------------------------
        // Compute SHA-256(payload)
        // -------------------------------------------------------------------------

        uint8_t hash[32];

        ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                         reinterpret_cast<const uint8_t*>(payload.c_str()),
                         payload.length(),
                         hash);

        if (ret != 0) {
            Log.printf("Failed to compute SHA-256 (%d)\n", ret);
            return false;
        }

        // -------------------------------------------------------------------------
        // Parse public key
        // -------------------------------------------------------------------------

        mbedtls_pk_context pk;
        mbedtls_pk_init(&pk);

        do {
            ret = mbedtls_pk_parse_public_key(
                &pk,
                reinterpret_cast<const unsigned char*>(UpdateHandler::PUBLIC_KEY),
                strlen(UpdateHandler::PUBLIC_KEY) + 1);

            if (ret != 0) {
                Log.printf("Failed to parse public key (%d)\n", ret);
                break;
            }

            // ---------------------------------------------------------------------
            // Verify signature
            // ---------------------------------------------------------------------

            ret = mbedtls_pk_verify(
                &pk, MBEDTLS_MD_SHA256, hash, sizeof(hash), signature, signatureLength);

            if (ret != 0) {
                Log.printf("Signature verification failed (%d)\n", ret);
                break;
            }

            success = true;

        } while (false);

        mbedtls_pk_free(&pk);

        return success;
    }
}  // namespace UpdateHandler