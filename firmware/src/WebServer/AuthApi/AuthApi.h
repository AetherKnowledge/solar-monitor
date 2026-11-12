#pragma once

#include <PsychicHttp.h>

namespace AuthApi {
    void registerRoutes(PsychicHttpServer& server);

    esp_err_t handleLogin(PsychicRequest* request, PsychicResponse* response, JsonVariant& body);
    esp_err_t handleLogout(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleValidate(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleUpdateConfig(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json);
}  // namespace AuthApi