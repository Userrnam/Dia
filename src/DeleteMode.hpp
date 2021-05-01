#pragma once

#include "AppInfo.hpp"


// TODO replace this with select mode?
// handle circles
struct DeleteMode : public Mode
{
	DeleteMode(sf::Keyboard::Key _k, AppInfo *_info) : Mode(_k, _info) {}

	virtual void onEvent(sf::Event& e) override;
	virtual void onEnter() override	{}
	virtual void onExit() override {}

	virtual std::string getModeDescription() override;
};

