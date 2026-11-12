#include "Logger.h"

Logger Log;

void Logger::begin(PsychicWebSocketHandler* websocket) {
    this->websocket = websocket;
    buffer.reserve(256);
}

void Logger::processChar(char c) {
    switch (c) {
        case '\r':
            flushWebSocket();
            lastWasCarriageReturn = true;
            break;

        case '\n':
            if (!lastWasCarriageReturn) {
                flushWebSocket();
            }

            lastWasCarriageReturn = false;
            break;

        default:
            lastWasCarriageReturn = false;
            buffer += c;
            break;
    }
}

size_t Logger::write(uint8_t c) {
    Serial.write(c);

    processChar(static_cast<char>(c));

    return 1;
}

size_t Logger::write(const uint8_t* data, size_t size) {
    Serial.write(data, size);

    for (size_t i = 0; i < size; ++i) {
        processChar(static_cast<char>(data[i]));
    }

    return size;
}

void Logger::flushWebSocket() {
    if (websocket && !buffer.isEmpty() && !websocket->getClientList().empty()) {
        websocket->sendAll(buffer.c_str());
    }

    buffer.clear();
}