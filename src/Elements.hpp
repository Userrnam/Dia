#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

struct Line
{
	uint64_t id;
	sf::Vector2f p[2];
	sf::Color color = sf::Color::Black;
	float width = 4;

	void print()
	{
		std::cout << "{" << p[0].x << ";" << p[0].y << "}; ";
		std::cout << "{" << p[1].x << ";" << p[1].y << "}";
	}
};

// may be we can use just sf::CircleShape?
struct Circle
{
	uint64_t id;
	sf::Vector2f center;
	float radius;
	sf::Color outlineColor = sf::Color::Black;
	sf::Color color = sf::Color(0, 0, 0, 0);
	float outlineThickness = 4;
};

struct Text
{
	uint64_t id;
	sf::Text text;
	sf::FloatRect bounding;
};

