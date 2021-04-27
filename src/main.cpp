#include <iostream>
#include <SFML/Graphics.hpp>


#define MAX_SELECT_DISTANCE 20
#define MAX_DELETE_DISTANCE 20
#define EPS 1e-4

float d2(sf::Vector2f v1, sf::Vector2f v2)
{
	sf::Vector2f v = v1 - v2;
	return v.x * v.x + v.y * v.y;
}


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

struct Circle
{
	// circle is defined by origin and point on circumference
	sf::Vector2f p[2];
	sf::Color outlineColor = sf::Color::Black;
	sf::Color color = sf::Color(0,0,0,0);
	float outlineThickness = 4;
};

bool same(sf::Vector2f v1, sf::Vector2f v2)
{
	return fabs(v1.x - v2.x) < EPS && fabs(v1.y - v2.y) < EPS;
}

#define MODE_NUMBER(mode, number, active)\
	number << 4 | active << 3 | mode

#define CREATE_MODE 1
#define EDIT_MODE   2
#define DELETE_MODE 3

enum class State
{
	CLine            = MODE_NUMBER(CREATE_MODE, 0, 0),
	CNewLine         = MODE_NUMBER(CREATE_MODE, 0, 1),
	CCircle          = MODE_NUMBER(CREATE_MODE, 1, 0),
	CNewCircle       = MODE_NUMBER(CREATE_MODE, 1, 1),

	EPoint           = MODE_NUMBER(EDIT_MODE, 0, 0),
	EMovingPoint     = MODE_NUMBER(EDIT_MODE, 0, 1),
//	EMovingLine      = MODE_NUMBER(EDIT_MODE, 0, 1),

	DNone            = MODE_NUMBER(DELETE_MODE, 0, 0),
};

int Mode(State state)
{
	return (int)state & 0b111;
}

std::string stateAsString(State state)
{
	switch (state)
	{
	case State::CLine:
	case State::CNewLine:
		return "Create Line";
	case State::CCircle:
	case State::CNewCircle:
		return "Create Circle";
	case State::EPoint:
		return "Edit Mode";
//	case State::EMovingLine:
//		return "Edit Line";
	case State::EMovingPoint:
		return "Edit Line Point";
	case State::DNone:
		return "Delete Mode";
	default: {}
	}
	return "Error";
}

struct AppInfo
{
	State state = State::CLine;
	int characterSize = 30;
	sf::Font font;
	sf::RenderWindow *window;
	sf::Vector2i windowSize;
	int gridSize = 50;

	sf::Vector2f *pVec;

	std::vector<Line> lines;
	std::vector<Circle> circles;
};

sf::Vector2f normalize(sf::Vector2f v)
{
	float l = sqrt(v.x * v.x + v.y * v.y);
	return v / l;
}

void drawCircles(AppInfo *info)
{
	sf::CircleShape shape;

	for (auto &circle : info->circles)
	{
		shape.setRadius(sqrt(d2(circle.p[0], circle.p[1])) - circle.outlineThickness/2);
		shape.setOutlineThickness(circle.outlineThickness);
		shape.setOutlineColor(circle.outlineColor);
		shape.setFillColor(circle.color);

		shape.setPosition(circle.p[0]);
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

	text.setString(stateAsString(info->state));

	info->window->draw(text);
}

const auto defaultWindowSize = sf::Vector2i(800, 600);

sf::Vector2f snap(AppInfo *info, sf::Vector2f pos)
{
	sf::Vector2f p;
	p.x = ((int)(pos.x + info->gridSize/2) / info->gridSize) * info->gridSize;
	p.y = ((int)(pos.y + info->gridSize/2) / info->gridSize) * info->gridSize;
	return p;
}

sf::Vector2f *getClosestPoint(AppInfo *info, sf::Vector2f pos, float *distance2, Line **pline = nullptr)
{
	sf::Vector2f *res = nullptr;
	Line *l = nullptr;
	float mind = 1e15;
	for (auto &line : info->lines)
	{
		for (int i = 0; i < 2; ++i)
		{
			float d = d2(pos, line.p[i]);
			if (d < mind)
			{
				mind = d;
				res = &line.p[i];
				l = &line;
			}
		}
	}
	*distance2 = mind;
	if (pline)
	{
		*pline = l;
	}
	return res;
}

float d2line(Line& l, sf::Vector2f pos)
{
	float a2 = d2(l.p[0], pos);
	float b2 = d2(l.p[1], pos);
	float c2 = d2(l.p[0], l.p[1]);

	float c = sqrt(c2);

	float t = (a2 - b2 - c2)/(2*c);

	return b2 - t*t;
}

Line *getClosestLine(AppInfo *info, sf::Vector2f pos, float *distance2)
{
	float d;
	Line *l;
	getClosestPoint(info, pos, &d, &l);

	for (auto& line : info->lines)
	{
		float dd = d2line(line, pos);
		if (dd < d)
		{
			d = dd;
			l = &line;
		}
	}

	*distance2 = d;

	return l;
}

void handleEditMode(AppInfo *info, sf::Event& e)
{
	switch (e.type)
	{
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			if (info->state == State::EMovingPoint)
			{
				*info->pVec = snap(info, mousePos);
			}

			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			if (info->state == State::EPoint)
			{
				float dist2;
				sf::Vector2f *p = getClosestPoint(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y), &dist2);
				if (dist2 < MAX_SELECT_DISTANCE * MAX_SELECT_DISTANCE)
				{
					info->state = State::EMovingPoint;
					info->pVec = p;
				}
			}

			break;
		}

		case sf::Event::MouseButtonReleased:
		{
			if (info->state == State::EMovingPoint)
			{
				info->state = State::EPoint;
				info->pVec = nullptr;
				// FIXME: 2 points of the same line may be the same.
				// If this happens, the line should be deleted
			}

			break;
		}

		default: {}
	}
}

