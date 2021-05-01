#include "EditMode.hpp"
#include "utils.hpp"

#define MAX_SELECT_DISTANCE 20


void EditMode::onEvent(struct AppInfo * info, sf::Event& e)
{
	switch (e.type)
	{
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			if (state == MovingPoint)
			{
				*pVec = snap(info, mousePos);
			}
			else if (state == ChangingCircleRadius)
			{
				auto vec = snap(info, mousePos);
				pCircle->radius = sqrt(d2(vec, pCircle->center));
			}

			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			sf::Vector2f pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			if (state == Point)
			{
				float dist2;
				sf::Vector2f *p = getClosestLinePoint(info, pos, &dist2);
				if (dist2 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					state = MovingPoint;
					pVec = p;
				}

				float dist3;
				Circle *p2 = getClosestCircle(info, pos, &dist3);
				if (dist3 < dist2 && dist3 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					state = MovingPoint;
					pVec = &p2->center;
					if (info->shiftPressed)
					{
						state = ChangingCircleRadius;
						pCircle = p2;
					}
				}
			}

			break;
		}

		case sf::Event::MouseButtonReleased:
		{
			if (state == MovingPoint)
			{
				state = Point;
				pVec = nullptr;
				// FIXME: 2 points of the same line may be the same.
				// If this happens, the line should be deleted
			}
			else if (state == ChangingCircleRadius)
			{
				state = Point;
				pCircle = nullptr;
			}

			break;
		}

		default: {}
	}
}

void EditMode::onEnter(struct AppInfo *)
{
	state = Point;
}

void EditMode::onExit(struct AppInfo *)
{
	state = Point;
}

std::string EditMode::getModeDescription()
{

	if (state == Point || state == MovingPoint)
		return "Edit Point";

	if (state == ChangingCircleRadius)
		return "Change Circle Raidus";

	return "Error";
}

