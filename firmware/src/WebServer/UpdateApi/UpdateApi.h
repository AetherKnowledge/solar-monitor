#pragma once

#include <Arduino.h>
#include <PsychicHttp.h>

namespace UpdateApi {
    void registerApi(PsychicHttpServer& server);
    esp_err_t handleGetVersion(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleLatestUpdate(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json,
                                 bool isFirmware);
    esp_err_t handleGetStatus(PsychicRequest* request, PsychicResponse* response);

}  // namespace UpdateApi