#include "DeleteMode.hpp"
#include "utils.hpp"


#define MAX_DELETE_DISTANCE 20


void DeleteMode::onEvent(struct AppInfo *info, sf::Event& e)
{
	if (e.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f mousePos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
		float dist2;
		Line *l = getClosestLine(info, mousePos, &dist2);

		// remove line
		if (dist2 < MAX_DELETE_DISTANCE * MAX_DELETE_DISTANCE)
		{
			int index = l - info->lines.data();
			
			info->lines.erase(info->lines.begin() + index);
		}
	}
}

std::string DeleteMode::getModeDescription()
{
	return "Delete Mode";
}

