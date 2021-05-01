#include <iostream>
#include <SFML/Graphics.hpp>

#include "AppInfo.hpp"

#include "DeleteMode.hpp"
#include "EditMode.hpp"
#include "CreateMode.hpp"

#include "utils.hpp"


void drawCircles(AppInfo *info)
{
	sf::CircleShape shape;

	for (auto &circle : info->circles)
	{
		shape.setRadius(circle.radius - circle.outlineThickness/2);
		shape.setOutlineThickness(circle.outlineThickness);
		shape.setOutlineColor(circle.outlineColor);
		shape.setFillColor(circle.color);

		shape.setPosition(circle.center);
		shape.setOrigin(shape.getRadius(), shape.getRadius());

		info->window->draw(shape);
	}
}

void drawLines(AppInfo *info)
{
	sf::VertexArray va;
	va.setPrimitiveType(sf::PrimitiveType::Quads);
	va.resize(info->lines.size() * 4);

	int k = 0;
	for (auto line : info->lines)
	{
		sf::Vector2f normal;
		normal.x = -(line.p[0] - line.p[1]).y;
		normal.y = (line.p[0] - line.p[1]).x;
		normal   = normalize(normal) * line.width/2.0f;

		va[k].color = line.color;
		va[k].position = line.p[0];
		va[k].position -= normal;
		k++;

		va[k].color = line.color;
		va[k].position = line.p[1];
		va[k].position -= normal;
		k++;

		va[k].color = line.color;
		va[k].position = line.p[1];
		va[k].position += normal;
		k++;

		va[k].color = line.color;
		va[k].position = line.p[0];
		va[k].position += normal;
		k++;

	}

	info->window->draw(va);
}

void drawText(AppInfo *info)
{
	sf::RectangleShape rect;
	rect.setFillColor(sf::Color(170, 170, 0, 50));

	for (auto& t : info->texts)
	{
		rect.setPosition(t.bounding.left, t.bounding.top);
		rect.setSize(sf::Vector2f(t.bounding.width, t.bounding.height));
		info->window->draw(rect);
		info->window->draw(t.text);
	}
}

void drawGrid(AppInfo *info)
{
	// calculate total number of lines
	int size = info->gridSize;
	int count_x = info->windowSize.x / size;
	int count_y = info->windowSize.y / size;

	sf::VertexArray va;
	va.resize(2*(count_x + count_y));
	va.setPrimitiveType(sf::PrimitiveType::Lines);

	int k = 0;

	// horizontal lines
	for (int y = 1; y < count_y+1; ++y)
	{
		sf::Vertex v;
		v.position = sf::Vector2f(0, size * y);
		v.color    = sf::Color::Black;
		va[k] = v;
		k++;

		v.position = sf::Vector2f(info->windowSize.x, size * y);
		va[k] = v;
		k++;
	}

	// vertical lines
	for (int x = 1; x < count_x+1; ++x)
	{
		sf::Vertex v;
		v.position = sf::Vector2f(size * x, 0);
		v.color    = sf::Color::Black;
		va[k]      = v;
		k++;

		v.position = sf::Vector2f(size * x, info->windowSize.y);
		va[k] = v;
		k++;
	}

	info->window->draw(va);
}

void drawAppMode(AppInfo *info)
{
	sf::Text text;
	text.setFont(info->font);
	text.setCharacterSize(info->characterSize);
	text.setPosition(10, 10);
	text.setFillColor(sf::Color::Black);

	text.setString(info->pCurrentMode->getModeDescription());

	info->window->draw(text);
}

void drawSnappedPoint(AppInfo *info)
{
	sf::CircleShape circle;
	circle.setFillColor(sf::Color::Red);
	circle.setRadius(5);
	circle.setOrigin(5, 5);
	circle.setPosition(info->snappedPos);
	info->window->draw(circle);
}

const auto defaultWindowSize = sf::Vector2i(800, 600);

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(defaultWindowSize.x, defaultWindowSize.y), "DATDFWY");

	AppInfo app;
	app.window = &window;
	app.windowSize = defaultWindowSize;

	app.font.loadFromFile("resources/Hack-Regular.ttf");

	app.modes.push_back(new CreateMode(sf::Keyboard::C, &app));
	app.modes.push_back(new EditMode(sf::Keyboard::E, &app));
	app.modes.push_back(new DeleteMode(sf::Keyboard::D, &app));

	app.pCurrentMode = app.modes[0];
	app.pCurrentMode->onEnter();

	while (window.isOpen())
	{
		sf::Event e;
		if (window.waitEvent(e))
		{
			if (e.type == sf::Event::Closed)  window.close();
			if (e.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, e.size.width, e.size.height);
				window.setView(sf::View(visibleArea));
				app.windowSize = sf::Vector2i(e.size.width, e.size.height);

				continue;
			}

			if (e.type == sf::Event::MouseMoved)
			{
				app.snappedPos = snap(&app, sf::Vector2f(e.mouseMove.x, e.mouseMove.y));
			}

			if (e.type == sf::Event::KeyPressed)
			{
				if (e.key.code == sf::Keyboard::LShift || e.key.code == sf::Keyboard::RShift)
				{
					app.shiftPressed = true;
				}
				if (e.key.control)
				{
					for (auto& mi : app.modes)
					{
						if (e.key.code == mi->key)
						{
							mi->onEnter();
							app.pCurrentMode->onExit();
							app.pCurrentMode = mi;
							break;
						}
					}
					continue;
				}
			}
			else if (e.type == sf::Event::KeyReleased)
			{
				if (e.key.code == sf::Keyboard::LShift || e.key.code == sf::Keyboard::RShift)
				{
					app.shiftPressed = false;
				}
			}

			app.pCurrentMode->onEvent(e);
		}

		window.clear(sf::Color::White);

		drawGrid(&app);
		drawLines(&app);
		drawCircles(&app);
		drawText(&app);
		drawSnappedPoint(&app);

		drawAppMode(&app);

		window.display();
	}

	return 0;
}

