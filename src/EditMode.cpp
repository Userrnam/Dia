#include "EditMode.hpp"
#include "utils.hpp"
#include "Drawer.hpp"

#define MAX_SELECT_DISTANCE 20.0f

const sf::Color selectionColor = sf::Color(70, 70, 180, 90);

void copyCopyInfoToSelectionAndAppInfo(Selection* selection, AppInfo* info, CopyInfo* copyInfo)
{
	// we also must use different ids for copied elements

	// insure that pointers in selection will be correct
	info->lines.reserve(info->lines.size() + copyInfo->lines.size() + 10);
	for (auto line : copyInfo->lines)
	{
		info->lines.push_back(line);
		info->elementId++;
		info->lines.back().id = info->elementId;
		selection->lines.push_back(&info->lines.back());
	}

	info->circles.reserve(info->circles.size() + copyInfo->circles.size() + 10);
	for (auto circles : copyInfo->circles)
	{
		info->circles.push_back(circles);
		info->elementId++;
		info->circles.back().id = info->elementId;
		selection->circles.push_back(&info->circles.back());
	}

	info->texts.reserve(info->texts.size() + copyInfo->texts.size() + 10);
	for (auto text : copyInfo->texts)
	{
		info->texts.push_back(text);
		info->elementId++;
		info->texts.back().id = info->elementId;
		selection->texts.push_back(&info->texts.back());
	}
}

void addTimeFrameFromSelection(AppInfo* info, ChangeType t)
{
	Change change;
	std::vector<Change> changes;

	change.elementType = ElementType::Line;
	for (auto element : info->selection.lines)
	{
		change.setPreviousValue(*element);
		changes.push_back(change);
	}
	change.elementType = ElementType::Circle;
	for (auto element : info->selection.circles)
	{
		change.setPreviousValue(*element);
		changes.push_back(change);
	}
	change.elementType = ElementType::Text;
	for (auto element : info->selection.texts)
	{
		change.setPreviousValue(*element);
		changes.push_back(change);
	}

	info->history.addChanges(changes, t);
}

void copySelectionToCopyInfo(CopyInfo* copyInfo, Selection* selection)
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

void getSelectionFromRectangle(sf::FloatRect selectionRectangle, Selection* selection, AppInfo* info)
{
	for (auto& line : info->lines)
	{
		if (selectionRectangle.contains(line.p[0]) && selectionRectangle.contains(line.p[1]))
		{
			selection->add(&line);
		}
	}

	for (auto& circle : info->circles)
	{
		if (selectionRectangle.contains(sf::Vector2f(circle.center.x + circle.radius, circle.center.y + circle.radius)) &&
			selectionRectangle.contains(sf::Vector2f(circle.center.x - circle.radius, circle.center.y - circle.radius))
			)
		{
			selection->add(&circle);
		}
	}

	for (auto& text : info->texts)
	{
		auto bounds = text.bounding;
		if (selectionRectangle.contains(sf::Vector2f(bounds.left, bounds.top)) &&
			selectionRectangle.contains(sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height))
			)
		{
			selection->add(&text);
		}
	}
}

void removeSelection(Selection* pSelection, AppInfo* info)
{
	if (pSelection->size() == 0)  return;

	Change change;
	std::vector<Change> changes;

	std::sort(pSelection->lines.begin(), pSelection->lines.end());
	int i = 0;
	change.elementType = ElementType::Line;
	for (auto line : pSelection->lines)
	{
		int index = line - info->lines.data() - i;

		change.setPreviousValue(info->lines[index]);
		changes.push_back(change);

		info->lines.erase(info->lines.begin() + index);
		i++;
	}

	std::sort(pSelection->circles.begin(), pSelection->circles.end());
	i = 0;
	change.elementType = ElementType::Circle;
	for (auto circle : pSelection->circles)
	{
		change.setPreviousValue(*circle);
		changes.push_back(change);

		int index = circle - info->circles.data() - i;
		info->circles.erase(info->circles.begin() + index);
		i++;
	}

	std::sort(pSelection->texts.begin(), pSelection->texts.end());
	i = 0;
	change.elementType = ElementType::Text;
	for (auto text : pSelection->texts)
	{
		change.setPreviousValue(*text);
		changes.push_back(change);

		int index = text - info->texts.data() - i;
		info->texts.erase(info->texts.begin() + index);
		i++;
	}

	info->history.addChanges(changes, ChangeType::Delete);

	pSelection->clear();
}

