#pragma once

#include <PsychicHttp.h>
#include <Common/Base64.h>
#include <Common/Hmac.h>

namespace WebServer {

    static constexpr unsigned long COOKIE_MAX_AGE = 3600;

    struct TokenPayload {
        uint8_t version = 1;
        uint64_t issuedAt = 0;
        uint64_t expiresAt = 0;
        uint8_t nonce[16];

        bool generate(uint64_t lifetimeSeconds) {
            timeval tv;
            gettimeofday(&tv, nullptr);

            issuedAt = tv.tv_sec;
            expiresAt = issuedAt + lifetimeSeconds;

            esp_fill_random(nonce, sizeof(nonce));

            return true;
        }

        bool serialize(char* buffer, size_t bufferSize) const {
            JsonDocument doc;

            doc["v"] = version;
            doc["iat"] = issuedAt;
            doc["exp"] = expiresAt;

            char nonce[32];
            Base64::encodeUrl(this->nonce, sizeof(this->nonce), nonce, sizeof(nonce));

            doc["n"] = nonce;

            return serializeJson(doc, buffer, bufferSize) > 0;
        }

        bool deserialize(const char* json) {
            JsonDocument doc;

            if (deserializeJson(doc, json))
                return false;

            version = doc["v"] | 1;
            issuedAt = doc["iat"] | 0;
            expiresAt = doc["exp"] | 0;

            size_t decoded;

            return Base64::decodeUrl(doc["n"] | "", nonce, sizeof(nonce), decoded);
        }
    };

    struct Token {
        TokenPayload payload;
        uint8_t signature[32];

        String serialize() const {
            char payloadJson[128];

            if (!payload.serialize(payloadJson, sizeof(payloadJson)))
                return "";

            char payloadEncoded[172];
            char signatureEncoded[48];

            Base64::encodeUrl(reinterpret_cast<const uint8_t*>(payloadJson),
                              strlen(payloadJson),
                              payloadEncoded,
                              sizeof(payloadEncoded));

            Base64::encodeUrl(
                signature, sizeof(signature), signatureEncoded, sizeof(signatureEncoded));

            String token;
            token.reserve(strlen(payloadEncoded) + strlen(signatureEncoded) + 2);

            token += payloadEncoded;
            token += '.';
            token += signatureEncoded;

            return token;
        }

        bool deserialize(const String& token) {
            int dot = token.indexOf('.');

            if (dot < 0)
                return false;

            String payloadEncoded = token.substring(0, dot);
            String signatureEncoded = token.substring(dot + 1);

            char payloadJson[128];
            size_t payloadLength;

            if (!Base64::decodeUrl(payloadEncoded.c_str(),
                                   reinterpret_cast<uint8_t*>(payloadJson),
                                   sizeof(payloadJson) - 1,
                                   payloadLength))
                return false;

            payloadJson[payloadLength] = '\0';

            if (!payload.deserialize(payloadJson))
                return false;

            size_t signatureLength;

            return Base64::decodeUrl(
                signatureEncoded.c_str(), signature, sizeof(signature), signatureLength);
        }

        bool verify(const uint8_t* secret, size_t secretLength) const {
            char payloadJson[128];

            if (!payload.serialize(payloadJson, sizeof(payloadJson)))
                return false;

            uint8_t expectedSignature[Hmac::SHA256_SIZE];

            if (!Hmac::sha256(secret,
                              secretLength,
                              reinterpret_cast<const uint8_t*>(payloadJson),
                              strlen(payloadJson),
                              expectedSignature)) {
                return false;
            }

            if (!Hmac::compare(expectedSignature, signature, Hmac::SHA256_SIZE)) {
                return false;
            }

            timeval tv;
            gettimeofday(&tv, nullptr);

            if (payload.expiresAt < static_cast<uint64_t>(tv.tv_sec))
                return false;

            if (payload.version != 1)
                return false;

            return true;
        }
    };

    class Middleware : public PsychicMiddleware {
       public:
        inline static const char* SESSION_COOKIE_NAME = "session";

        esp_err_t run(PsychicRequest* request,
                      PsychicResponse* response,
                      PsychicMiddlewareNext next) override;
    };

    bool validateSession(const String& token);
    const String generateSessionToken(const String& password);

}  // namespace WebServer