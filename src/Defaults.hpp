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
		sf::Color color = sf::Color(0,0,0,0);
		sf::Color outlineColor = sf::Color::Black;
		float outlineThickness = 4;
	} circle;

	struct
	{
		int size = 35;
		sf::Color color;
	} text;
};

Defaults loadDefaults(const std::string& path, bool *success);

