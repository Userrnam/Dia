#pragma once

#include "AppInfo.hpp"

AppInfo loadProject(const std::string& path, bool *success, sf::Font *font);
bool saveProject(AppInfo *info, const std::string& path);

