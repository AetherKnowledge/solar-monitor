#pragma once

#include <LittleFS.h>

inline bool copyFile(const char* sourcePath, const char* destPath) {
    File sourceFile = LittleFS.open(sourcePath, "r");
    if (!sourceFile)
        return false;

    File destFile = LittleFS.open(destPath, "w");
    if (!destFile) {
        sourceFile.close();
        return false;
    }

    uint8_t buffer[512];  // 512 bytes is efficient for flash pages
    while (sourceFile.available()) {
        int bytesRead = sourceFile.read(buffer, sizeof(buffer));
        if (bytesRead > 0) {
            size_t written = destFile.write(buffer, bytesRead);

            if (written != (size_t)bytesRead) {
                destFile.close();
                sourceFile.close();
                return false;
            }
        }
    }

    destFile.close();
    sourceFile.close();
    return true;
}