#pragma once

#include <Arduino.h>
#include <string>

namespace WebServer {
    extern std::string WEBSITE_VERSION;

    bool start();
    bool stop();
    void registerApis();
}  // namespace WebServer