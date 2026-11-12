#include "WebSocket.h"
#include <Common/Logger.h>

namespace WebSocket {

    PsychicWebSocketHandler websocket;

    void setup(PsychicHttpServer& server) {
        websocket.onOpen([](PsychicWebSocketClient* client) {
            Serial.printf("WebSocket client #%u connected\n", client->socket());
        });

        websocket.onClose([](PsychicWebSocketClient* client) {
            Serial.printf("WebSocket client #%u disconnected\n", client->socket());
        });

        websocket.onFrame(
            [](PsychicWebSocketRequest* request, httpd_ws_frame_t* frame) -> esp_err_t {
                if (frame->type == HTTPD_WS_TYPE_TEXT && frame->len == 8 &&
                    memcmp(frame->payload, "__init__", 8) == 0) {
                    return ESP_OK;
                }

                return request->reply(frame);
            });

        server.on("/api/ws", &websocket);

        // Send Logger output to this websocket
        Log.begin(&websocket);

        Log.println("WebSocket registered");
    }

    void loop() {
        // Nothing to do.
    }

}  // namespace WebSocket