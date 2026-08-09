#include "UpdateHandler.h"
#include <Common/Network.h>
#include <System/SystemManager.h>
#include <Common/Logger.h>
#include <Common/UpdateStatus.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Networking/NetworkService.h>
#include <mbedtls/sha256.h>
#include <Version.h>
#include <WebServer/WebServer.h>
#include <Common/Logger.h>
#include "UpdatePublicKey.h"

namespace UpdateHandler {
    UpdateProgress updateProgress = {UpdateStatus::NotStarted, 0, false};
    UpdateRequest currentUpdateRequest = {"", 0, "", "", "0.0.0", "", false};

    TaskHandle_t updateTaskHandle = nullptr;

    void updateTask(void* parameter) {
        while (true) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            downloadAndInstall(currentUpdateRequest, currentUpdateRequest.isFirmware);

            currentUpdateRequest = {"", 0, "", "", "0.0.0", "", false};
        }
    }

    void setup() {
        if (updateTaskHandle == nullptr) {
            xTaskCreate(updateTask, "UpdateTask", 16384, nullptr, 1, &updateTaskHandle);
        }
    }

    esp_err_t onUpdateFinish(PsychicRequest* request, PsychicResponse* response, bool isFirmware) {
        const String name = isFirmware ? "Firmware" : "Website";

        if (!Update.hasError() && Update.isFinished() &&
            updateProgress.status == UpdateStatus::UpdateComplete) {
            updateProgress.status = UpdateStatus::NotStarted;
            SystemManager::requestRestart();

            String completeMessage = name + " update complete. Restarting...";

            return Response::success(response, completeMessage, 200, &Log);
        }

        updateProgress.status = UpdateStatus::NotStarted;
        String errorMessage = name + " update failed. Please check the logs for more information.";
        return Response::error(response, errorMessage, 500, &Log);
    }

    esp_err_t onChunk(PsychicRequest* request,
                      uint8_t* data,
                      size_t len,
                      uint64_t index,
                      bool final,
                      bool isFirmware) {
        const char* name = isFirmware ? "Firmware" : "Website";
        const auto partition = isFirmware ? U_FLASH : U_SPIFFS;

        std::optional<size_t> total;

        // Only basic uploads have a meaningful content length.
        if (index == 0 && !request->isMultipart()) {
            total = request->contentLength();
        }

        if (updateProgress.status == UpdateStatus::UpdateFailed) {
            return ESP_FAIL;
        }

        // First chunk
        if (index == 0) {
            if (updateProgress.status == UpdateStatus::InProgress ||
                updateProgress.status == UpdateStatus::Requested) {
                Log.println("Update already in progress");
                return ESP_FAIL;
            }

            updateProgress.status = UpdateStatus::Requested;
            Log.printf("Starting %s update\n", name);

            if (total) {
                Log.printf("%s size: %u bytes\n", name, *total);
            }

            bool hasStarted = Update.begin(total ? *total : UPDATE_SIZE_UNKNOWN, partition);

            if (!hasStarted) {
                updateProgress.status = UpdateStatus::UpdateFailed;

                Log.println("Failed to begin " + String(name) + " update");
                Log.printf("Update error: %d\n", Update.getError());
                Update.printError(Log);
                return ESP_FAIL;
            }
        }

        // Write current chunk
        if (Update.write(data, len) != len) {
            updateProgress.status = UpdateStatus::UpdateFailed;

            Log.println("Failed to write " + String(name) + " chunk");
            Log.printf("Update error: %d\n", Update.getError());
            Update.printError(Log);
            return ESP_FAIL;
        }

        if (updateProgress.status != UpdateStatus::InProgress) {
            updateProgress.status = UpdateStatus::InProgress;
        }

        if (total) {
            Log.printf("%s: %llu / %u bytes\r",
                       name,
                       static_cast<unsigned long long>(index + len),
                       *total);

            updateProgress.progress = static_cast<uint8_t>(((index + len) * 100) / *total);
            updateProgress.hasTotalSize = true;
        } else {
            Log.printf("%s: %llu bytes\r", name, static_cast<unsigned long long>(index + len));

            updateProgress.progress = 0;
            updateProgress.hasTotalSize = false;
        }

        // Final chunk
        if (final) {
            Log.println();

            if (!Update.end(true)) {
                updateProgress.status = UpdateStatus::UpdateFailed;

                Log.println("Failed to finish " + String(name) + " update");
                Update.printError(Log);
                return ESP_FAIL;
            }

            updateProgress.status = UpdateStatus::UpdateComplete;
            Log.println(String(name) + " written successfully");
        }

        return ESP_OK;
    }

    bool downloadAndInstall(const UpdateRequest& request, bool isFirmware) {
        const char* name = isFirmware ? "Firmware" : "Website";
        const auto partition = isFirmware ? U_FLASH : U_SPIFFS;

        if (updateProgress.status == UpdateStatus::InProgress) {
            Log.println("Update already in progress");
            return false;
        }

        updateProgress.status = UpdateStatus::Requested;
        updateProgress.progress = 0;
        updateProgress.hasTotalSize = false;

        Log.printf("Starting %s update\n", name);
        Log.printf("%s size: %u bytes\n", name, request.size);

        if (!NetworkService::getNetworkStatus().connected) {
            Log.println("No network connection");
            updateProgress.status = UpdateStatus::UpdateFailed;
            return false;
        }

        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        if (!http.begin(client, request.url)) {
            Log.println("Failed to open download URL");
            updateProgress.status = UpdateStatus::UpdateFailed;
            return false;
        }

        int code = http.GET();

        if (code != HTTP_CODE_OK) {
            Log.printf("HTTP GET failed (%d)\n", code);
            http.end();
            updateProgress.status = UpdateStatus::UpdateFailed;
            return false;
        }

        int contentLength = http.getSize();

        if (contentLength >= 0 && static_cast<size_t>(contentLength) != request.size) {
            Log.printf("Warning: Manifest size (%u) != HTTP Content-Length (%d)\n",
                       request.size,
                       contentLength);
        }

        if (!Update.begin(request.size, partition)) {
            Log.println("Failed to begin update");
            Log.printf("Update error: %d\n", Update.getError());
            Update.printError(Log);

            http.end();
            updateProgress.status = UpdateStatus::UpdateFailed;
            return false;
        }

        auto* stream = http.getStreamPtr();

        static uint8_t buffer[1024];
        size_t written = 0;

        mbedtls_sha256_context sha;
        mbedtls_sha256_init(&sha);
        mbedtls_sha256_starts(&sha, 0);

        updateProgress.status = UpdateStatus::InProgress;

        while (http.connected() && written < request.size) {
            size_t available = stream->available();

            if (!available) {
                delay(1);
                continue;
            }

            size_t toRead = std::min(available, sizeof(buffer));
            int len = stream->readBytes(buffer, toRead);

            if (len <= 0)
                continue;

            if (Update.write(buffer, len) != static_cast<size_t>(len)) {
                Log.println("Failed to write update chunk");
                Log.printf("Update error: %d\n", Update.getError());
                Update.printError(Log);

                Update.abort();
                http.end();
                mbedtls_sha256_free(&sha);

                updateProgress.status = UpdateStatus::UpdateFailed;
                return false;
            }

            mbedtls_sha256_update(&sha, buffer, static_cast<size_t>(len));

            written += static_cast<size_t>(len);

            Log.printf("%s: %u / %u bytes\r", name, static_cast<unsigned>(written), request.size);
        }

        Log.println();

        http.end();

        if (written != request.size) {
            Log.println("Download incomplete");

            Update.abort();
            mbedtls_sha256_free(&sha);

            updateProgress.status = UpdateStatus::UpdateFailed;
            return false;
        }

        uint8_t digest[32];
        mbedtls_sha256_finish(&sha, digest);
        mbedtls_sha256_free(&sha);

        char actualHash[65];

        for (int i = 0; i < 32; i++) {
            sprintf(actualHash + i * 2, "%02x", digest[i]);
        }

        actualHash[64] = '\0';

        if (strcasecmp(actualHash, request.sha256.c_str()) != 0) {
            Log.println("SHA-256 verification failed");
            Log.printf("Expected: %s\n", request.sha256.c_str());
            Log.printf("Actual:   %s\n", actualHash);

            Update.abort();

            updateProgress.status = UpdateStatus::UpdateFailed;
            return false;
        }

        Log.println("SHA-256 verified successfully");

        if (!Update.end(true)) {
            Log.println("Failed to finish update");
            Log.printf("Update error: %d\n", Update.getError());
            Update.printError(Log);

            Update.abort();

            updateProgress.status = UpdateStatus::UpdateFailed;
            return false;
        }

        updateProgress.status = UpdateStatus::UpdateComplete;

        Log.println(String(name) + " written successfully");
        Log.println("Restarting...");

        SystemManager::requestRestart();

        return true;
    }

    int compareVersions(String a, String b) {
        a.replace("v", "");
        b.replace("v", "");

        size_t aIndex = 0;
        size_t bIndex = 0;

        while (aIndex < a.length() || bIndex < b.length()) {
            int aPart = 0;
            int bPart = 0;

            while (aIndex < a.length() && a[aIndex] != '.') {
                aPart = aPart * 10 + (a[aIndex++] - '0');
            }

            while (bIndex < b.length() && b[bIndex] != '.') {
                bPart = bPart * 10 + (b[bIndex++] - '0');
            }

            if (aPart < bPart)
                return -1;

            if (aPart > bPart)
                return 1;

            if (aIndex < a.length())
                ++aIndex;

            if (bIndex < b.length())
                ++bIndex;
        }

        return 0;
    }

    bool requestUpdate(const UpdateRequest& request, bool isFirmware) {
        if (updateProgress.status == UpdateStatus::InProgress ||
            updateProgress.status == UpdateStatus::Requested) {
            Log.println("Update already in progress");
            return false;
        }

        if (!request.validatePayload()) {
            Log.println("Update request rejected: Invalid signature");
            return false;
        }

        if (request.url.isEmpty()) {
            Log.println("Update request rejected: URL is empty");
            return false;
        }

        if (request.version.isEmpty()) {
            Log.println("Update request rejected: Version is empty");
            return false;
        }

        if (request.size == 0) {
            Log.println("Update request rejected: Size is zero");
            return false;
        }

        if (request.sha256.length() != 64) {
            Log.println("Update request rejected: Invalid SHA-256");
            return false;
        }

        for (char c : request.sha256) {
            if (!isxdigit(static_cast<unsigned char>(c))) {
                Log.println("Update request rejected: SHA-256 contains invalid characters");
                return false;
            }
        }

        currentUpdateRequest = request;
        currentUpdateRequest.isFirmware = isFirmware;

        updateProgress.status = UpdateStatus::Requested;

        if (updateTaskHandle == nullptr) {
            Log.println("Update task not initialized");
            return false;
        }
        xTaskNotifyGive(updateTaskHandle);

        Log.printf("Update requested: %s\n", isFirmware ? "Firmware" : "Website");
        Log.printf("Version: %s\n", request.version.c_str());
        Log.printf("URL: %s\n", request.url.c_str());
        Log.printf("Size: %u bytes\n", request.size);
        Log.printf("SHA-256: %s\n", request.sha256.c_str());

        return true;
    }

    const UpdateProgress& getUpdateProgress() {
        return updateProgress;
    }
}  // namespace UpdateHandler