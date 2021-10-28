#pragma once

#include "Elements.hpp"

const sf::Color cursorColor = sf::Color(255, 0, 0);

struct TextEdit
{
	Text* pText;
	int cursorPos = -1;

	void setText(Text *t)
	{
		pText = t;
		cursorPos = t->text.getString().getSize();
	}

	void handleKey(const sf::Event& e);
	sf::FloatRect getCursor() const;
};

