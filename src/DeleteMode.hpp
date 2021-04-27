#pragma once

#include "AppInfo.hpp"


// TODO replace this with select mode?
// handle circles
struct DeleteMode : public Mode
{
	DeleteMode(sf::Keyboard::Key _k) : Mode(_k) {}

	virtual void onEvent(struct AppInfo *info, sf::Event& e) override;
	virtual void onEnter(struct AppInfo *info) override	{}
	virtual void onExit(struct AppInfo *info) override {}

	virtual std::string getModeDescription() override;
};

