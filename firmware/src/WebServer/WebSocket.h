#pragma once

#include <Arduino.h>
#include <PsychicHttp.h>

namespace WebSocket {
    void setup(PsychicHttpServer& server);
    void loop();
}  // namespace WebSocket