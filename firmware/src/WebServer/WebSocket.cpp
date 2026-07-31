#include "WebSocket.h"
#include <Common/Logger.h>

namespace WebSocket {

    PsychicWebSocketHandler websocket;

    void setup(PsychicHttpServer& server) {
        websocket.onOpen([](PsychicWebSocketClient* client) {
            Log.printf("WebSocket client #%u connected\n", client->socket());
        });

        websocket.onClose([](PsychicWebSocketClient* client) {
            Log.printf("WebSocket client #%u disconnected\n", client->socket());
        });

        websocket.onFrame(
            [](PsychicWebSocketRequest* request, httpd_ws_frame_t* frame) -> esp_err_t {
                // Handle incoming messages here if needed.

                // If you don't care about incoming messages:
                return ESP_OK;

                // Or echo them back:
                // return request->reply(frame);
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