#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace UpdateApi {
    void registerApi(AsyncWebServer& server);
}  // namespace UpdateApi