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

struct Circle
{
	// circle is defined by origin and point on circumference
	sf::Vector2f p[2];
	sf::Color outlineColor = sf::Color::Black;
	sf::Color color = sf::Color(0,0,0,0);
	float outlineThickness = 4;
};


typedef void(*EventHandler)(struct AppInfo *, sf::Event &);
typedef void(*ModeExit)(struct AppInfo *, struct ModeInfo *);
typedef void(*ModeEnter)(struct AppInfo *, struct ModeInfo *);

struct Data
{
	void *data = nullptr;

	template<typename T>
	void alloc()
	{
		data = malloc(sizeof(T));
		new (data) T;
	}

	template<typename T>
	T* as()
	{
		return (T *)data;
	}

	void dealloc()
	{
		free(data);
	}
};

struct Mode
{
	sf::Keyboard::Key key;

	Mode(sf::Keyboard::Key _k) : key(_k) {}

	virtual void onEvent(struct AppInfo *, sf::Event& e) = 0;
	virtual void onExit(struct AppInfo *)  = 0;
	virtual void onEnter(struct AppInfo *) = 0;
	virtual std::string getModeDescription() = 0;

	Data data;
};

/*
 *
	virtual void onEvent(struct AppInfo *, sf::Event& e) override {}
	virtual void onExit(struct AppInfo *, sf::Event& e)  override {}
	virtual void onEnter(struct AppInfo *, sf::Event& e)  override{}
 * */


struct AppInfo
{
	int characterSize = 30;
	sf::Font font;
	sf::RenderWindow *window;
	sf::Vector2i windowSize;
	int gridSize = 50;
	Mode *pCurrentMode;


	std::vector<Line> lines;
	std::vector<Circle> circles;

	std::vector<Mode *> modes;
};

