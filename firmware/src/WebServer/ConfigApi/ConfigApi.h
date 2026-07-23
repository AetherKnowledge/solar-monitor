#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace ConfigApi {
    void registerApi(AsyncWebServer& server);
    void handleUploadConfig(AsyncWebServerRequest* request, JsonVariant& json);
    void handleGetConfig(AsyncWebServerRequest* request);

    void onUpload(AsyncWebServerRequest* request,
                  String filename,
                  size_t index,
                  uint8_t* data,
                  size_t len,
                  bool final);

    void onUploadFinish(AsyncWebServerRequest* request);
    bool validateConfig();
}  // namespace ConfigApi
