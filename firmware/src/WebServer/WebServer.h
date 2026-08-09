#pragma once

#include <Arduino.h>
#include <PsychicHttp.h>
#include "Auth.h"

namespace WebServer {
    extern String WEBSITE_VERSION;
    extern Middleware auth;

    bool start();
    bool stop();
    void registerApis();
    size_t getUsedBytes();
    size_t getTotalBytes();
}  // namespace WebServer