#include "UpdateHandler.h"
#include <Common/Network.h>
#include <System/SystemManager.h>
#include <Common/Logger.h>
#include <Common/UpdateStatus.h>

namespace UpdateHandler {
    UpdateProgress updateProgress = {UpdateStatus::NotStarted, 0, false};

    void onUpdateFinish(AsyncWebServerRequest* request, bool isFirmware) {
        const String name = isFirmware ? "Firmware" : "Website";

        if (!Update.hasError() && Update.isFinished() &&
            updateProgress.status == UpdateStatus::UpdateComplete) {
            updateProgress.status = UpdateStatus::NotStarted;
            SystemManager::requestRestart();

            String completeMessage = name + " update complete. Restarting...";

            Response::success(request, 200, completeMessage, &Log);
            return;
        }

        updateProgress.status = UpdateStatus::NotStarted;
        String errorMessage = name + " update failed. Please check the logs for more information.";
        Response::error(request, 500, errorMessage, &Log);
    }

    void onChunk(uint8_t* data,
                 size_t len,
                 size_t index,
                 bool final,
                 std::optional<size_t> total,
                 bool isFirmware) {
        const char* name = isFirmware ? "Firmware" : "Website";
        const auto partition = isFirmware ? U_FLASH : U_SPIFFS;

        if (updateProgress.status == UpdateStatus::UpdateFailed) {
            return;
        }

        // First chunk
        if (index == 0) {
            if (updateProgress.status != UpdateStatus::NotStarted) {
                Log.println("Update already in progress");
                return;
            }

            updateProgress.status = UpdateStatus::Requested;
            Log.printf("Starting %s update\n", name);
            if (total) {
                Log.printf("%s size: %u bytes\n", name, total.value());
            }

            /*
             * U_SPIFFS tells Update that we're updating
             * the filesystem/data partition rather than
             * an application partition.
             */

            bool hasStarted = total ? Update.begin(*total, partition)
                                    : Update.begin(UPDATE_SIZE_UNKNOWN, partition);

            if (!hasStarted) {
                updateProgress.status = UpdateStatus::UpdateFailed;

                Log.println("Failed to begin " + String(name) + " update");
                Update.printError(Log);
                return;
            }
        }

        // Write current chunk
        if (Update.write(data, len) != len) {
            updateProgress.status = UpdateStatus::UpdateFailed;

            Log.println("Failed to write " + String(name) + " chunk");
            Update.printError(Log);
            return;
        }

        if (updateProgress.status != UpdateStatus::InProgress) {
            updateProgress.status = UpdateStatus::InProgress;
        }

        if (total) {
            Log.printf("%s: %u / %u bytes\r", name, index + len, total.value());
        } else {
            Log.printf("%s: %u bytes\r", name, index + len);
        }

        // Final chunk
        if (final) {
            Log.println();

            if (!Update.end(true)) {
                updateProgress.status = UpdateStatus::UpdateFailed;

                Log.println("Failed to finish " + String(name) + " update");
                Update.printError(Log);
                return;
            }

            updateProgress.status = UpdateStatus::UpdateComplete;
            Log.println(String(name) + " written successfully");
        }
    }

    const UpdateProgress& getUpdateProgress() {
        return updateProgress;
    }
}  // namespace UpdateHandler