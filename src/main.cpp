#include <iostream>
#include <SFML/Graphics.hpp>


struct Line
{
	sf::Vector2f p[2];
	sf::Color color = sf::Color::Black;
	float width = 4;

	void print()
	{
		std::cout << "{" << p[0].x << ";" << p[0].y << "}; ";
		std::cout << "{" << p[1].x << ";" << p[1].y << "}";
	}
};

struct AppInfo
{
	sf::RenderWindow *window;
	sf::Vector2i windowSize;
	int gridSize = 50;

	std::vector<Line> lines;
};

sf::Vector2f normalize(sf::Vector2f v)
{
	float l = sqrt(v.x * v.x + v.y * v.y);
	return v / l;
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

const auto defaultWindowSize = sf::Vector2i(800, 600);

enum class State
{
	None,
	CreatingNewLine
};

sf::Vector2f snap(AppInfo *info, sf::Vector2f pos)
{
	sf::Vector2f p;
	p.x = ((int)(pos.x + info->gridSize/2) / info->gridSize) * info->gridSize;
	p.y = ((int)(pos.y + info->gridSize/2) / info->gridSize) * info->gridSize;
	return p;
}

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(defaultWindowSize.x, defaultWindowSize.y), "DATDFWY");

	AppInfo app;
	app.window = &window;
	app.windowSize = defaultWindowSize;

	State state;
	state = State::None;

	while (window.isOpen())
	{
		sf::Event e;
		if (window.waitEvent(e))
		{
			switch (e.type)
			{
				case sf::Event::Closed:
				{
					window.close();
					break;
				}

				case sf::Event::Resized:
				{
					sf::FloatRect visibleArea(0, 0, e.size.width, e.size.height);
					window.setView(sf::View(visibleArea));
					app.windowSize = sf::Vector2i(e.size.width, e.size.height);
					break;
				}

				case sf::Event::MouseMoved:
				{
					if (state == State::CreatingNewLine)
					{
						app.lines.back().p[1] = snap(&app, sf::Vector2f(e.mouseMove.x, e.mouseMove.y));
					}

					break;
				}

				case sf::Event::MouseButtonPressed:
				{
					if (state == State::None)
					{
						state = State::CreatingNewLine;

						app.lines.push_back({});
						app.lines.back().p[0] = snap(&app, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
						app.lines.back().p[1] = app.lines.back().p[0];
					}

					break;
				}

				case sf::Event::MouseButtonReleased:
				{
					if (state == State::CreatingNewLine)
					{
						app.lines.back().p[1] = snap(&app, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
						state = State::None;
						if (app.lines.back().p[0] == app.lines.back().p[1])
						{
							app.lines.pop_back();
						}
					}

					break;
				}

				default: {}
			}
		}

		window.clear(sf::Color::White);

		drawGrid(&app);
		drawLines(&app);

		window.display();
	}

	return 0;
}

