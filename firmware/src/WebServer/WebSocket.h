#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace WebSocket {
    void setup(AsyncWebServer& server);
    void loop();
}  // namespace WebSocket