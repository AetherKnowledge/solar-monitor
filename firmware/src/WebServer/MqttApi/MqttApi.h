#pragma once

#include <Config/ConfigManager.h>
#include <PsychicHttp.h>

namespace MqttApi {
    void registerRoutes(PsychicHttpServer& server);
    esp_err_t handleGetConfig(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleUpdateConfig(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json);
}  // namespace MqttApi