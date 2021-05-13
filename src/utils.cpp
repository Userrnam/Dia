#include "utils.hpp"

sf::Vector2f* getClosestLinePoint(AppInfo* info, sf::Vector2f pos, float* distance2, Line** pline)
{
	sf::Vector2f* res = nullptr;
	Line* l = nullptr;
	float mind = 1e15;
	for (auto& line : info->lines)
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

Circle* getClosestCircle(AppInfo* info, sf::Vector2f pos, float* distance2)
{
	float mind = 1e15;
	Circle* res = nullptr;

	for (auto& circle : info->circles)
	{
		float d2center2 = d2(circle.center, pos);
		float d = sqrt(d2center2) - circle.radius;
		float d2 = d * d;
		if (d2 < mind)
		{
			mind = d2;
			res = &circle;
		}
	}

	*distance2 = mind;

	return res;
}

float d2line(Line& l, sf::Vector2f pos)
{
	float lineLength = sqrt(d2(l.p[0], l.p[1]));
	auto lineNormal = (l.p[1] - l.p[0]) / lineLength;

	auto d = dot(lineNormal, pos - l.p[0]);
	if (0 < d && d < lineLength)
	{
		float a2 = d2(l.p[0], pos);
		float b2 = d2(l.p[1], pos);
		float c2 = d2(l.p[0], l.p[1]);

		float c = sqrt(c2);

		float t = (a2 - b2 - c2) / (2 * c);

		return b2 - t * t;
	}

	float mind = d2(pos, l.p[0]);
	float dist = d2(pos, l.p[1]);
	if (dist < mind)
	{
		return dist;
	}
	return mind;
}

Line* getClosestLine(AppInfo* info, sf::Vector2f pos, float* distance2)
{
	float d = 1e6;
	Line* l = nullptr;

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

char getCharFromKeyEvent(sf::Event::KeyEvent& e)
{
	char c = -1;

	if (sf::Keyboard::A <= e.code && e.code <= sf::Keyboard::Z)
	{
		c = 'a' + (e.code - sf::Keyboard::A);
		if (e.shift)
		{
			c -= ('a' - 'A');
		}
		return c;
	}

	switch (e.code)
	{
	case sf::Keyboard::Num1:
		if (e.shift)  return '!';
		return '1';
	case sf::Keyboard::Num2:
		if (e.shift)  return '@';
		return '2';
	case sf::Keyboard::Num3:
		if (e.shift)  return '#';
		return '3';
	case sf::Keyboard::Num4:
		if (e.shift)  return '$';
		return '4';
	case sf::Keyboard::Num5:
		if (e.shift)  return '%';
		return '5';
	case sf::Keyboard::Num6:
		if (e.shift)  return '^';
		return '6';
	case sf::Keyboard::Num7:
		if (e.shift)  return '&';
		return '7';
	case sf::Keyboard::Num8:
		if (e.shift)  return '*';
		return '8';
	case sf::Keyboard::Num9:
		if (e.shift)  return '(';
		return '9';
	case sf::Keyboard::Num0:
		if (e.shift)  return ')';
		return '0';
	case sf::Keyboard::Hyphen:
		if (e.shift)  return '_';
		return '-';
	case sf::Keyboard::Equal:
		if (e.shift)  return '+';
		return '=';
	case sf::Keyboard::Tilde:
		if (e.shift)  return '`';
		return '~';
	case sf::Keyboard::LBracket:
		if (e.shift)  return '{';
		return '[';
	case sf::Keyboard::RBracket:
		if (e.shift)  return '}';
		return ']';
	case sf::Keyboard::Comma:
		if (e.shift)  return '<';
		return ',';
	case sf::Keyboard::SemiColon:
		if (e.shift)  return ':';
		return ';';
	case sf::Keyboard::Quote:
		if (e.shift)  return '\"';
		return '\'';
	case sf::Keyboard::Slash:
		if (e.shift)  return '?';
		return '/';
	case sf::Keyboard::BackSlash:
		if (e.shift)  return '|';
		return '\\';
	case sf::Keyboard::Period:
		if (e.shift)  return '>';
		return '.';
	case sf::Keyboard::Enter:
		if (e.shift)  return '\n';
		return '\n';
	case sf::Keyboard::Space:
		return ' ';
	case sf::Keyboard::Tab:
		return '\t';
	default: {}
	}

	return '?';
}

bool charPrintable(sf::Event::KeyEvent& e)
{
	if (sf::Keyboard::A <= e.code && e.code <= sf::Keyboard::Z)
	{
		return true;
	}

	switch (e.code)
	{
	case sf::Keyboard::Num1:
	case sf::Keyboard::Num2:
	case sf::Keyboard::Num3:
	case sf::Keyboard::Num4:
	case sf::Keyboard::Num5:
	case sf::Keyboard::Num6:
	case sf::Keyboard::Num7:
	case sf::Keyboard::Num8:
	case sf::Keyboard::Num9:
	case sf::Keyboard::Num0:
	case sf::Keyboard::Hyphen:
	case sf::Keyboard::Equal:
	case sf::Keyboard::Tilde:
	case sf::Keyboard::LBracket:
	case sf::Keyboard::RBracket:
	case sf::Keyboard::Comma:
	case sf::Keyboard::SemiColon:
	case sf::Keyboard::Quote:
	case sf::Keyboard::Divide:
	case sf::Keyboard::Space:
	case sf::Keyboard::Tab:
	case sf::Keyboard::Slash:
	case sf::Keyboard::BackSlash:
	case sf::Keyboard::Enter:
	case sf::Keyboard::Period:
		return true;
	default: {}
	}

	return false;
}

sf::Vector2f getCharacterSize(const sf::Font* font, int size)
{
	sf::Text t;
	t.setFont(*font);
	t.setCharacterSize(size);
	t.setString("H");
	auto bounding = t.getGlobalBounds();
	return sf::Vector2f(bounding.width, bounding.height);
}

void updateBoundingBox(Text* text)
{
	text->bounding = text->text.getGlobalBounds();
	auto size = getCharacterSize(text->text.getFont(), text->text.getCharacterSize());
	if (text->bounding.height < 10)
	{
		text->bounding.height = size.y;
		text->bounding.top -= size.y;
	}
	if (text->bounding.width < 10)
	{
		text->bounding.width = size.x;
	}
}

