#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

struct Line
{
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
	sf::Vector2f center;
	float radius;
	sf::Color outlineColor = sf::Color::Black;
	sf::Color color = sf::Color(0,0,0,0);
	float outlineThickness = 4;
};

struct Text
{
	sf::Text text;
	sf::FloatRect bounding;
};

struct Mode
{
	sf::Keyboard::Key key;
	struct AppInfo *info;

	Mode(sf::Keyboard::Key _k, struct AppInfo *_info) : key(_k), info(_info) {}

	virtual void onEvent(sf::Event& e) = 0;
	virtual void onExit()  = 0;
	virtual void onEnter() = 0;
	virtual std::string getModeDescription() = 0;

	virtual void beforeDraw() {};
	virtual void afterDraw() {};
};

/*
 *
	virtual void onEvent(sf::Event& e) override {}
	virtual void onExit()  override {}
	virtual void onEnter() override {}
 * */


struct AppInfo
{
	int characterSize = 30;
	sf::Font font;
	sf::RenderWindow *window;
	sf::Vector2i windowSize;
	int gridSize = 64;
	Mode *pCurrentMode;

	bool shiftPressed = false;

	sf::Vector2f snappedPos;
	sf::View camera;
	float cameraZoom = 1.0f;
	sf::View defaultView;

	bool snapping = true;

	std::vector<Line> lines;
	std::vector<Circle> circles;
	std::vector<Text> texts;

	std::vector<Mode *> modes;
};

