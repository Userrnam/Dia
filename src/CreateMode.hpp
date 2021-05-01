#pragma once

#include "AppInfo.hpp"
#include "utils.hpp"


struct CreateMode : public Mode
{
	enum State {
		Line,
		NewLine,
		Circle,
		NewCircle
	};

	State state = Line;

	CreateMode(sf::Keyboard::Key _k) : Mode(_k) {}

	virtual void onEvent(struct AppInfo *info, sf::Event& e) override;
	virtual void onEnter(struct AppInfo *) override;
	virtual void onExit(struct AppInfo *) override;
	virtual std::string getModeDescription() override;

};


