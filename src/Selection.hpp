#pragma once

#include "Elements.hpp"

struct Selection
{
	std::vector<Line*> lines;
	std::vector<Circle*> circles;
	std::vector<Text*> texts;

	void clear()
	{
		lines = {};
		circles = {};
		texts = {};
	}

	void add(Line* line);
	void add(Circle* circle);
	void add(Text* text);

	bool contains(Line* line);
	bool contains(Circle* circle);
	bool contains(Text* text);

	void move(sf::Vector2f vec);

	sf::Vector2f closestPoint(sf::Vector2f v);

	int size()
	{
		return lines.size() + circles.size() + texts.size();
	}
};

struct CopyInfo
{
	std::vector<Line> lines;
	std::vector<Circle> circles;
	std::vector<Text> texts;

	void clear()
	{
		lines.clear();
		circles.clear();
		texts.clear();
	}

	int size()
	{
		return lines.size() + circles.size() + texts.size();
	}

	void move(sf::Vector2f mov);
};

