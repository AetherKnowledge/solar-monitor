#include "ConfigApi.h"
#include <Common/Network.h>
#include <Config/ConfigManager.h>
#include <Common/Logger.h>

namespace ConfigApi {
    void registerApi(AsyncWebServer& server) {
        Log.println("Config API registered");
    }
}  // namespace ConfigApi
