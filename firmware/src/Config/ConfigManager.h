#pragma once

#include "ConfigTypes.h"
#include <Common/File.h>
#include <Common/Enum.h>
#include <Common/UpdateStatus.h>

namespace ConfigManager {
    inline constexpr char CONFIG_LOCATION[] = "/config.json";
    inline constexpr char CONFIG_BAK_LOCATION[] = "/config.json.bak";

    extern Config config;
    extern bool hasLoaded;

    bool loadFile(File& file);
    bool load();
    bool save();
}  // namespace ConfigManager