#pragma once

#include "AppInfo.hpp"

void drawCircles(const std::vector<Circle>& circles, sf::RenderWindow *window);
void drawLines(const std::vector<Line>& lines, sf::RenderWindow *window);
void drawTexts(const std::vector<Text>& texts, sf::RenderWindow *window, bool drawBack=true);

