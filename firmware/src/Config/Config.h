#pragma once

#include "ConfigTypes.h"
#include <Common/File.h>

inline constexpr char CONFIG_LOCATION[] = "/config.json";
inline constexpr char CONFIG_BAK_LOCATION[] = "/config.json.bak";

extern Config config;
extern bool configLoaded;

extern volatile bool networkUpdateRequested;
extern NetworkConfig pendingNetworkConfig;

bool loadConfigFile(File& file);
bool loadConfig();
bool saveConfig();
bool updateNetworkConfig(const NetworkConfig& newConfig);
bool updateMqttConfig(const MQTTConfig& newConfig);
bool updateModbusConfig(const std::vector<ModbusDevice>& devices);
void resetConfig();