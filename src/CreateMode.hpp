#pragma once

#include "AppInfo.hpp"
#include "utils.hpp"


struct CreateMode : public Mode
{
	enum State {
		Line,
		NewLine,
		Circle,
		NewCircle,
		Text,
		NewText,
	};

	State state = Line;

	CreateMode(sf::Keyboard::Key _k, AppInfo *_info) : Mode(_k, _info) {}

	virtual void onEvent(sf::Event& e) override;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual std::string getModeDescription() override;

	void checkText();

};


