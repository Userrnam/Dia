#include "CreateMode.hpp"
#include "utils.hpp"


float calcRadius(sf::Vector2f center, sf::Vector2f point)
{
	return sqrt(d2(center, point));
}

bool checkText(AppInfo*);

static void handleMouseMove(AppInfo* info, sf::Event& e)
{
	sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
	if (info->state == State::CNewLine)
	{
		info->lines.back().p[1] = snap(info, mousePos);
	}
	else if (info->state == State::CNewCircle)
	{
		info->circles.back().radius = calcRadius(info->circles.back().center, snap(info, mousePos));
	}
}

static void handleButtonPress(AppInfo* info, sf::Event& e)
{
	if (e.mouseButton.button != sf::Mouse::Button::Left)
	{
		return;
	}
	auto pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
	if (info->state == State::CLine)
	{
		info->previousState = info->state;
		info->state = State::CNewLine;
		info->elementId++;

		info->lines.push_back({});
		info->lines.back().width = info->defaults.line.width;
		info->lines.back().color = info->defaults.line.color;
		info->lines.back().p[0] = snap(info, pos);
		info->lines.back().p[1] = info->lines.back().p[0];
		info->lines.back().id = info->elementId;
	}
	else if (info->state == State::CCircle)
	{
		info->previousState = info->state;
		info->state = State::CNewCircle;
		info->elementId++;

		info->circles.push_back({});
		info->circles.back().outlineColor = info->defaults.circle.outlineColor;
		info->circles.back().color = info->defaults.circle.color;
		info->circles.back().outlineThickness = info->defaults.circle.outlineThickness;
		info->circles.back().center = snap(info, pos);
		info->circles.back().radius = 0;
		info->circles.back().id = info->elementId;
	}
	else if (info->state == State::CText || info->state == State::CNewText)
	{
		if (checkText(info) && (info->previousState == State::CText || info->previousState == State::CNewText))
		{
			Change change;
			change.elementType = ElementType::Text;
			change.elementId = info->texts.back().id;
			change.setPreviousValue(info->texts.back());

			info->history.addChanges({ change }, ChangeType::Create);
		}

		info->previousState = info->state;
		info->state = State::CNewText;
		info->elementId++;

		info->texts.push_back({});
		info->texts.back().text.setFont(info->font);
		// FIXME add ability to change default character size
		info->texts.back().text.setCharacterSize(info->defaults.text.size);
		info->texts.back().text.setFillColor(info->defaults.text.color);
		info->texts.back().text.setPosition(snap(info, pos));
		info->texts.back().id = info->elementId;
	}
}

static void handleButtonRelease(AppInfo* info, sf::Event& e)
{
	if (e.mouseButton.button != sf::Mouse::Button::Left)
	{
		std::cout << "(0342)Error: Unexpected Mouse Button" << std::endl;
		return;
	}
	if (info->state == State::CNewLine)
	{
		info->lines.back().p[1] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
		info->previousState = info->state;
		info->state = State::CLine;
		if (same(info->lines.back().p[0], info->lines.back().p[1]))
		{
			info->lines.pop_back();
		}
		else
		{
			Change change;
			change.elementType = ElementType::Line;
			change.elementId = info->lines.back().id;
			change.setPreviousValue(info->lines.back());

			info->history.addChanges({ change }, ChangeType::Create);
		}
	}
	else if (info->state == State::CNewCircle)
	{
		info->circles.back().radius = calcRadius(info->circles.back().center,
			snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y)));

		info->previousState = info->state;
		info->state = State::CCircle;
		if (info->circles.back().radius < EPS)
		{
			info->circles.pop_back();
		}
		else
		{
			Change change;
			change.elementType = ElementType::Circle;
			change.elementId = info->circles.back().id;
			change.setPreviousValue(info->circles.back());

			info->history.addChanges({ change }, ChangeType::Create);
		}
	}
}

static void handleKeyPress(AppInfo* info, sf::Event& e)
{
	if (info->state != State::CNewText)
	{
		if (e.key.code == sf::Keyboard::L)
		{
			info->previousState = info->state;
			info->state = State::CLine;
		}
		else if (e.key.code == sf::Keyboard::C)
		{
			info->previousState = info->state;
			info->state = State::CCircle;
		}
		else if (e.key.code == sf::Keyboard::T)
		{
			info->previousState = info->state;
			info->state = State::CText;
		}
	}
	else
	{
		auto s = info->texts.back().text.getString();
		if (e.key.code == sf::Keyboard::BackSpace)
		{
			if (s.getSize() == 0)
			{
				return;
			}

			s.erase(s.getSize() - 1);
			info->texts.back().text.setString(s);
		}

		if (charPrintable(e.key))
		{
			info->texts.back().text.setString(s + getCharFromKeyEvent(e.key));
		}

		updateBoundingBox(&info->texts.back());
	}
}

void onCreateEvent(AppInfo* info, sf::Event& e) {
	switch (e.type)
	{
	case sf::Event::MouseMoved:
		handleMouseMove(info, e);
		break;

	case sf::Event::MouseButtonPressed:
		handleButtonPress(info, e);
		break;

	case sf::Event::MouseButtonReleased:
		handleButtonRelease(info, e);
		break;

	case sf::Event::KeyPressed:
		handleKeyPress(info, e);
		break;

	default: {}
	}
}


void onCreateEnter(AppInfo* info)
{
	info->state = State::CLine;
	info->previousState = State::CLine;
}

void onCreateExit(AppInfo* info)
{
	// it's there because user can enter <Control-C>
	// and current mode will be restarted
	if (checkText(info) && (info->state == State::CText || info->state == State::CNewText))
	{
		if (info->history.timeFrames.size() == 0
			|| info->history.timeFrames.back().changes.back().elementId != info->texts.back().id)
		{
			Change change;
			change.elementType = ElementType::Text;
			change.elementId = info->texts.back().id;
			change.setPreviousValue(info->texts.back());

			info->history.addChanges({ change }, ChangeType::Create);
		}
	}
	if (info->state == State::CNewLine)
	{
		Change change;
		change.elementType = ElementType::Line;
		change.elementId = info->lines.back().id;
		change.setPreviousValue(info->lines.back());

		info->history.addChanges({ change }, ChangeType::Create);
	}
	else if (info->state == State::CNewCircle)
	{
		Change change;
		change.elementType = ElementType::Circle;
		change.elementId = info->circles.back().id;
		change.setPreviousValue(info->circles.back());

		info->history.addChanges({ change }, ChangeType::Create);
	}
}

// returns false if text last text was empty
// or texts is empty
bool checkText(AppInfo* info)
{
	if (info->state == State::CText || info->state == State::CNewText)
	{
		if (info->texts.size() > 0)
		{
			if (info->texts.back().text.getString().getSize() == 0)
			{
				info->texts.pop_back();
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}
