#include "Drawer.hpp"
#include "utils.hpp"


void drawCircles(const std::vector<Circle>& circles, sf::RenderWindow *window)
{
	sf::CircleShape shape;

	for (auto &circle : circles)
	{
		shape.setRadius(circle.radius - circle.outlineThickness/2);
		shape.setOutlineThickness(circle.outlineThickness);
		shape.setOutlineColor(circle.outlineColor);
		shape.setFillColor(circle.color);

		shape.setPosition(circle.center);
		shape.setOrigin(shape.getRadius(), shape.getRadius());

		window->draw(shape);
	}
}
void drawLines(const std::vector<Line>& lines, sf::RenderWindow *window)
{
	sf::VertexArray va;
	va.setPrimitiveType(sf::PrimitiveType::Quads);
	va.resize(lines.size() * 4);

	int k = 0;
	for (auto line : lines)
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

	window->draw(va);
}

void drawTexts(const std::vector<Text>& texts, sf::RenderWindow *window, bool drawBack)
{
	sf::RectangleShape rect;
	rect.setFillColor(sf::Color(170, 170, 0, 50));

	for (auto& t : texts)
	{
		if (drawBack)
		{
			rect.setPosition(t.bounding.left, t.bounding.top);
			rect.setSize(sf::Vector2f(t.bounding.width, t.bounding.height));
			window->draw(rect);
		}
		window->draw(t.text);
	}
}

