#include "AuthApi.h"
#include "WebServer/WebServer.h"
#include "esp_err.h"
#include <Config/ConfigManager.h>
#include <Common/Network.h>
#include <WebServer/Auth.h>
#include <sys/time.h>
#include <System/SystemManager.h>

namespace AuthApi {
    void registerRoutes(PsychicHttpServer& server) {
        server.on("/api/auth/login", HTTP_POST, handleLogin);
        server.on("/api/auth/validate", HTTP_POST, handleValidate);
        server.on("/api/auth/logout", HTTP_POST, handleLogout);
        server.on("/api/auth/config", HTTP_GET, handleGetConfig);
        server.on("/api/auth/config", HTTP_POST, handleUpdateConfig);
    }

    esp_err_t handleLogin(PsychicRequest* request, PsychicResponse* response, JsonVariant& body) {
        if (!ConfigManager::config.site.passwordEnabled ||
            ConfigManager::config.site.password.isEmpty()) {
            return Response::error(response, "Authentication is disabled", 403);
        }

        String password = body["password"] | "";

        if (password.isEmpty()) {
            return Response::error(response, "Missing credentials");
        }

        const String& token = WebServer::generateSessionToken(password);

        if (token.isEmpty()) {
            return Response::error(response, "Invalid credentials", 401);
        }

        response->setCookie(WebServer::Middleware::SESSION_COOKIE_NAME,
                            token.c_str(),
                            WebServer::COOKIE_MAX_AGE,

                            // Allow the session cookie for normal navigation while protecting
                            // against most cross-site request forgery (CSRF) attacks.
                            "HttpOnly; SameSite=Lax; Path=/");

        return Response::success(response, "Login successful");
    }

    esp_err_t handleLogout(PsychicRequest* request, PsychicResponse* response) {
        response->setCookie(WebServer::Middleware::SESSION_COOKIE_NAME,
                            "",
                            0,

                            // Allow the session cookie for normal navigation while protecting
                            // against most cross-site request forgery (CSRF) attacks.
                            "HttpOnly; SameSite=Lax; Path=/");

        return Response::success(response, "Logout successful");
    }

    esp_err_t handleValidate(PsychicRequest* request, PsychicResponse* response) {
        if (!ConfigManager::config.site.passwordEnabled ||
            ConfigManager::config.site.password.isEmpty()) {
            return Response::success(response, "No authentication required");
        }

        const String& token = request->getCookie(WebServer::Middleware::SESSION_COOKIE_NAME);

        if (token.isEmpty()) {
            return Response::error(response, "No session found", 401);
        }

        if (!WebServer::validateSession(token)) {
            return Response::error(response, "Invalid session", 401);
        }

        return Response::success(response, "Session is valid");
    }

    esp_err_t handleGetSystemData(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        SystemManager::systemDevice.toJson(doc);

        return Response::sendJson(response, doc);
    }

    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response) {
        JsonDocument doc;
        ConfigManager::config.site.toJson(doc.to<JsonObject>());

        return Response::sendJson(response, doc);
    }

    esp_err_t handleUpdateConfig(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json) {
        SystemManager::requestUpdate(json);
        return Response::success(response, "OK", 202);
    }

}  // namespace AuthApi