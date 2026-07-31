#pragma once

#include <Arduino.h>
#include <PsychicHttp.h>

namespace ConfigApi {
    void registerRoutes(PsychicHttpServer& server);
    esp_err_t handleUploadConfig(PsychicRequest* request, JsonVariant& json);
    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response);

    esp_err_t onUpload(PsychicRequest* request,
                       const String& filename,
                       uint64_t index,
                       uint8_t* data,
                       size_t len,
                       bool final);

    esp_err_t onUploadFinish(PsychicRequest* request, PsychicResponse* response);
    bool validateConfig();
}  // namespace ConfigApi
