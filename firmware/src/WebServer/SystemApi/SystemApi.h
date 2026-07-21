#pragma once

#include <ESPAsyncWebServer.h>

namespace SystemApi {
    void registerApi(AsyncWebServer& server);
    void handleRestartRequest(AsyncWebServerRequest* request);
}  // namespace SystemApi