#pragma once

#include <Arduino.h>
#include <PsychicHttp.h>

namespace ModbusApi {
    void registerRoutes(PsychicHttpServer& server);
    esp_err_t handleGetDevices(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleGetDevice(PsychicRequest* request, PsychicResponse* response, String id);
    esp_err_t handleUpdateDevice(PsychicRequest* request,
                                 PsychicResponse* response,
                                 JsonVariant& json);
    esp_err_t handleGetValues(PsychicRequest* request, PsychicResponse* response);
    esp_err_t handleGetStatus(PsychicRequest* request, PsychicResponse* response);
}  // namespace ModbusApi