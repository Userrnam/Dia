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
	
	bool contains(Line *line);
	bool contains(Circle *circle);
	bool contains(Text *text);

	void move(sf::Vector2f vec);

	sf::Vector2f closestPoint(sf::Vector2f v);

	int size()
	{
		return lines.size() + circles.size() + texts.size();
	}
};

struct CopyInfo
{
	std::vector<Line> lines;
	std::vector<Circle> circles;
	std::vector<Text> texts;

	void clear()
	{
		lines.clear();
		circles.clear();
		texts.clear();
	}

	int size()
	{
		return lines.size() + circles.size() + texts.size();
	}

	void move(sf::Vector2f mov);
};

struct EditMode : public Mode
{
	sf::Vector2f *pVec = 0;

	Selection selection;
	sf::FloatRect selectionRectangle;

	CopyInfo copyInfo;

	enum State
	{
		Point,
		MovingPoint,
		MovingLine,
		MovingSelection,
		SelectElement,
		SelectEnd,
		SelectionRectangle,
		ChangingCircleRadius,
		MovingText,
		MovingCopy,
		// TODO add text editing
	};

	sf::Vector2f movingSelectionReferencePoint;

	State state = Point;
	State possibleNextState = Point;
	Circle *pCircle = nullptr;
	Text   *pText   = nullptr;
	Line   *pLine   = nullptr;
	sf::Vector2f point;
	sf::Vector2f referencePoint;

	EditMode(sf::Keyboard::Key _k, AppInfo *_info) : Mode(_k, _info) {}

	virtual void onEvent(sf::Event& e) override;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual std::string getModeDescription() override;

	virtual void beforeDraw() override;
};

