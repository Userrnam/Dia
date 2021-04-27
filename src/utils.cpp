#include "utils.hpp"

sf::Vector2f *getClosestPoint(AppInfo *info, sf::Vector2f pos, float *distance2, Line **pline)
{
	sf::Vector2f *res = nullptr;
	Line *l = nullptr;
	float mind = 1e15;
	for (auto &line : info->lines)
	{
		for (int i = 0; i < 2; ++i)
		{
			float d = d2(pos, line.p[i]);
			if (d < mind)
			{
				mind = d;
				res = &line.p[i];
				l = &line;
			}
		}
	}
	*distance2 = mind;
	if (pline)
	{
		*pline = l;
	}
	return res;
}

float d2line(Line& l, sf::Vector2f pos)
{
	float a2 = d2(l.p[0], pos);
	float b2 = d2(l.p[1], pos);
	float c2 = d2(l.p[0], l.p[1]);

	float c = sqrt(c2);

	float t = (a2 - b2 - c2)/(2*c);

	return b2 - t*t;
}

Line *getClosestLine(AppInfo *info, sf::Vector2f pos, float *distance2)
{
	float d;
	Line *l;
	getClosestPoint(info, pos, &d, &l);

	for (auto& line : info->lines)
	{
		float dd = d2line(line, pos);
		if (dd < d)
		{
			d = dd;
			l = &line;
		}
	}

	*distance2 = d;

	return l;
}


