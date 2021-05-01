#include "DeleteMode.hpp"
#include "utils.hpp"


#define MAX_DELETE_DISTANCE 20


void DeleteMode::onEvent(sf::Event& e)
{
	if (e.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f mousePos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);

		for (int i = 0; i < info->texts.size(); ++i)
		{
			auto& t = info->texts[i];
			if (t.bounding.contains(mousePos))
			{
				info->texts.erase(info->texts.begin() + i);
				return;
			}
		}

		float dist2;
		Line *l = getClosestLine(info, mousePos, &dist2);

		float dist3;
		Circle *c = getClosestCircle(info, mousePos, &dist3);

		if (dist2 < dist3)
		{
			// remove line
			if (dist2 < MAX_DELETE_DISTANCE * MAX_DELETE_DISTANCE)
			{
				int index = l - info->lines.data();
				
				info->lines.erase(info->lines.begin() + index);
			}
		}
		else
		{
			// remove circle
			if (dist3 < MAX_DELETE_DISTANCE * MAX_DELETE_DISTANCE)
			{
				int index = c - info->circles.data();
				
				info->circles.erase(info->circles.begin() + index);
			}
		}
	}
}

std::string DeleteMode::getModeDescription()
{
	return "Delete Mode";
}

