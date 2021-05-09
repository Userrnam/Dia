#pragma once

#include "AppInfo.hpp"

void drawCircles(const std::vector<Circle>& circles, sf::RenderTarget *window, sf::Transform* t=nullptr);
void drawLines(const std::vector<Line>& lines, sf::RenderTarget *window, sf::Transform* t=nullptr);
void drawTexts(const std::vector<Text>& texts, sf::RenderTarget *window, bool drawBack=true, sf::Transform* t=nullptr);

