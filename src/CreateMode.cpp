#include "CreateMode.hpp"


float calcRadius(sf::Vector2f center, sf::Vector2f point)
{
	return sqrt(d2(center, point));
}

void CreateMode::onEvent(struct AppInfo *info, sf::Event& e) {
	switch (e.type)
	{
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			if (state == NewLine)
			{
				info->lines.back().p[1] = snap(info, mousePos);
			}
			else if (state == NewCircle)
			{
				info->circles.back().radius = calcRadius(info->circles.back().center, snap(info, mousePos));
			}
			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			if (state == Line)
			{
				state = NewLine;

				info->lines.push_back({});
				info->lines.back().p[0] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->lines.back().p[1] = info->lines.back().p[0];
			}
			else if (state == Circle)
			{
				state = NewCircle;

				info->circles.push_back({});
				info->circles.back().center = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->circles.back().radius = 0;
			}

			break;
		}

		case sf::Event::MouseButtonReleased:
		{
			if (state == NewLine)
			{
				info->lines.back().p[1] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				state = Line;
				if (same(info->lines.back().p[0], info->lines.back().p[1]))
				{
					info->lines.pop_back();
				}
			}
			else if (state == NewCircle)
			{
				info->circles.back().radius = calcRadius(info->circles.back().center,
						snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y)));

				state = Circle;
				if (info->circles.back().radius < EPS)
				{
					info->circles.pop_back();
				}
			}

			break;
		}

		case sf::Event::KeyPressed:
		{
			if (e.key.code == sf::Keyboard::L)
			{
				state = Line;
			}
			else if (e.key.code == sf::Keyboard::C)
			{
				state = Circle;
			}
			break;
		}

		default: {}
	}
}


void CreateMode::onEnter(struct AppInfo *)
{
	state = Line;
}

void CreateMode::onExit(struct AppInfo *)
{
	state = Line;
}

std::string CreateMode::getModeDescription()
{
	if (state == Line || state == NewLine)
		return "Create Line";

	if (state == Circle || state == NewCircle)
		return "Create Circle";

	return "Error";
}


