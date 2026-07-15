#pragma once

#include "ConfigTypes.h"

extern Config config;
extern bool configLoaded;

extern volatile bool networkUpdateRequested;
extern NetworkConfig pendingNetworkConfig;

bool loadConfig();
bool saveConfig();
bool setNetworkConfig(const NetworkConfig &newConfig);
bool setMqttConfig(const MQTTConfig &newConfig);
void resetConfig();