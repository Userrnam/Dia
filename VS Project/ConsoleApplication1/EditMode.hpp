#pragma once

#include "AppInfo.hpp"

void onEditEvent(AppInfo* info, sf::Event& e);
void onEditEnter(AppInfo* info);
void onEditExit(AppInfo* info);
void editBeforeDraw(AppInfo* info);

