#include "ConfigApi.h"
#include <Common/Network.h>
#include <Config/ConfigManager.h>

namespace ConfigApi {
    void registerApi(AsyncWebServer& server) {
        Serial.println("Config API registered");
    }
}  // namespace ConfigApi
