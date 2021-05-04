#include "EditMode.hpp"
#include "utils.hpp"

#define MAX_SELECT_DISTANCE 20

const sf::Color selectionColor = sf::Color(70,70,180,90);

void Selection::add(Line *line)
{
	for (auto l : lines)
	{
		if (l == line)   return;
	}

	lines.push_back(line);
}

void Selection::add(Circle *circle)
{
	for (auto c : circles)
	{
		if (c == circle)   return;
	}

	circles.push_back(circle);
}

void Selection::add(Text *text)
{
	for (auto t : texts)
	{
		if (t == text)   return;
	}

	texts.push_back(text);
}

void removeSelection(Selection *pSelection, AppInfo *info)
{
	std::sort(pSelection->lines.begin(), pSelection->lines.end());
	int i = 0;
	for (auto line : pSelection->lines)
	{
		int index = line - info->lines.data() - i;
		info->lines.erase(info->lines.begin() + index);
		i++;
	}

	std::sort(pSelection->circles.begin(), pSelection->circles.end());
	i = 0;
	for (auto circle : pSelection->circles)
	{
		int index = circle - info->circles.data() - i;
		info->circles.erase(info->circles.begin() + index);
		i++;
	}

	std::sort(pSelection->texts.begin(), pSelection->texts.end());
	i = 0;
	for (auto text : pSelection->texts)
	{
		int index = text - info->texts.data() - i;
		info->texts.erase(info->texts.begin() + index);
		i++;
	}

	pSelection->clear();
}

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
			else if (state == MovingLine)
			{
				auto move = vec - *pVec;
				pLine->p[0] += move;
				pLine->p[1] += move;
			}
			else if (state == SelectElement)
			{
				state = possibleNextState;
			}

			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			if (!info->shiftPressed)
			{
				selection.clear();
			}

			if (state == SelectEnd)
			{
				state = Point;
			}

			sf::Vector2f pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			if (state == Point)
			{
				// check texts
				for (auto& t : info->texts)
				{
					if (t.bounding.contains(pos))
					{
						selection.add(&t);
						state = SelectElement;
						possibleNextState = MovingText;
						pText = &t;
						return;
					}
				}
				
				Line *ll;
				float dist2;
				sf::Vector2f *p = getClosestLinePoint(info, pos, &dist2, &ll);
				if (dist2 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					// if user clicks on point, it cannot be a selection
					state = MovingPoint;
					pVec = p;
					return;
				}

				float dist3;
				Circle *p2 = getClosestCircle(info, pos, &dist3);
				if (dist3 < dist2 && dist3 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					selection.add(p2);
					state = SelectElement;
					possibleNextState = MovingPoint;
					pVec = &p2->center;

					if (info->shiftPressed)
					{
						possibleNextState = ChangingCircleRadius;
						pCircle = p2;
					}
					return;
				}

				// check line
				Line *pL = getClosestLine(info, pos, &dist2);
				if (dist2 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					selection.add(pL);
					state = SelectElement;
					possibleNextState = MovingLine;
					pLine = pL;

					float dl1 = d2(pos, pLine->p[0]);
					float dl2 = d2(pos, pLine->p[1]);

					if (dl1 < dl2)
					{
						pVec = &pLine->p[0];
					}
					else
					{
						pVec = &pLine->p[1];
					}
					return;
				}

				selection.clear();
				state = Point;
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
			else if (state == MovingLine)
			{
				state = Point;
				pLine = nullptr;
			}
			else if (state == SelectElement)
			{
				state = SelectEnd;
			}

			break;
		}
		case sf::Event::KeyPressed:
		{
			if (e.key.code == sf::Keyboard::D)
			{
				removeSelection(&selection, info);
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
		return "Moving Text";

	if (state == MovingLine)
		return "Moving Line";

	if (state == SelectElement)
		return "Select Element";

	if (state == SelectEnd)
		return "Select End";

	return "Error";
}


void drawCirclesSelection(const std::vector<Circle*>& circles, sf::RenderWindow *window);
void drawLinesSelection(const std::vector<Line*>& lines, sf::RenderWindow *window);
void drawTextsSelection(const std::vector<Text*>& texts, sf::RenderWindow *window, bool drawBack=true);

void EditMode::beforeDraw()
{
	// draw selection
	drawLinesSelection(selection.lines, info->window);
	drawCirclesSelection(selection.circles, info->window);
	drawTextsSelection(selection.texts, info->window);
}

void drawCirclesSelection(const std::vector<Circle*>& circles, sf::RenderWindow *window)
{
	sf::CircleShape shape;

	shape.setOutlineColor(selectionColor);
	shape.setFillColor(sf::Color(0,0,0,0));

	for (auto c : circles)
	{
		auto& circle = *c;

		shape.setRadius(circle.radius - circle.outlineThickness/2 - 5);
		shape.setOutlineThickness(circle.outlineThickness + 5);

		shape.setPosition(circle.center);
		shape.setOrigin(shape.getRadius(), shape.getRadius());

		window->draw(shape);
	}
}

void drawLinesSelection(const std::vector<Line*>& lines, sf::RenderWindow *window)
{
	sf::VertexArray va;
	va.setPrimitiveType(sf::PrimitiveType::Quads);
	va.resize(lines.size() * 4);

	int k = 0;
	for (auto l: lines)
	{
		auto& line = *l;

		sf::Vector2f normal;
		normal.x = -(line.p[0] - line.p[1]).y;
		normal.y = (line.p[0] - line.p[1]).x;
		normal   = normalize(normal) * (line.width/2.0f + 5);

		va[k].color = selectionColor;
		va[k].position = line.p[0];
		va[k].position -= normal;
		k++;

		va[k].color = selectionColor;
		va[k].position = line.p[1];
		va[k].position -= normal;
		k++;

		va[k].color = selectionColor;
		va[k].position = line.p[1];
		va[k].position += normal;
		k++;

		va[k].color = selectionColor;
		va[k].position = line.p[0];
		va[k].position += normal;
		k++;

	}

	window->draw(va);
}

void drawTextsSelection(const std::vector<Text*>& texts, sf::RenderWindow *window, bool drawBack)
{
	sf::RectangleShape rect;
	rect.setFillColor(selectionColor);

	for (auto& t : texts)
	{
		rect.setPosition(t->bounding.left-5, t->bounding.top-5);
		rect.setSize(sf::Vector2f(t->bounding.width+10, t->bounding.height+10));
		window->draw(rect);
	}
}

