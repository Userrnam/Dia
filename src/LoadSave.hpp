#pragma once

#include "AppInfo.hpp"

bool loadProject(const std::string& path, AppInfo *app);
bool saveProject(AppInfo* info, const std::string& path);

