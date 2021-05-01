#include "EditMode.hpp"
#include "utils.hpp"

#define MAX_SELECT_DISTANCE 20


void EditMode::onEvent(sf::Event& e)
{
	switch (e.type)
	{
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			auto vec = snap(info, mousePos);

			if (state == MovingPoint)
			{
				*pVec = vec;
			}
			else if (state == ChangingCircleRadius)
			{
				pCircle->radius = sqrt(d2(vec, pCircle->center));
			}
			else if (state == MovingText)
			{
				pText->text.setPosition(vec);
				pText->bounding = pText->text.getGlobalBounds();
			}

			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			sf::Vector2f pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			if (state == Point)
			{
				// check texts
				for (auto& t : info->texts)
				{
					if (t.bounding.contains(pos))
					{
						state = MovingText;
						pText = &t;
						return;
					}
				}
				
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
			else if (state == MovingText)
			{
				state = Point;
				pText = nullptr;
			}

			break;
		}

		default: {}
	}
}

void EditMode::onEnter()
{
	state = Point;
}

void EditMode::onExit()
{
	state = Point;
}

std::string EditMode::getModeDescription()
{

	if (state == Point || state == MovingPoint)
		return "Edit Point";

	if (state == ChangingCircleRadius)
		return "Change Circle Raidus";

	if (state == MovingText)
		return "MovingText";

	return "Error";
}