static void handleMouseMoveEvent(AppInfo* info, sf::Event& e)
{
	sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
	auto vec = snap(info, mousePos);

	if (info->state == State::EMovingLinePoint)
	{
		*info->pVec = vec;
	} 
	else if (info->state == State::EMovingCirclePoint)
	{
		auto move = vec - info->referencePoint; // *info->pVec; // use refference point here
		auto v = snap(info, *info->pVec + move) - *info->pVec - move;
		info->referencePoint += move + v;
		*info->pVec += move + v;
	}
	else if (info->state == State::EChangingCircleRadius)
	{
		info->pCircle->radius = sqrt(d2(vec, info->pCircle->center));
	}
	else if (info->state == State::EMovingText)
	{
		//info->pText->text.setPosition(vec);
		auto move = vec - info->referencePoint; // *info->pVec; // use refference point here
		auto v = snap(info, info->pText->text.getPosition() + move) - info->pText->text.getPosition() - move;
		info->referencePoint += move + v;
		info->pText->text.move(move + v);
		info->pText->bounding = info->pText->text.getGlobalBounds();
	}
	else if (info->state == State::EMovingLine)
	{
		auto move = vec - info->referencePoint; // *info->pVec; // use refference point here
		auto v = snap(info, *info->pVec + move) - *info->pVec - move;
		info->referencePoint += move + v;
		info->pLine->p[0] += move + v;
		info->pLine->p[1] += move + v;
	}
	else if (info->state == State::ESelectElement)
	{
		info->state = info->possibleNextState;
	}
	else if (info->state == State::ESelectionRectangle)
	{
		info->selectionRectangle.width = mousePos.x - info->selectionRectangle.left;
		info->selectionRectangle.height = mousePos.y - info->selectionRectangle.top;
	}
	else if (info->state == State::EMovingSelection)
	{
		//auto mov = vec - info->movingSelectionReferencePoint;
		//info->movingSelectionReferencePoint = vec;
		auto move = vec - info->referencePoint; // *info->pVec; // use refference point here
		auto v = snap(info, info->movingSelectionReferencePoint + move) - info->movingSelectionReferencePoint - move;
		info->referencePoint += move + v;
		info->movingSelectionReferencePoint += move + v;
		info->selection.move(move + v);
	}
	else if (info->state == State::EMovingCopy)
	{
		auto mov = vec - info->referencePoint;
		info->referencePoint = vec;
		info->copyInfo.move(mov);
	}
}

