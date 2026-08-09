#include "Auth.h"
#include <Common/Network.h>
#include <Config/ConfigManager.h>
#include "SecretKey.h"

namespace WebServer {
    esp_err_t Middleware::run(PsychicRequest* request,
                              PsychicResponse* response,
                              PsychicMiddlewareNext next) {
        // Allow unauthenticated endpoints
        String path = request->path();

        if (path == "/api/auth/login" || path == "/login" || path == "/favicon.ico" ||
            path == "/api/auth/validate" || path.startsWith("/_app") || path == "/roboto.woff2") {
            return next();
        }

        const String& token = request->getCookie(SESSION_COOKIE_NAME);

        if (!validateSession(token)) {
            if (path.startsWith("/api/")) {
                return Response::error(response, "Unauthorized", 401);
            }
            return response->redirect("/login");
        }

        return next();
    }

    bool validateSession(const String& token) {
        Token sessionToken;
        if (!sessionToken.deserialize(token)) {
            return false;
        }

        return sessionToken.verify(Secret::KEY, sizeof(Secret::KEY));
    }

    const String generateSessionToken(const String& password) {
        if (password != ConfigManager::config.site.password)
            return "";

        Token token;

        token.payload.generate(WebServer::COOKIE_MAX_AGE);

        char payloadJson[128];

        if (!token.payload.serialize(payloadJson, sizeof(payloadJson)))
            return "";

        if (!Hmac::sha256(Secret::KEY,
                          sizeof(Secret::KEY),
                          reinterpret_cast<const uint8_t*>(payloadJson),
                          strlen(payloadJson),
                          token.signature)) {
            return "";
        }

        return token.serialize();
    }

}  // namespace WebServer