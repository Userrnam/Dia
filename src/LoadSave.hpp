#pragma once

#include "AppInfo.hpp"

AppInfo loadProject(const std::string& path, bool *success, sf::Font *font);
void saveProject(AppInfo *info, const std::string& path);

