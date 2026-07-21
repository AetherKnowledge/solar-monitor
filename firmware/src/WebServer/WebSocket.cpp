#include "WebSocket.h"
#include <Common/Logger.h>

namespace WebSocket {
    AsyncWebSocket websocket("/api/ws");

    void setup(AsyncWebServer& server) {
        websocket.onEvent([](AsyncWebSocket* server,
                             AsyncWebSocketClient* client,
                             AwsEventType type,
                             void* arg,
                             uint8_t* data,
                             size_t len) {
            switch (type) {
                case WS_EVT_CONNECT:
                    Log.printf("WebSocket client #%u connected\n", client->id());
                    break;

                case WS_EVT_DISCONNECT:
                    Log.printf("WebSocket client #%u disconnected\n", client->id());
                    break;

                case WS_EVT_DATA:
                    // Handle messages from the browser here if needed
                    break;

                case WS_EVT_PONG:
                case WS_EVT_ERROR:
                    break;
            }
        });

        server.addHandler(&websocket);

        // Send Logger output to this websocket
        Log.begin(&websocket);

        Log.println("WebSocket registered");
    }

    void loop() {
        websocket.cleanupClients();
    }
}  // namespace WebSocket