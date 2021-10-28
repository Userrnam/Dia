#include "Selection.hpp"
#include "utils.hpp"

void Selection::move(sf::Vector2f v)
{
	for (auto& line : lines)
	{
		line->p[0] += v;
		line->p[1] += v;
	}

	for (auto& circle : circles)
	{
		circle->center += v;
	}

	for (auto& text : texts)
	{
		text->bounding.left += v.x;
		text->bounding.top += v.y;

		text->text.move(v);
	}
}

sf::Vector2f Selection::closestPoint(sf::Vector2f v)
{
	sf::Vector2f res = sf::Vector2f(0, 0);
	float dMin = 1e7;

	for (auto line : lines)
	{
		float dist = d2(v, line->p[0]);
		if (dist < dMin)
		{
			dMin = dist;
			res = line->p[0];
		}
		dist = d2(v, line->p[1]);
		if (dist < dMin)
		{
			dMin = dist;
			res = line->p[1];
		}
	}

	for (auto circle : circles)
	{
		float dist = d2(v, circle->center);
		if (dist < dMin)
		{
			dMin = dist;
			res = circle->center;
		}
	}

	for (auto text : texts)
	{
		auto point = sf::Vector2f(text->bounding.left, text->bounding.top);
		float dist = d2(v, point);
		if (dist < dMin)
		{
			dMin = dist;
			res = point;
		}
	}

	return res;
}

void Selection::add(Line* line)
{
	for (auto l : lines)
	{
		if (l == line)   return;
	}

	lines.push_back(line);
}

void Selection::add(Circle* circle)
{
	for (auto c : circles)
	{
		if (c == circle)   return;
	}

	circles.push_back(circle);
}

void Selection::add(Text* text)
{
	for (auto t : texts)
	{
		if (t == text)   return;
	}

	texts.push_back(text);
}

bool Selection::contains(Line* line)
{
	for (auto l : lines)
	{
		if (l == line)  return true;;
	}
	return false;
}

bool Selection::contains(Circle* circle)
{
	for (auto c : circles)
	{
		if (c == circle)  return true;
	}
	return false;
}

bool Selection::contains(Text* text)
{
	for (auto t : texts)
	{
		if (t == text)  return true;;
	}

	return false;
}

void CopyInfo::move(sf::Vector2f v)
{
	for (auto& line : lines)
	{
		line.p[0] += v;
		line.p[1] += v;
	}

	for (auto& circle : circles)
	{
		circle.center += v;
	}

	for (auto& text : texts)
	{
		text.bounding.left += v.x;
		text.bounding.top += v.y;

		text.text.move(v);
	}
}

