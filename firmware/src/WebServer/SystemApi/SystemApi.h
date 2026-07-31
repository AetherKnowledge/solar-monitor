#pragma once

#include <PsychicHttp.h>

namespace SystemApi {
    void registerRoutes(PsychicHttpServer& server);
    esp_err_t handleRestartRequest(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleGetSystemData(PsychicRequest* request, PsychicResponse* response);
}  // namespace SystemApi