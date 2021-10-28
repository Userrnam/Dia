#pragma once

#include <SFML/Graphics.hpp>

struct Defaults
{
	struct
	{
		sf::Color color = sf::Color::Black;
		float width = 4;
	} line;

	struct
	{
		sf::Color color = sf::Color(0, 0, 0, 0);
		sf::Color outlineColor = sf::Color::Black;
		float outlineThickness = 4;
	} circle;

	struct
	{
		int size = 35;
		sf::Color color;
		std::string fontName = "Hack-Regular.ttf";
		sf::Font *font = nullptr;
	} text;

	struct
	{
		int size = 35;
		sf::Font *font = nullptr;
		std::string fontName = "Hack-Regular.ttf";
	} ui;
};

bool loadDefaults(struct AppInfo *info, const std::string& path);

