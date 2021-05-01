#pragma once

#include "AppInfo.hpp"


struct EditMode : public Mode
{
	sf::Vector2f *pVec = 0;

	enum State
	{
		Point,
		MovingPoint,
		ChangingCircleRadius,
		MovingText,
		// TODO add text editing
	};

	State state = Point;
	Circle *pCircle = nullptr;
	Text   *pText   = nullptr;

	EditMode(sf::Keyboard::Key _k, AppInfo *_info) : Mode(_k, _info) {}

	virtual void onEvent(sf::Event& e) override;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual std::string getModeDescription() override;

};

