#pragma once

#include "AppInfo.hpp"


struct EditMode : public Mode
{
	EditMode(sf::Keyboard::Key _k) : Mode(_k) {}

	virtual void onEvent(struct AppInfo * info, sf::Event& e) override;
	virtual void onEnter(struct AppInfo *) override;
	virtual void onExit(struct AppInfo *) override;
	virtual std::string getModeDescription() override;

};