static void handleButtonPressed(AppInfo* info, sf::Event& e)
{
	if (e.mouseButton.button != sf::Mouse::Button::Right)
		std::cout << "(3240)Error: Unexpected Mouse Button: " << (int)e.mouseButton.button << std::endl;

	// TODO When is it true?
	if (info->state == State::ESelectEnd)
		info->state = State::EPoint;

	sf::Vector2f pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
	if (info->state == State::EPoint || info->shiftPressed && info->state == State::EMovingSelection
		|| info->state == State::EEditText || info->state == State::ESelectionRectangle)
	{
		if (info->state == State::EEditText)
		{
			if (info->selection.texts.back()->text.getString().getSize() == 0)
			{
				removeSelection(&info->selection, info);
				info->selection.clear();
			}
			else
			{
				Change change;
				change.elementType = ElementType::Text;
				change.setPreviousValue(*info->selection.texts.back());

				info->history.addChanges({ change }, ChangeType::Edit);
			}
		}

		// check texts
		for (auto& t : info->texts)
		{
			if (t.bounding.contains(pos))
			{
				if (info->selection.contains(&t))
				{
					info->state = State::EMovingSelection;
					info->referencePoint = pos;
					info->movingSelectionReferencePoint = sf::Vector2f(t.bounding.left, t.bounding.top);
				}
				else
				{
					if (!info->shiftPressed)  info->selection.clear();
					info->selection.add(&t);
					info->state = State::ESelectElement;
					info->possibleNextState = State::EMovingText;
					info->pText = &t;
					info->referencePoint = pos;
				}
				return;
			}
		}

		// check points at the ends of lines
		{
			Line* line;
			float dist2;
			sf::Vector2f* p = getClosestLinePoint(info, pos, &dist2, &line);
			auto dmax = MAX_SELECT_DISTANCE * info->cameraZoom;
			if (dist2 < dmax * dmax)
			{
				if (info->selection.contains(line) && info->selection.size() > 1)
				{
					info->state = State::EMovingSelection;
					info->referencePoint = *p;
					info->movingSelectionReferencePoint = *p;
					return;
				}

				// if user clicks on point, it cannot be a selection
				info->state = State::EMovingLinePoint;
				info->pLine = line;
				info->pVec = p;
				info->referencePoint = pos;
				return;
			}
		}

		// check circles
		{
			float dist2;
			Circle* circle = getClosestCircle(info, pos, &dist2);
			auto dmax = MAX_SELECT_DISTANCE * info->cameraZoom;
			if (dist2 < dmax * dmax)
			{
				if (info->selection.contains(circle) && info->selection.size() > 1)
				{
					info->state = State::EMovingSelection;
					info->referencePoint = pos;
					info->movingSelectionReferencePoint = circle->center;
					return;
				}

				if (!info->shiftPressed)  info->selection.clear();
				info->selection.add(circle);
				info->state = State::ESelectElement;
				info->possibleNextState = State::EMovingCirclePoint;
				info->pCircle = circle;
				info->pVec = &circle->center;
				info->referencePoint = pos;

				if (info->shiftPressed)
				{
					info->possibleNextState = State::EChangingCircleRadius;
				}
				return;
			}
		}

		// check line
		{
			float dist2;
			Line* line = getClosestLine(info, pos, &dist2);

			auto dmax = MAX_SELECT_DISTANCE * info->cameraZoom;
			if (dist2 < dmax * dmax)
			{
				float dl1 = d2(pos, line->p[0]);
				float dl2 = d2(pos, line->p[1]);

				if (dl1 < dl2)
				{
					info->pVec = &line->p[0];
				}
				else
				{
					info->pVec = &line->p[1];
				}

				if (info->selection.contains(line))
				{
					info->state = State::EMovingSelection;
					info->referencePoint = pos;
					info->movingSelectionReferencePoint = *info->pVec;
					return;
				}

				if (!info->shiftPressed)  info->selection.clear();
				info->selection.add(line);
				info->state = State::ESelectElement;
				info->possibleNextState = State::EMovingLine;
				info->pLine = line;
				info->referencePoint = pos; // nocheck

				return;
			}
		}

		// if we did't hit anything, start selection
		// rectangle

		if (!info->shiftPressed)
			info->selection.clear();

		info->state = State::ESelectionRectangle;
		info->selectionRectangle.left = pos.x;
		info->selectionRectangle.top = pos.y;
	}
	else if (info->state == State::EMovingCopy)
	{
		info->selection.clear();
		copyCopyInfoToSelectionAndAppInfo(&info->selection, info, &info->copyInfo);

		addTimeFrameFromSelection(info, ChangeType::Create);

		info->copyInfo.clear();
		info->state = State::EPoint;
	}
}

