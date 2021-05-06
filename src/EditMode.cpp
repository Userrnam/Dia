#include "EditMode.hpp"
#include "utils.hpp"
#include "Drawer.hpp"

#define MAX_SELECT_DISTANCE 20

const sf::Color selectionColor = sf::Color(70,70,180,90);

void CopyInfo::move(sf::Vector2f v)
{
	for (auto& line : lines)
	{
		line.p[0] += v;
		line.p[1] += v;
	}

	for (auto& circle : circles)
	{
		circle.center += v;
	}

	for (auto& text : texts)
	{
		text.bounding.left += v.x;
		text.bounding.top += v.y;

		text.text.move(v);
	}
}

void copyCopyInfoToSelectionAndAppInfo(Selection *selection, AppInfo *info, CopyInfo *copyInfo)
{
	// insure that pointers in selection will be correct
	info->lines.reserve(info->lines.size()+copyInfo->lines.size()+10);
	for (auto line : copyInfo->lines)
	{
		info->lines.push_back(line);
		selection->lines.push_back(&info->lines.back());
	}

	info->circles.reserve(info->circles.size()+copyInfo->circles.size()+10);
	for (auto circles : copyInfo->circles)
	{
		info->circles.push_back(circles);
		selection->circles.push_back(&info->circles.back());
	}

	info->texts.reserve(info->texts.size()+copyInfo->texts.size()+10);
	for (auto text : copyInfo->texts)
	{
		info->texts.push_back(text);
		selection->texts.push_back(&info->texts.back());
	}
}

void copySelectionToCopyInfo(CopyInfo *copyInfo, Selection *selection)
{
	for (auto line : selection->lines)
	{
		copyInfo->lines.push_back(*line);
	}

	for (auto circles : selection->circles)
	{
		copyInfo->circles.push_back(*circles);
	}

	for (auto text : selection->texts)
	{
		copyInfo->texts.push_back(*text);
	}
}

Selection getSelectionFromRectangle(sf::FloatRect selectionRectangle, AppInfo *info)
{
	Selection result;

	for (auto& line : info->lines)
	{
		if (selectionRectangle.contains(line.p[0]) && selectionRectangle.contains(line.p[1]))
		{
			result.lines.push_back(&line);
		}
	}

	for (auto& circle : info->circles)
	{
		if (selectionRectangle.contains(sf::Vector2f(circle.center.x+circle.radius, circle.center.y+circle.radius)) &&
			selectionRectangle.contains(sf::Vector2f(circle.center.x-circle.radius, circle.center.y-circle.radius)) 
			)
		{
			result.circles.push_back(&circle);
		}
	}

	for (auto& text : info->texts)
	{
		auto bounds = text.bounding;
		if (selectionRectangle.contains(sf::Vector2f(bounds.left, bounds.top)) &&
			selectionRectangle.contains(sf::Vector2f(bounds.left+bounds.width, bounds.top+bounds.height))
			)
		{
			result.texts.push_back(&text);
		}
	}

	return result;
}

void Selection::move(sf::Vector2f v)
{
	for (auto& line : lines)
	{
		line->p[0] += v;
		line->p[1] += v;
	}

	for (auto& circle : circles)
	{
		circle->center += v;
	}

	for (auto& text : texts)
	{
		text->bounding.left += v.x;
		text->bounding.top += v.y;

		text->text.move(v);
	}
}

sf::Vector2f Selection::closestPoint(sf::Vector2f v)
{
	sf::Vector2f res = sf::Vector2f(0, 0);
	float dMin = 1e7;

	for (auto line : lines)
	{
		float dist = d2(v, line->p[0]);
		if (dist < dMin)
		{
			dMin = dist;
			res = line->p[0];
		}
		dist = d2(v, line->p[1]);
		if (dist < dMin)
		{
			dMin = dist;
			res = line->p[1];
		}
	}

	for (auto circle : circles)
	{
		float dist = d2(v, circle->center);
		if (dist < dMin)
		{
			dMin = dist;
			res = circle->center;
		}
	}

	for (auto text : texts)
	{
		auto point = sf::Vector2f(text->bounding.left, text->bounding.top);
		float dist = d2(v, point);
		if (dist < dMin)
		{
			dMin = dist;
			res = point;
		}
	}

	return res;
}

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

bool Selection::contains(Line *line)
{
	for (auto l : lines)
	{
		if (l == line)  return true;;
	}
	return false;
}

bool Selection::contains(Circle *circle)
{
	for (auto c : circles)
	{
		if (c == circle)  return true;
	}
	return false;
}

