#include "CreateMode.hpp"


float calcRadius(sf::Vector2f center, sf::Vector2f point)
{
	return sqrt(d2(center, point));
}

void CreateMode::onEvent(sf::Event& e) {
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
			auto pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			if (state == Line)
			{
				state = NewLine;

				info->lines.push_back({});
				info->lines.back().p[0] = snap(info, pos);
				info->lines.back().p[1] = info->lines.back().p[0];
			}
			else if (state == Circle)
			{
				state = NewCircle;

				info->circles.push_back({});
				info->circles.back().center = snap(info, pos);
				info->circles.back().radius = 0;
			}
			else if (state == Text || state == NewText)
			{
				checkText();

				state = NewText;
				info->texts.push_back({});
				info->texts.back().text.setFont(info->font);
				info->texts.back().text.setCharacterSize(35);
				info->texts.back().text.setFillColor(sf::Color::Black);
				info->texts.back().text.setPosition(snap(info, pos));
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
			if (state != NewText)
			{
				if (e.key.code == sf::Keyboard::L)
				{
					state = Line;
				}
				else if (e.key.code == sf::Keyboard::C)
				{
					state = Circle;
				}
				else if (e.key.code == sf::Keyboard::T)
				{
					state = Text;
				}
			}
			else
			{
				auto s = info->texts.back().text.getString();
				if (e.key.code == sf::Keyboard::BackSpace)
				{
					if (s.getSize() == 0)
					{
						return;
					}

					s.erase(s.getSize()-1);
					info->texts.back().text.setString(s);
				}

				if (charPrintable(e.key))
				{
					info->texts.back().text.setString(s + getCharFromKeyEvent(e.key));
				}

				// update bounding box
				info->texts.back().bounding = info->texts.back().text.getGlobalBounds();
			}
			break;
		}

		default: {}
	}
}


void CreateMode::onEnter()
{
	state = Line;
}

void CreateMode::onExit()
{
	state = Line;
	checkText();
}

std::string CreateMode::getModeDescription()
{
	if (state == Line || state == NewLine)
		return "Create Line";

	if (state == Circle || state == NewCircle)
		return "Create Circle";

	if (state == Text || state == NewText)
		return "Create Text";

	return "Error";
}

void CreateMode::checkText()
{
	if (state == Text || state == NewText)
	{
		if (info->texts.size() > 0)
		{
			if (info->texts.back().text.getString().getSize() == 0)
			{
				info->texts.pop_back();
				return;
			}
		}
	}
}


