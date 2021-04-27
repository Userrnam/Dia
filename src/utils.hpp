#pragma once

#include "AppInfo.hpp"

#define EPS 1e-4

inline float d2(sf::Vector2f v1, sf::Vector2f v2)
{
	sf::Vector2f v = v1 - v2;
	return v.x * v.x + v.y * v.y;
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
	sf::Vector2f p;
	p.x = ((int)(pos.x + info->gridSize/2) / info->gridSize) * info->gridSize;
	p.y = ((int)(pos.y + info->gridSize/2) / info->gridSize) * info->gridSize;
	return p;
}

sf::Vector2f *getClosestPoint(AppInfo *info, sf::Vector2f pos, float *distance2, Line **pline = nullptr);
float d2line(Line& l, sf::Vector2f pos);
Line *getClosestLine(AppInfo *info, sf::Vector2f pos, float *distance2);

