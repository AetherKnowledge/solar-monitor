#include "Logger.h"

Logger Log;

void Logger::begin(AsyncWebSocket* websocket) {
    this->websocket = websocket;
}

size_t Logger::write(uint8_t c) {
    Serial.write(c);

    buffer += static_cast<char>(c);

    if (c == '\n') {
        flushWebSocket();
    }

    return 1;
}

size_t Logger::write(const uint8_t* data, size_t size) {
    Serial.write(data, size);

    for (size_t i = 0; i < size; i++) {
        buffer += static_cast<char>(data[i]);

        if (data[i] == '\n') {
            flushWebSocket();
        }
    }

    return size;
}

void Logger::flushWebSocket() {
    if (websocket && !buffer.isEmpty()) {
        websocket->textAll(buffer);
    }

    buffer.clear();
}