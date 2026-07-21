#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class Logger : public Print {
   public:
    void begin(AsyncWebSocket* websocket);

    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t size) override;

   private:
    AsyncWebSocket* websocket = nullptr;
    String buffer;

    void flushWebSocket();
};

extern Logger Log;