bool Selection::contains(Text *text)
{
	for (auto t : texts)
	{
		if (t == text)  return true;;
	}

	return false;
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
			else if (state == SelectionRectangle)
			{
				selectionRectangle.width  = mousePos.x - selectionRectangle.left;
				selectionRectangle.height = mousePos.y - selectionRectangle.top;
			}
			else if (state == MovingSelection)
			{
				auto mov = vec - movingSelectionReferencePoint;
				movingSelectionReferencePoint = vec;
				selection.move(mov);
			}
			else if (state == MovingCopy)
			{
				auto mov = vec - referencePoint;			
				referencePoint = vec;
				copyInfo.move(mov);
			}

			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			if (state == SelectEnd)
			{
				state = Point;
			}

			sf::Vector2f pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			if (state == Point || info->shiftPressed && state == MovingSelection)
			{
				// check texts
				for (auto& t : info->texts)
				{
					if (t.bounding.contains(pos))
					{
						if (selection.contains(&t))
						{
							state = MovingSelection;
							movingSelectionReferencePoint = sf::Vector2f(t.bounding.left, t.bounding.top);
						}
						else
						{
							if (!info->shiftPressed)  selection.clear();
							selection.add(&t);
							state = SelectElement;
							possibleNextState = MovingText;
							pText = &t;
						}
						return;
					}
				}
				
				Line *ll;
				float dist2;
				sf::Vector2f *p = getClosestLinePoint(info, pos, &dist2, &ll);
				if (dist2 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					if (selection.contains(ll) && selection.size() > 1)
					{
						state = MovingSelection;
						movingSelectionReferencePoint = *p;
						return;
					}

					// if user clicks on point, it cannot be a selection
					state = MovingPoint;
					pVec = p;
					return;
				}

				float dist3;
				Circle *p2 = getClosestCircle(info, pos, &dist3);
				if (dist3 < dist2 && dist3 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					if (selection.contains(p2) && selection.size() > 1)
					{
						state = MovingSelection;
						movingSelectionReferencePoint = p2->center;
						return;
					}

					if (!info->shiftPressed)  selection.clear();
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
					float dl1 = d2(pos, pL->p[0]);
					float dl2 = d2(pos, pL->p[1]);

					if (dl1 < dl2)
					{
						pVec = &pL->p[0];
					}
					else
					{
						pVec = &pL->p[1];
					}

					if (selection.contains(pL))
					{
						state = MovingSelection;
						movingSelectionReferencePoint = *pVec;
						return;
					}

					if (!info->shiftPressed)  selection.clear();
					selection.add(pL);
					state = SelectElement;
					possibleNextState = MovingLine;
					pLine = pL;

					return;
				}

				selection.clear();

				state = SelectionRectangle;
				selectionRectangle.left = pos.x;
				selectionRectangle.top = pos.y;
			}
			else if (state == MovingCopy)
			{
				selection.clear();
				copyCopyInfoToSelectionAndAppInfo(&selection, info, &copyInfo);
				copyInfo.clear();
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
			else if (state == SelectionRectangle)
			{
				selection = getSelectionFromRectangle(selectionRectangle, info);
				state = Point;
				selectionRectangle = {};
			}
			else if (state == MovingSelection)
			{
				state = Point;
			}

			break;
		}
		case sf::Event::KeyPressed:
		{
			if (e.key.code == sf::Keyboard::D)
			{
				removeSelection(&selection, info);
			}
			else if (e.key.code == sf::Keyboard::Y)
			{
				referencePoint = selection.closestPoint(info->snappedPos);
				copyInfo.clear();
				copySelectionToCopyInfo(&copyInfo, &selection);
				state = MovingCopy;
			}

			break;
		}

		default: {}
	}
}

void EditMode::onEnter()
{
	state = Point;
	selection.clear();
}

void EditMode::onExit()
{
	if (copyInfo.size())
	{
		selection.clear();
		copyCopyInfoToSelectionAndAppInfo(&selection, info, &copyInfo);
		copyInfo.clear();
	}

	state = Point;
	selection.clear();
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

	if (state == SelectionRectangle)
		return "Selection Rectangle";

	if (state == MovingSelection)
		return "Moving Selection";

	if (state == MovingCopy)
		return "Moving Copy";

	return "Error";
}


void drawCirclesSelection(const std::vector<Circle*>& circles, sf::RenderWindow *window);
void drawLinesSelection(const std::vector<Line*>& lines, sf::RenderWindow *window);
void drawTextsSelection(const std::vector<Text*>& texts, sf::RenderWindow *window, bool drawBack=true);

void EditMode::beforeDraw()
{
	if (state == SelectionRectangle)
	{
		sf::RectangleShape shape;
		shape.setPosition(selectionRectangle.left, selectionRectangle.top);
		shape.setSize(sf::Vector2f(selectionRectangle.width, selectionRectangle.height));
		shape.setFillColor(selectionColor);

		info->window->draw(shape);
	}
	else
	{
		// draw selection
		drawLinesSelection(selection.lines, info->window);
		drawCirclesSelection(selection.circles, info->window);
		drawTextsSelection(selection.texts, info->window);
	}

	// draw copyInfo
	drawLines(copyInfo.lines, info->window);
	drawCircles(copyInfo.circles, info->window);
	drawTexts(copyInfo.texts, info->window);
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

