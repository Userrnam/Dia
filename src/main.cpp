#include <iostream>
#include <SFML/Graphics.hpp>

#include "AppInfo.hpp"

#include "EditMode.hpp"
#include "CreateMode.hpp"

#include "utils.hpp"
#include "Drawer.hpp"


void drawGrid(AppInfo *info)
{
	// calculate total number of lines
	float size = (float)info->gridSize / info->cameraZoom;
	int count_x = info->windowSize.x / size;
	int count_y = info->windowSize.y / size;

	sf::VertexArray va;
	va.resize(2*(count_x + count_y + 4));
	va.setPrimitiveType(sf::PrimitiveType::Lines);

	int k = 0;

	// calculate view displacement
	sf::Vector2f displacement = info->defaultView.getCenter() - info->camera.getCenter();
	displacement /= info->cameraZoom;

	displacement.x += (float)info->windowSize.x / 2.0f * (1.0f - 1.0f/info->cameraZoom);
	displacement.y += (float)info->windowSize.y / 2.0f * (1.0f - 1.0f/info->cameraZoom);

	const float acc = 100;
	float dx = (((int)(displacement.x * acc)) % ((int)(size * acc))) / acc;
	float dy = (((int)(displacement.y * acc)) % ((int)(size * acc))) / acc;

	displacement = sf::Vector2f(dx, dy);

	// horizontal lines
	for (int y = 0; y < count_y+2; ++y)
	{
		sf::Vertex v;
		v.position = sf::Vector2f(-size, size * y);
		v.position += displacement;
		v.color    = sf::Color::Black;
		va[k] = v;
		k++;

		v.position = sf::Vector2f(info->windowSize.x+size, size * y);
		v.position += displacement;
		va[k] = v;
		k++;
	}

	// vertical lines
	for (int x = 0; x < count_x+2; ++x)
	{
		sf::Vertex v;
		v.position = sf::Vector2f(size * x, -size);
		v.position += displacement;
		v.color    = sf::Color::Black;
		va[k]      = v;
		k++;

		v.position = sf::Vector2f(size * x, info->windowSize.y+size);
		v.position += displacement;
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
	circle.setRadius(5 * info->cameraZoom);
	circle.setOrigin(5 * info->cameraZoom, 5 * info->cameraZoom);
	circle.setPosition(info->snappedPos);
	info->window->draw(circle);
}

void drawGridSize(AppInfo *info)
{
	sf::Text text;
	text.setFont(info->font);
	text.setCharacterSize(info->characterSize);
	text.setFillColor(sf::Color::Black);

	text.setString("Grid Size: " + std::to_string(info->gridSize));

	text.setPosition(10, info->windowSize.y - 20 - text.getGlobalBounds().height);

	info->window->draw(text);
}

const auto defaultWindowSize = sf::Vector2i(800, 600);

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(defaultWindowSize.x, defaultWindowSize.y), "DATDFWY");

	AppInfo app;
	app.window = &window;
	app.windowSize = defaultWindowSize;

	{
		auto vs = sf::Vector2f(window.getSize());
		app.camera.setSize(vs);
		app.camera.setCenter(vs/2.0f);
	}

	app.defaultView = app.camera;

	app.font.loadFromFile("resources/Hack-Regular.ttf");

	app.modes.push_back(new CreateMode(sf::Keyboard::C, &app));
	app.modes.push_back(new EditMode(sf::Keyboard::E, &app));

	app.pCurrentMode = app.modes[0];
	app.pCurrentMode->onEnter();

	bool middleButtonPressed = false;
	sf::Vector2f posWhenMiddleButtonPressed;

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
				
				auto vs = sf::Vector2f(e.size.width, e.size.height);
				app.defaultView.setSize(vs);
				app.defaultView.setCenter(vs/2.0f);

				app.camera.setSize(vs * app.cameraZoom);
				app.camera.setCenter(vs * app.cameraZoom/2.0f);

				app.windowSize = sf::Vector2i(e.size.width, e.size.height);

				goto EndOfEvent;
			}

			if (e.type == sf::Event::MouseButtonPressed)
			{
				if (e.mouseButton.button == sf::Mouse::Button::Middle)
				{
					middleButtonPressed = true;
					posWhenMiddleButtonPressed = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
				}
			}

			if (e.type == sf::Event::MouseButtonReleased)
			{
				if (e.mouseButton.button == sf::Mouse::Button::Middle)
				{
					middleButtonPressed = false;
					auto current = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
					app.camera.move(posWhenMiddleButtonPressed-current);
				}
			}

			if (e.type == sf::Event::MouseMoved)
			{
				if (middleButtonPressed)
				{
					auto current = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
					app.camera.move(posWhenMiddleButtonPressed-current);
					posWhenMiddleButtonPressed = current;
				}

				// update mouse pos and snapped point
				auto point = sf::Vector2i(e.mouseMove.x, e.mouseMove.y);
				auto v = window.mapPixelToCoords(point, app.camera);

				// update mouse pos
				e.mouseMove.x = v.x;
				e.mouseMove.y = v.y;

				app.snappedPos = v;
				app.snappedPos = snap(&app, v);
			}

			if (e.type == sf::Event::MouseButtonPressed || e.type == sf::Event::MouseButtonReleased)
			{
				auto point = sf::Vector2i(e.mouseButton.x, e.mouseButton.y);
				auto v = window.mapPixelToCoords(point, app.camera);

				// update mouse pos
				e.mouseButton.x = v.x;
				e.mouseButton.y = v.y;
			}

			if (e.type == sf::Event::MouseWheelMoved)
			{
				if (e.mouseWheel.delta == 0)
				{
					continue;
				}

				float val = 1.0f + e.mouseWheel.delta * 0.01f;
				app.cameraZoom *= val;
				app.camera.zoom(val);
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
					goto EndOfEvent;
				}
				if (e.key.code == sf::Keyboard::Hyphen)
				{
					if (app.gridSize > 1)
					{
						app.gridSize /= 2;
					}
					goto EndOfEvent;
				}
				if (e.key.code == sf::Keyboard::Equal)
				{
					if (app.gridSize < INT_MAX/2)
					{
						app.gridSize *= 2;
					}
					goto EndOfEvent;
				}
				// TODO change this to something more usable
				if (e.key.code == sf::Keyboard::Up)
				{
					app.camera.move(0, -10);
				}
				if (e.key.code == sf::Keyboard::Down)
				{
					app.camera.move(0, 10);
				}
				if (e.key.code == sf::Keyboard::Left)
				{
					app.camera.move(-10, 0);
				}
				if (e.key.code == sf::Keyboard::Right)
				{
					app.camera.move(10, 0);
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

EndOfEvent:

		window.setView(app.defaultView);

		window.clear(sf::Color::White);

		drawGrid(&app);

		window.setView(app.camera);

		app.pCurrentMode->beforeDraw();

		drawLines(app.lines, app.window);
		drawCircles(app.circles, app.window);
		drawTexts(app.texts, app.window);

		app.pCurrentMode->afterDraw();

		drawSnappedPoint(&app);

		window.setView(app.defaultView);
		drawAppMode(&app);
		drawGridSize(&app);

		window.display();
	}

	return 0;
}

