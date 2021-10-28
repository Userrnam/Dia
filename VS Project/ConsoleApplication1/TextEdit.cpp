#include "TextEdit.hpp"
#include "utils.hpp"


void TextEdit::handleKey(const sf::Event& e)
{
	auto s = pText->text.getString();
	if (e.key.code == sf::Keyboard::BackSpace)
	{
		if (cursorPos-1 < 0)
		{
			return;
		}

		s.erase(cursorPos-1);
		cursorPos--;
		pText->text.setString(s);
	}
	else if (e.key.code == sf::Keyboard::Delete)
	{
		if (cursorPos >= pText->text.getString().getSize())
		{
			return;
		}

		s.erase(cursorPos);
		pText->text.setString(s);
	}
	else if (e.key.code == sf::Keyboard::Left)
	{
		if (cursorPos - 1 >= 0)
		{
			cursorPos--;
		}
		return;
	}
	else if (e.key.code == sf::Keyboard::Right)
	{
		if (cursorPos < pText->text.getString().getSize())
		{
			cursorPos++;
		}
		return;
	}

	if (charPrintable(e.key))
	{
		std::string s = pText->text.getString();
		s.insert(s.begin() + cursorPos, getCharFromKeyEvent(e.key));
		pText->text.setString(s);
		cursorPos++;
	}

	updateBoundingBox(pText);
}

sf::FloatRect TextEdit::getCursor() const
{
	sf::FloatRect fr;
	auto pos = pText->text.findCharacterPos(cursorPos);
	fr.left = pos.x;
	fr.top  = pos.y;

	// FIXME this is dumb
	sf::Text dummy;
	dummy.setString("|");
	dummy.setFont(*pText->text.getFont());
	dummy.setCharacterSize(pText->text.getCharacterSize());

	auto bounds = dummy.getGlobalBounds();
	fr.width  = bounds.width;
	fr.height = bounds.height;

	return fr;
}

