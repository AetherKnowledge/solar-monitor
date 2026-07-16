#pragma once

#include "ConfigTypes.h"

extern Config config;
extern bool configLoaded;

extern volatile bool networkUpdateRequested;
extern NetworkConfig pendingNetworkConfig;

bool loadConfig();
bool saveConfig();
bool updateNetworkConfig(const NetworkConfig &newConfig);
bool updateMqttConfig(const MQTTConfig &newConfig);
void resetConfig();