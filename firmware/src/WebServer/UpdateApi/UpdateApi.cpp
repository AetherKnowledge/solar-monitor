#include "UpdateApi.h"
#include <Common/Network.h>
#include <Update.h>
#include <System/SystemManager.h>

namespace UpdateApi {
    void registerApi(AsyncWebServer& server) {
        server.on(
            "/api/update/firmware",
            HTTP_POST,

            // Called when request finishes
            [](AsyncWebServerRequest* request) {
                if (Update.hasError()) {
                    Response::error(request, 500, "Firmware update failed");
                    return;
                }

                SystemManager::requestRestart = true;
                Serial.println("Firmware update successful");

                Response::success(request, 200, "Firmware update successful");
            },

            // Upload callback - unused when sending raw octet-stream
            nullptr,

            // Body callback
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) {
                // First chunk
                if (index == 0) {
                    Serial.println("Starting firmware update");
                    Serial.printf("Firmware size: %u bytes\n", total);

                    if (!Update.begin(total, U_FLASH)) {
                        Serial.println("Failed to begin firmware update");
                        Update.printError(Serial);
                        return;
                    }
                }

                // Write this chunk
                if (Update.write(data, len) != len) {
                    Serial.println("Failed to write firmware chunk");
                    Update.printError(Serial);
                    return;
                }

                Serial.printf("Firmware: %u / %u bytes\r", index + len, total);

                // Last chunk
                if (index + len == total) {
                    Serial.println();

                    if (!Update.end(true)) {
                        Serial.println("Failed to finish firmware update");
                        Update.printError(Serial);
                        return;
                    }

                    Serial.println("Firmware written successfully");
                }
            });

        server.addHandler(new AsyncCallbackJsonWebHandler(
            "/api/update/test", [](AsyncWebServerRequest* request, JsonVariant& json) {
                Response::success(request, 200, "Test successful");
            }));

        // server.addHandler(new AsyncCallbackJsonWebHandler(
        //     "/api/update/website", [](AsyncWebServerRequest* request, JsonVariant& json) {
        //         handlePostWebsite(request, json);
        //     }));

        Serial.println("Update API registered");
    }
}  // namespace UpdateApi