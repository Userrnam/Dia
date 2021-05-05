#pragma once

#include "AppInfo.hpp"


struct Selection
{
	std::vector<Line*> lines;
	std::vector<Circle*> circles;
	std::vector<Text*> texts;

	void clear()
	{
		lines = {};
		circles = {};
		texts = {};
	}

	void add(Line *line);
	void add(Circle *circle);
	void add(Text *text);
};

struct EditMode : public Mode
{
	sf::Vector2f *pVec = 0;

	Selection selection;
	sf::FloatRect selectionRectangle;

	enum State
	{
		Point,
		MovingPoint,
		MovingLine,
		SelectElement,
		SelectEnd,
		SelectionRectangle,
		ChangingCircleRadius,
		MovingText,
		// TODO add text editing
	};

	State state = Point;
	State possibleNextState = Point;
	Circle *pCircle = nullptr;
	Text   *pText   = nullptr;
	Line   *pLine   = nullptr;
	sf::Vector2f point;

	EditMode(sf::Keyboard::Key _k, AppInfo *_info) : Mode(_k, _info) {}

	virtual void onEvent(sf::Event& e) override;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual std::string getModeDescription() override;

	virtual void beforeDraw() override;
};