static void handleButtonRelease(AppInfo* info, sf::Event& e)
{
	Change change;

	if (info->state == State::EMovingLinePoint || info->state == State::EMovingCirclePoint)
	{
		if (info->state == State::EMovingLinePoint)
		{
			change.elementType = ElementType::Line;
			change.setPreviousValue(*info->pLine);
		}
		else if (info->state == State::EMovingCirclePoint)
		{
			change.elementType = ElementType::Circle;
			change.setPreviousValue(*info->pCircle);
		}

		info->history.addChanges({ change }, ChangeType::Edit);

		// We could move here to CreateMode
		info->state = State::EPoint;
		info->pVec = nullptr;
		// FIXME: 2 points of the same line may be the same.
		// If this happens, the line should be deleted
	}
	else if (info->state == State::EChangingCircleRadius)
	{
		change.elementType = ElementType::Circle;
		change.setPreviousValue(*info->pCircle);
		info->history.addChanges({ change }, ChangeType::Edit);

		info->state = State::EPoint;
		info->pCircle = nullptr;
	}
	else if (info->state == State::EMovingText)
	{
		change.elementType = ElementType::Text;
		change.setPreviousValue(*info->pText);
		info->history.addChanges({ change }, ChangeType::Edit);

		info->state = State::EPoint;
		info->pText = nullptr;
	}
	else if (info->state == State::EMovingLine)
	{
		change.elementType = ElementType::Line;
		change.setPreviousValue(*info->pLine);
		info->history.addChanges({ change }, ChangeType::Edit);

		info->state = State::EPoint;
		info->pLine = nullptr;
	}
	else if (info->state == State::ESelectElement)
	{
		info->state = State::ESelectEnd;
	}
	else if (info->state == State::ESelectionRectangle)
	{
		getSelectionFromRectangle(info->selectionRectangle, &info->selection, info);
		info->state = State::EPoint;
		info->selectionRectangle = {};
	}
	else if (info->state == State::EMovingSelection)
	{
		addTimeFrameFromSelection(info, ChangeType::Edit);

		info->state = State::EPoint;
	}
	else if (info->state == State::EEditText)
	{
		std::cout << "(3902)Error: Unhandled case" << std::endl;
	}
}

static void handleKeyPress(AppInfo* info, sf::Event& e)
{
	if (info->state != State::EEditText)
	{
		if (e.key.code == sf::Keyboard::D || e.key.code == sf::Keyboard::Delete)
		{
			removeSelection(&info->selection, info);
		}
		else if (e.key.code == sf::Keyboard::Y)
		{
			info->referencePoint = info->selection.closestPoint(info->snappedPos);
			info->copyInfo.clear();
			copySelectionToCopyInfo(&info->copyInfo, &info->selection);
			info->state = State::EMovingCopy;

			auto mov = info->snappedPos - info->referencePoint;
			info->referencePoint = info->snappedPos;
			info->copyInfo.move(mov);
		}
		else if (e.key.code == sf::Keyboard::T)
		{
			if (info->selection.size() == 1 && info->selection.texts.size() == 1)
			{
				info->state = State::EEditText;
				info->textEdit.setText(info->selection.texts.back());
			}
		}
	}
	else
	{
		info->textEdit.handleKey(e);
	}
}

void onEditEvent(AppInfo* info, sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::MouseMoved:
		handleMouseMoveEvent(info, e);
		break;

	case sf::Event::MouseButtonPressed:
		handleButtonPressed(info, e);
		break;

	case sf::Event::MouseButtonReleased:
		handleButtonRelease(info, e);
		break;

	case sf::Event::KeyPressed:
		handleKeyPress(info, e);
		break;

	default: {}
	}
}

void onEditEnter(AppInfo* info)
{
	info->state = State::EPoint;
	info->selection.clear();
}

void onEditExit(AppInfo* info)
{
	// copy paste from handleButtonRelease
	Change change;

	if (info->state == State::EMovingLinePoint || info->state == State::EMovingCirclePoint)
	{
		if (info->state == State::EMovingLinePoint)
		{
			change.elementType = ElementType::Line;
			change.setPreviousValue(*info->pLine);
		}
		else if (info->state == State::EMovingCirclePoint)
		{
			change.elementType = ElementType::Circle;
			change.setPreviousValue(*info->pCircle);
		}

		info->history.addChanges({ change }, ChangeType::Edit);

		// We could move here to CreateMode
		info->state = State::EPoint;
		info->pVec = nullptr;
		// FIXME: 2 points of the same line may be the same.
		// If this happens, the line should be deleted
	}
	else if (info->state == State::EChangingCircleRadius)
	{
		change.elementType = ElementType::Circle;
		change.setPreviousValue(*info->pCircle);
		info->history.addChanges({ change }, ChangeType::Edit);

		info->state = State::EPoint;
		info->pCircle = nullptr;
	}
	else if (info->state == State::EMovingText)
	{
		change.elementType = ElementType::Text;
		change.setPreviousValue(*info->pText);
		info->history.addChanges({ change }, ChangeType::Edit);

		info->state = State::EPoint;
		info->pText = nullptr;
	}
	else if (info->state == State::EMovingLine)
	{
		change.elementType = ElementType::Line;
		change.setPreviousValue(*info->pLine);
		info->history.addChanges({ change }, ChangeType::Edit);

		info->state = State::EPoint;
		info->pLine = nullptr;
	}
	else if (info->state == State::EMovingSelection)
	{
		addTimeFrameFromSelection(info, ChangeType::Edit);

		info->state = State::EPoint;
	}

	// copy from handleButtonPressed
	else if (info->state == State::EEditText)
	{
		if (info->selection.texts.back()->text.getString().getSize() == 0)
		{
			removeSelection(&info->selection, info);
			info->selection.clear();
		}
		else
		{
			Change change;
			change.elementType = ElementType::Text;
			change.setPreviousValue(*info->selection.texts.back());

			info->history.addChanges({ change }, ChangeType::Edit);
		}
	}

	if (info->copyInfo.size())
	{
		info->selection.clear();
		copyCopyInfoToSelectionAndAppInfo(&info->selection, info, &info->copyInfo);
		addTimeFrameFromSelection(info, ChangeType::Create);
		info->copyInfo.clear();
	}

	info->state = State::EPoint;
	info->selection.clear();
}

