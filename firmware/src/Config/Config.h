#pragma once

#include "ConfigTypes.h"

extern Config config;

bool loadConfig();
bool saveConfig();
void resetConfig();