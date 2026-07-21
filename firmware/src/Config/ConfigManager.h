#pragma once

#include "ConfigTypes.h"
#include <Common/File.h>
#include <Common/Enum.h>
#include <Common/UpdateStatus.h>

namespace ConfigManager {
    inline constexpr char CONFIG_LOCATION[] = "/config.json";
    inline constexpr char CONFIG_BAK_LOCATION[] = "/config.json.bak";

    extern fs::LittleFSFS ConfigFS;
    extern Config config;
    extern bool hasLoaded;

    bool loadFile(File& file);
    bool mountFS();
    bool load();
    bool save();
    void reset();
}  // namespace ConfigManager