#pragma once

#include <Arduino.h>
#include <string>

namespace WebServer {
    extern String WEBSITE_VERSION;

    bool start();
    bool stop();
    void registerApis();
    size_t getUsedBytes();
    size_t getTotalBytes();
}  // namespace WebServer