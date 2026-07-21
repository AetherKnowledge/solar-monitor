#include "UpdateApi.h"
#include <Common/Network.h>
#include <Update.h>
#include <System/SystemManager.h>
#include <Version.h>
#include <WebServer/WebServer.h>

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

        server.on(
            "/api/update/website",
            HTTP_POST,

            // Called when request finishes
            [](AsyncWebServerRequest* request) {
                if (Update.hasError()) {
                    Response::error(request, 500, "Website update failed");
                    return;
                }

                Serial.println("Website update successful");

                Response::success(request, 200, "Website update successful");

                // Restart after the response has been sent
                SystemManager::requestRestart = true;
            },

            // Upload callback
            nullptr,

            // Raw body callback
            [](AsyncWebServerRequest* request,
               uint8_t* data,
               size_t len,
               size_t index,
               size_t total) {
                // First chunk
                if (index == 0) {
                    Serial.println("Starting website update");

                    Serial.printf("Website size: %u bytes\n", total);

                    /*
                     * U_SPIFFS tells Update that we're updating
                     * the filesystem/data partition rather than
                     * an application partition.
                     */
                    if (!Update.begin(total, U_SPIFFS)) {
                        Serial.println("Failed to begin website update");

                        Update.printError(Serial);

                        return;
                    }
                }

                // Write current chunk
                if (Update.write(data, len) != len) {
                    Serial.println("Failed to write website chunk");

                    Update.printError(Serial);

                    return;
                }

                Serial.printf("Website: %u / %u bytes\r", index + len, total);

                // Final chunk
                if (index + len == total) {
                    Serial.println();

                    if (!Update.end(true)) {
                        Serial.println("Failed to finish website update");

                        Update.printError(Serial);

                        return;
                    }

                    Serial.println("Website written successfully");
                }
            });

        server.on("/api/version", HTTP_GET, [](AsyncWebServerRequest* request) {
            handleGetVersion(request);
        });

        Serial.println("Update API registered");
    }

    void handleGetVersion(AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["firmware"] = Version::FIRMWARE;
        doc["website"] = WebServer::WEBSITE_VERSION;

        String response;
        serializeJson(doc, response);

        Serial.println("Version: " + response);

        request->send(200, "application/json", response);
    }

}  // namespace UpdateApi