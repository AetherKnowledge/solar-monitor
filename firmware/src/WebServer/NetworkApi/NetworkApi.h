#pragma once

#include <PsychicHttp.h>

namespace NetworkApi {
    void registerApi(PsychicHttpServer& server);
    esp_err_t handleGetNetworks(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleScanNetworks(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleUpdateConfig(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json);
}  // namespace NetworkApi