void handleCreateMode(AppInfo *info, sf::Event& e)
{
	switch (e.type)
	{
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mousePos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			if (info->state == State::CNewLine)
			{
				info->lines.back().p[1] = snap(info, mousePos);
			}
			else if (info->state == State::CNewCircle)
			{
				info->circles.back().p[1] = snap(info, mousePos);
			}
			break;
		}

		case sf::Event::MouseButtonPressed:
		{
			if (info->state == State::CLine)
			{
				info->state = State::CNewLine;

				info->lines.push_back({});
				info->lines.back().p[0] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->lines.back().p[1] = info->lines.back().p[0];
			}
			else if (info->state == State::CCircle)
			{
				info->state = State::CNewCircle;

				info->circles.push_back({});
				info->circles.back().p[0] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->circles.back().p[1] = info->circles.back().p[0];
			}

			break;
		}

		case sf::Event::MouseButtonReleased:
		{
			if (info->state == State::CNewLine)
			{
				info->lines.back().p[1] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->state = State::CLine;
				if (same(info->lines.back().p[0], info->lines.back().p[1]))
				{
					info->lines.pop_back();
				}
			}
			else if (info->state == State::CNewCircle)
			{
				info->circles.back().p[1] = snap(info, sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
				info->state = State::CCircle;
				if (same(info->circles.back().p[0], info->circles.back().p[1]))
				{
					info->circles.pop_back();
				}
			}

			break;
		}

		case sf::Event::KeyPressed:
		{
			if (e.key.code == sf::Keyboard::L)
			{
				info->state = State::CLine;
			}
			else if (e.key.code == sf::Keyboard::C)
			{
				info->state = State::CCircle;
			}
			break;
		}

		default: {}
	}
}

void handleDeleteMode(AppInfo *info, sf::Event& e)
{
	if (e.type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f mousePos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
		float dist2;
		Line *l = getClosestLine(info, mousePos, &dist2);

		// remove line
		if (dist2 < MAX_DELETE_DISTANCE * MAX_DELETE_DISTANCE)
		{
			int index = l - info->lines.data();
			
			info->lines.erase(info->lines.begin() + index);
		}
	}
}

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(defaultWindowSize.x, defaultWindowSize.y), "DATDFWY");

	AppInfo app;
	app.window = &window;
	app.windowSize = defaultWindowSize;

	app.font.loadFromFile("resources/Hack-Regular.ttf");

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

			if (e.type == sf::Event::KeyPressed)
			{
				int mode = Mode(app.state);
				if (e.key.code == sf::Keyboard::C && mode != CREATE_MODE)
				{
					app.state = State::CLine;
					continue;
				}
				if (e.key.code == sf::Keyboard::E && mode != EDIT_MODE)
				{
					app.state = State::EPoint;
					continue;
				}
				if (e.key.code == sf::Keyboard::D && mode != DELETE_MODE)
				{
					app.state = State::DNone;
					continue;
				}
			}

			switch (Mode(app.state))
			{
				case CREATE_MODE:
				{
					handleCreateMode(&app, e);
					break;
				}
				case EDIT_MODE:
				{
					handleEditMode(&app, e);
					break;
				}
				case DELETE_MODE:
				{
					handleDeleteMode(&app, e);
					break;
				}
			}

		}

		window.clear(sf::Color::White);

		drawGrid(&app);
		drawLines(&app);
		drawCircles(&app);
		drawAppMode(&app);

		window.display();
	}

	return 0;
}

