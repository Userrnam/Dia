#pragma once

#include "AppInfo.hpp"

#define EPS 1e-4

inline float d2(sf::Vector2f v1, sf::Vector2f v2)
{
	sf::Vector2f v = v1 - v2;
	return v.x * v.x + v.y * v.y;
}

inline void print(sf::Vector2f v)
{
	std::cout << "{" << v.x << "; " << v.y << "}\n";
}

inline float dot(sf::Vector2f v1, sf::Vector2f v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

inline bool same(sf::Vector2f v1, sf::Vector2f v2)
{
	return fabs(v1.x - v2.x) < EPS && fabs(v1.y - v2.y) < EPS;
}

inline sf::Vector2f normalize(sf::Vector2f v)
{
	float l = sqrt(v.x * v.x + v.y * v.y);
	return v / l;
}

inline sf::Vector2f snap(AppInfo *info, sf::Vector2f pos)
{
	if (!info->snapping)   return pos;

	sf::Vector2f p;
	pos /= info->cameraZoom;

	float size = info->gridSize / info->cameraZoom;
	if (pos.x > 0)  p.x = ((int)(pos.x + size/2) / (int)size) * size;
	else            p.x = ((int)(pos.x - size/2) / (int)size) * size;
	if (pos.y > 0)	p.y = ((int)(pos.y + size/2) / (int)size) * size;
	else	        p.y = ((int)(pos.y - size/2) / (int)size) * size;

	p *= info->cameraZoom;
	return p;
}

sf::Vector2f *getClosestLinePoint(AppInfo *info, sf::Vector2f pos, float *distance2, Line **pline = nullptr);
Circle *getClosestCircle(AppInfo *info, sf::Vector2f pos, float *distance2);

float d2line(Line& l, sf::Vector2f pos);
Line *getClosestLine(AppInfo *info, sf::Vector2f pos, float *distance2);

char getCharFromKeyEvent(sf::Event::KeyEvent& e);
bool charPrintable(sf::Event::KeyEvent& e);

