#pragma once

#include <Arduino.h>
#include <PsychicHttp.h>

class Logger : public Print {
   public:
    void begin(PsychicWebSocketHandler* websocket);

    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t size) override;

   private:
    PsychicWebSocketHandler* websocket = nullptr;
    String buffer;

    bool lastWasCarriageReturn = false;

    void processChar(char c);
    void flushWebSocket();
};

extern Logger Log;