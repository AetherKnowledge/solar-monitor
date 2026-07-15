#pragma once

#include "ConfigTypes.h"

extern Config config;
extern bool configLoaded;

extern volatile bool configUpdateRequested;
extern Config pendingConfig;

bool loadConfig();
bool saveConfig();
bool setNetworkConfig(const NetworkConfig &newConfig);
bool setMqttConfig(const MQTTConfig &newConfig);
void resetConfig();