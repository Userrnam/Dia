#include "CreateMode.hpp"

struct CreateModeInfo
{
	enum State {
		Line,
		NewLine,
		Circle,
		NewCircle
	};

	State state = Line;
};


void CreateMode::onEvent(struct AppInfo *info, sf::Event& e) {
	auto *pd = data.as<CreateModeInfo>();
	switch (e.type)
	{
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			if (pd->state == CreateModeInfo::NewLine)
			{
				info->lines.back().p[1] = snap(info, mousePos);
			}
			else if (pd->state == CreateModeInfo::NewCircle)
			{
				info->circles.back().p[1] = snap(info, mousePos);
			}
			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			if (pd->state == CreateModeInfo::Line)
			{
				pd->state = CreateModeInfo::NewLine;

				info->lines.push_back({});
				info->lines.back().p[0] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->lines.back().p[1] = info->lines.back().p[0];
			}
			else if (pd->state == CreateModeInfo::Circle)
			{
				pd->state = CreateModeInfo::NewCircle;

				info->circles.push_back({});
				info->circles.back().p[0] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->circles.back().p[1] = info->circles.back().p[0];
			}

			break;
		}

		case sf::Event::MouseButtonReleased:
		{
			if (pd->state == CreateModeInfo::NewLine)
			{
				info->lines.back().p[1] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				pd->state = CreateModeInfo::Line;
				if (same(info->lines.back().p[0], info->lines.back().p[1]))
				{
					info->lines.pop_back();
				}
			}
			else if (pd->state == CreateModeInfo::NewCircle)
			{
				info->circles.back().p[1] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				pd->state = CreateModeInfo::Circle;
				if (same(info->circles.back().p[0], info->circles.back().p[1]))
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
				pd->state = CreateModeInfo::Line;
			}
			else if (e.key.code == sf::Keyboard::C)
			{
				pd->state = CreateModeInfo::Circle;
			}
			break;
		}

		default: {}
	}
}


void CreateMode::onEnter(struct AppInfo *)
{
	data.alloc<CreateModeInfo>();
}

void CreateMode::onExit(struct AppInfo *)
{
	data.dealloc();
}

std::string CreateMode::getModeDescription()
{
	auto *pd = data.as<CreateModeInfo>();
	if (pd->state == CreateModeInfo::Line || pd->state == CreateModeInfo::NewLine)
		return "Create Line";

	if (pd->state == CreateModeInfo::Circle || pd->state == CreateModeInfo::NewCircle)
		return "Create Circle";

	return "Error";
}