void drawCirclesSelection(const std::vector<Circle*>& circles, sf::RenderWindow* window);
void drawLinesSelection(const std::vector<Line*>& lines, sf::RenderWindow* window);
void drawTextsSelection(const std::vector<Text*>& texts, sf::RenderWindow* window, bool drawBack = true);

void editBeforeDraw(AppInfo* info)
{
	if (info->state == State::ESelectionRectangle)
	{
		sf::RectangleShape shape;
		shape.setPosition(info->selectionRectangle.left, info->selectionRectangle.top);
		shape.setSize(sf::Vector2f(info->selectionRectangle.width, info->selectionRectangle.height));
		shape.setFillColor(selectionColor);

		info->window->draw(shape);
	}
	else if (info->state == State::EEditText)
	{
		auto cursorRect = info->textEdit.getCursor();
		sf::RectangleShape shape;
		shape.setPosition(cursorRect.left, cursorRect.top);
		shape.setSize(sf::Vector2f(cursorRect.width, cursorRect.height));
		shape.setFillColor(cursorColor);

		info->window->draw(shape);
	}

	// draw selection
	drawLinesSelection(info->selection.lines, info->window);
	drawCirclesSelection(info->selection.circles, info->window);
	drawTextsSelection(info->selection.texts, info->window);

	// draw copyInfo
	drawLines(info->copyInfo.lines, info->window);
	drawCircles(info->copyInfo.circles, info->window);
	drawTexts(info->copyInfo.texts, info->window);
}

void drawCirclesSelection(const std::vector<Circle*>& circles, sf::RenderWindow* window)
{
	sf::CircleShape shape;

	shape.setOutlineColor(selectionColor);
	shape.setFillColor(sf::Color(0, 0, 0, 0));

	for (auto c : circles)
	{
		auto& circle = *c;

		shape.setRadius(circle.radius - circle.outlineThickness / 2 - 5);
		shape.setOutlineThickness(circle.outlineThickness + 10);

		shape.setPosition(circle.center);
		shape.setOrigin(shape.getRadius(), shape.getRadius());

		window->draw(shape);
	}
}

void drawLinesSelection(const std::vector<Line*>& lines, sf::RenderWindow* window)
{
	sf::VertexArray va;
	va.setPrimitiveType(sf::PrimitiveType::Quads);
	va.resize(lines.size() * 4);

	int k = 0;
	for (auto l : lines)
	{
		auto& line = *l;

		sf::Vector2f normal;
		normal.x = -(line.p[0] - line.p[1]).y;
		normal.y = (line.p[0] - line.p[1]).x;
		normal = normalize(normal) * (line.width / 2.0f + 5);

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

void drawTextsSelection(const std::vector<Text*>& texts, sf::RenderWindow* window, bool drawBack)
{
	sf::RectangleShape rect;
	rect.setFillColor(selectionColor);

	for (auto& t : texts)
	{
		rect.setPosition(t->bounding.left - 5, t->bounding.top - 5);
		rect.setSize(sf::Vector2f(t->bounding.width + 10, t->bounding.height + 10));
		window->draw(rect);
	}
}
