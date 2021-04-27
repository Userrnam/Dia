#include "EditMode.hpp"
#include "utils.hpp"

#define MAX_SELECT_DISTANCE 20

struct EditInfo
{
	sf::Vector2f *pVec = 0;

	enum State
	{
		Point,
		MovingPoint
	};

	State state = Point;
};

void EditMode::onEvent(struct AppInfo * info, sf::Event& e)
{
	EditInfo *pd = info->pCurrentMode->data.as<EditInfo>();
	switch (e.type)
	{
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			if (pd->state == EditInfo::MovingPoint)
			{
				*pd->pVec = snap(info, mousePos);
			}

			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			if (pd->state == EditInfo::Point)
			{
				float dist2;
				sf::Vector2f *p = getClosestPoint(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y), &dist2);
				if (dist2 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					pd->state = EditInfo::MovingPoint;
					pd->pVec = p;
				}
			}

			break;
		}

		case sf::Event::MouseButtonReleased:
		{
			if (pd->state == EditInfo::MovingPoint)
			{
				pd->state = EditInfo::Point;
				pd->pVec = nullptr;
				// FIXME: 2 points of the same line may be the same.
				// If this happens, the line should be deleted
			}

			break;
		}

		default: {}
	}
}

void EditMode::onEnter(struct AppInfo *)
{
	data.alloc<EditInfo>();
}

void EditMode::onExit(struct AppInfo *)
{
	data.dealloc();
}

std::string EditMode::getModeDescription()
{
	auto *pd = data.as<EditInfo>();

	if (pd->state == EditInfo::Point || pd->state == EditInfo::MovingPoint)
		return "Edit Point";

	return "Error";
}

