#include "UpdateHandler.h"
#include <Common/Network.h>
#include <System/SystemManager.h>
#include <Common/Logger.h>

namespace UpdateHandler {
    struct UpdateState {
        bool failed = false;
    };

    void onFirmwareUpdateFinish(AsyncWebServerRequest* request) {
        if (Update.hasError()) {
            Response::error(request, 500, "Firmware update failed");
            return;
        }

        SystemManager::requestRestart = true;
        Log.println("Firmware update successful");

        Response::success(request, 200, "Firmware update successful");
    }
    void onFirmwareUpdate(
        AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        // First chunk
        if (index == 0) {
            auto* state = new UpdateState();
            request->_tempObject = state;

            Log.println("Starting firmware update");
            Log.printf("Firmware size: %u bytes\n", total);

            if (!Update.begin(total, U_FLASH)) {
                Log.println("Failed to begin firmware update");
                Update.printError(Log);
                return;
            }
        }

        // Write this chunk
        if (Update.write(data, len) != len) {
            Log.println("Failed to write firmware chunk");

            Update.printError(Log);
            return;
        }

        Log.printf("Firmware: %u / %u bytes\r", index + len, total);

        // Last chunk
        if (index + len == total) {
            Log.println();

            if (!Update.end(true)) {
                Log.println("Failed to finish firmware update");
                Update.printError(Log);
                return;
            }

            Log.println("Firmware written successfully");
        }
    }

    void onWebsiteUpdateFinish(AsyncWebServerRequest* request) {
        if (Update.hasError()) {
            Response::error(request, 500, "Website update failed");
            return;
        }

        Log.println("Website update successful");

        Response::success(request, 200, "Website update successful");

        // Restart after the response has been sent
        SystemManager::requestRestart = true;
    }

    void onWebsiteUpdate(
        AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        // First chunk
        if (index == 0) {
            auto* state = new UpdateState();
            request->_tempObject = state;

            Log.println("Starting website update");
            Log.printf("Website size: %u bytes\n", total);

            /*
             * U_SPIFFS tells Update that we're updating
             * the filesystem/data partition rather than
             * an application partition.
             */
            if (!Update.begin(total, U_SPIFFS)) {
                Log.println("Failed to begin website update");

                Update.printError(Log);

                return;
            }
        }

        // Write current chunk
        if (Update.write(data, len) != len) {
            Log.println("Failed to write website chunk");

            Update.printError(Log);

            return;
        }

        Log.printf("Website: %u / %u bytes\r", index + len, total);

        // Final chunk
        if (index + len == total) {
            Log.println();

            if (!Update.end(true)) {
                Log.println("Failed to finish website update");

                Update.printError(Log);

                return;
            }

            Log.println("Website written successfully");
        }
    }
}  // namespace UpdateHandler