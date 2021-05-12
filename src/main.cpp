#include <iostream>
#include <SFML/Graphics.hpp>

#include "AppInfo.hpp"

#include "EditMode.hpp"
#include "CreateMode.hpp"

#include "utils.hpp"
#include "Drawer.hpp"
#include "CommandLine.hpp"
#include "LoadSave.hpp"
#include "Defaults.hpp"

const sf::Color gridColor = sf::Color(200, 200, 200, 255);


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
		v.color    = gridColor;
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
		v.color    = gridColor;
		va[k]      = v;
		k++;

		v.position = sf::Vector2f(size * x, info->windowSize.y+size);
		v.position += displacement;
		va[k] = v;
		k++;
	}

	info->window->draw(va);
}

std::string getDescription(AppInfo *info)
{
	switch (info->state)
	{
		case State::None: return "Error: Current State Is None";
		case State::CommandLine: return "CommandLine";
		case State::CLine:   case State::CNewLine:     return "Create Line";
		case State::CCircle: case State::CNewCircle:   return "Create Circle";
		case State::CText:   case State::CNewText:     return "Create Text";
		case State::EPoint:                 return "Edit Point";
		case State::EMovingLinePoint:       return "Edit Line Point";
		case State::EMovingCirclePoint:     return "Edit Circle Point";
		case State::EChangingCircleRadius:  return "Change Circle Radius";
		case State::EMovingText:            return "Moving Text";
		case State::EMovingLine:            return "Moving Line";
		case State::ESelectElement:         return "Select Element";
		case State::ESelectEnd:             return "Select End";
		case State::ESelectionRectangle:    return "Selection Rectangle";
		case State::EMovingSelection:       return "Moving Selection";
		case State::EMovingCopy:            return "Moving Copy";
		case State::EEditText:              return "Edit Text";
		default: return "Error: Unhandled State";
	}
}

void drawAppMode(AppInfo *info)
{
	sf::Text text;
	text.setFont(info->font);
	text.setCharacterSize(info->characterSize);
	text.setPosition(10, 10);
	text.setFillColor(sf::Color::Black);

	text.setString(getDescription(info));

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
	// draw it in top right corner
	sf::Text text;
	text.setFont(info->font);
	text.setCharacterSize(info->characterSize);
	text.setFillColor(sf::Color::Black);

	text.setString("Grid Size: " + std::to_string(info->gridSize));

	text.setPosition(info->windowSize.x - text.getGlobalBounds().width-20, 10);

	info->window->draw(text);
}

void drawCommandLine(AppInfo *info, const std::string& commandLine)
{
	if (commandLine.size())
	{
		sf::Text text;
		text.setFont(info->font);
		text.setCharacterSize(info->characterSize);
		text.setFillColor(sf::Color::Black);

		text.setString(commandLine);

		text.setPosition(10, info->windowSize.y - text.getGlobalBounds().top - 50);

		info->window->draw(text);
	}
	else if (info->error.size())
	{
		sf::Text text;
		text.setFont(info->font);
		text.setCharacterSize(info->characterSize);
		text.setFillColor(sf::Color::Red);

		text.setString(info->error);

		text.setPosition(10, info->windowSize.y - text.getGlobalBounds().top - 50);

		info->window->draw(text);
	}
}

const auto defaultWindowSize = sf::Vector2i(1600, 1200);

void handleExport(AppInfo& app, Command& commad)
{
	sf::RenderTexture texture;

	// figure out texture size
	sf::Vector2f max, min;
	if (app.lines.size())
	{
		max = app.lines[0].p[0];
		min = max;
	}
	else if (app.circles.size())
	{
		max = app.circles[0].center;
		min = max;
	}
	else if (app.texts.size())
	{
		max.x = app.texts[0].bounding.left;
		max.y = app.texts[0].bounding.top;
		min = max;
	}
	// return, diagram is empty
	else
	{
		return;
	}

	for (auto& line : app.lines)
	{
		for (int i = 0; i < 2; ++i)
		{
			if (line.p[i].x < min.x)   min.x = line.p[i].x;
			if (line.p[i].x > max.x)   max.x = line.p[i].x;
			if (line.p[i].y < min.y)   min.y = line.p[i].y;
			if (line.p[i].y > max.y)   max.y = line.p[i].y;
		}
	}

	for (auto& circle : app.circles)
	{
		if (circle.center.x - circle.radius < min.x)
			min.x = circle.center.x - circle.radius;
		if (circle.center.x + circle.radius > max.x)
			max.x = circle.center.x + circle.radius;
		if (circle.center.y - circle.radius < min.y)
			min.y = circle.center.y - circle.radius;
		if (circle.center.y + circle.radius > max.y)
			max.y = circle.center.y + circle.radius;
	}

	for (auto& text : app.texts)
	{
		for (int i = 0; i < 2; ++i)
		{
			if (text.bounding.left < min.x)  min.x = text.bounding.left;
			if (text.bounding.top < min.y)   min.y = text.bounding.top;
			if (text.bounding.left + text.bounding.width > max.x)
				max.x = text.bounding.left + text.bounding.width;
			if (text.bounding.top + text.bounding.height > max.y)
				max.x = text.bounding.top + text.bounding.height;
		}
	}

	float scale = commad.intParam[0] * 0.01;
	float textureWidth =  (max.x-min.x) * scale + 20;
	float textureHeight = (max.y-min.y) * scale + 20;

	sf::RenderTexture renderTexture;
	if (!renderTexture.create(textureWidth, textureHeight))
	{
		return;
	}

	sf::Transform transform;
	min  *= scale;
	min -= sf::Vector2f(10, 10);
	transform.translate(-min);
	transform.scale(scale, scale);

	renderTexture.clear(sf::Color::White);

	drawLines(app.lines, &renderTexture, &transform);
	drawCircles(app.circles, &renderTexture, &transform);
	drawTexts(app.texts, &renderTexture, false, &transform);

	auto filename = commad.stringParam[0];

	renderTexture.display();
	if (!renderTexture.getTexture().copyToImage().saveToFile(filename))
	{
		app.error = "Export Failed";
	}
}

void handleSet(AppInfo& app, Command& command)
{
	if (command.target == Command::Line)
	{
		if (command.scope == Command::Global)
		{
			if (command.paramType == ParamType::LineWidth)
			{
				for (auto& line : app.lines)
				{
					line.width = command.intParam[0];
				}
			}
			else if (command.paramType == ParamType::LineColor)
			{
				for (auto& line : app.lines)
				{
					line.color.r = command.intParam[0];
					line.color.g = command.intParam[1];
					line.color.b = command.intParam[2];
					line.color.a = command.intParam[3];
				}
			}
		}
		else if (command.scope == Command::Defaults)
		{
			if (command.paramType == ParamType::LineWidth)
			{
				app.defaults.line.width = command.intParam[0];
			}
			else if (command.paramType == ParamType::LineColor)
			{
				app.defaults.line.color.r = command.intParam[0];
				app.defaults.line.color.g = command.intParam[1];
				app.defaults.line.color.b = command.intParam[2];
				app.defaults.line.color.a = command.intParam[3];
			}
		}
		else
		{
			if (command.paramType == ParamType::LineWidth)
			{
				for (auto& line : app.selection.lines)
				{
					line->width = command.intParam[0];
				}
			}
			else if (command.paramType == ParamType::LineColor)
			{
				for (auto& line : app.selection.lines)
				{
					line->color.r = command.intParam[0];
					line->color.g = command.intParam[1];
					line->color.b = command.intParam[2];
					line->color.a = command.intParam[3];
				}
			}
		}
	}
	else if (command.target == Command::Circle)
	{
		if (command.scope == Command::Global)
		{
			if (command.paramType == ParamType::CircleBorderWidth)
			{
				for (auto& circle : app.circles)
				{
					circle.outlineThickness = command.intParam[0];
				}
			}
			else if (command.paramType == ParamType::CircleBorderColor)
			{
				for (auto& circle : app.circles)
				{
					circle.outlineColor.r = command.intParam[0];
					circle.outlineColor.g = command.intParam[1];
					circle.outlineColor.b = command.intParam[2];
					circle.outlineColor.a = command.intParam[3];
				}
			}
			else if (command.paramType == ParamType::CircleFillColor)
			{
				for (auto& circle : app.circles)
				{
					circle.color.r = command.intParam[0];
					circle.color.g = command.intParam[1];
					circle.color.b = command.intParam[2];
					circle.color.a = command.intParam[3];
				}
			}
		}
		else if (command.scope == Command::Defaults)
		{
			if (command.paramType == ParamType::CircleBorderWidth)
			{
				app.defaults.circle.outlineThickness = command.intParam[0];
			}
			else if (command.paramType == ParamType::CircleBorderColor)
			{
				app.defaults.circle.outlineColor.r = command.intParam[0];
				app.defaults.circle.outlineColor.g = command.intParam[1];
				app.defaults.circle.outlineColor.b = command.intParam[2];
				app.defaults.circle.outlineColor.a = command.intParam[3];
			}
			else if (command.paramType == ParamType::CircleFillColor)
			{
				app.defaults.circle.color.r = command.intParam[0];
				app.defaults.circle.color.g = command.intParam[1];
				app.defaults.circle.color.b = command.intParam[2];
				app.defaults.circle.color.a = command.intParam[3];
			}
		}
		else
		{
			if (command.paramType == ParamType::CircleBorderWidth)
			{
				for (auto circle : app.selection.circles)
				{
					circle->outlineThickness = command.intParam[0];
				}
			}
			else if (command.paramType == ParamType::CircleBorderColor)
			{
				for (auto circle : app.selection.circles)
				{
					circle->outlineColor.r = command.intParam[0];
					circle->outlineColor.g = command.intParam[1];
					circle->outlineColor.b = command.intParam[2];
					circle->outlineColor.a = command.intParam[3];
				}
			}
			else if (command.paramType == ParamType::CircleFillColor)
			{
				for (auto circle : app.selection.circles)
				{
					circle->color.r = command.intParam[0];
					circle->color.g = command.intParam[1];
					circle->color.b = command.intParam[2];
					circle->color.a = command.intParam[3];
				}
			}
		}
	}
	else if (command.target == Command::Text)
	{
		if (command.scope == Command::Global)
		{
			if (command.paramType == ParamType::TextSize)
			{
				for (auto& text : app.texts)
				{
					text.text.setCharacterSize(command.intParam[0]);
					text.bounding = text.text.getGlobalBounds();
				}
			}
			else if (command.paramType == ParamType::TextColor)
			{
				for (auto& text : app.texts)
				{
					text.text.setFillColor(sf::Color(command.intParam[0], command.intParam[1],
								command.intParam[2], command.intParam[3]));
				}
			}
		}
		else if (command.scope == Command::Defaults)
		{
			if (command.paramType == ParamType::TextSize)
			{
				for (auto text : app.selection.texts)
				{
					app.defaults.text.size = command.intParam[0];
				}
			}
			else if (command.paramType == ParamType::TextColor)
			{
				for (auto& text : app.selection.texts)
				{
					app.defaults.text.color = sf::Color(command.intParam[0], command.intParam[1],
								command.intParam[2], command.intParam[3]);
				}
			}
		}
		else
		{
			if (command.paramType == ParamType::TextSize)
			{
				for (auto text : app.selection.texts)
				{
					text->text.setCharacterSize(command.intParam[0]);
					text->bounding = text->text.getGlobalBounds();
				}
			}
			else if (command.paramType == ParamType::TextColor)
			{
				for (auto& text : app.selection.texts)
				{
					text->text.setFillColor(sf::Color(command.intParam[0], command.intParam[1],
								command.intParam[2], command.intParam[3]));
				}
			}
		}
	}
	else
	{
		app.error = "(3290) Internal Error";
	}
}

void handleLoad(AppInfo& app, Command& command)
{
	bool success;
	AppInfo info = loadProject(command.stringParam[0], &success, &app.font);
	if (!success)
	{
		app.error = "Failed To Load " + command.stringParam[0];
		return;
	}
	
	app.lines = info.lines;
	app.circles = info.circles;
	app.texts = info.texts;
	app.elementId = app.lines.size() + app.circles.size() + app.texts.size();
}

void handleSave(AppInfo& app, Command& command)
{
	if (!saveProject(&app, command.stringParam[0]))
	{
		app.error = "Failed To Save Prject";
	}
}

void handleCommandLine(sf::Event& e, AppInfo& app, std::string& commandLine)
{
	if (e.type == sf::Event::KeyPressed)
	{
		if (e.key.code == sf::Keyboard::Enter)
		{
			// restore previous state
			app.state = app.previousState;

			bool success;
			Command command = parseCommand(commandLine, &success);
			commandLine = "";
			if (!success)
			{
				app.error = "Failed To Parse Command";
				return;
			}
			if (command.type == Command::Set)
			{
				handleSet(app, command);
			}
			else if (command.type == Command::Export)
			{
				handleExport(app, command);
			}
			else if (command.type == Command::Load)
			{
				handleLoad(app, command);
			}
			else if (command.type == Command::Save)
			{
				handleSave(app, command);
			}
		}
		else if (e.key.code == sf::Keyboard::Backspace)
		{
			if (commandLine.size())
			{
				commandLine.pop_back();
				if (commandLine.size() == 0)
				{
					app.state = app.previousState;
				}
			}
		}
		else if (charPrintable(e.key))
		{
			commandLine += getCharFromKeyEvent(e.key);
		}
	}
}

void editBasedOnChanges(AppInfo *info, const std::vector<Change>& changes)
{
	for (auto& change : changes)
	{

		if (change.elementType == ElementType::Line)
		{
			Line line;
			line = *(Line *)change.previousValue;

			bool lineFound = false;
			for (auto& l : info->lines)
			{
				if (l.id == line.id)
				{
					l = line;
					lineFound = true;
					break;
				}
			}
			if (!lineFound)
			{
				std::cout << "(2392)Error: moved line was not found" << std::endl;
			}
		}
		else if (change.elementType == ElementType::Circle)
		{
			Circle circle;
			circle = *(Circle *)change.previousValue;

			bool circleFound = false;
			for (auto& c : info->circles)
			{
				if (c.id == circle.id)
				{
					c = circle;
					circleFound = true;
					break;
				}
			}
			if (!circleFound)
			{
				std::cout << "(2392)Error: moved line was not found" << std::endl;
			}
		}
		else if (change.elementType == ElementType::Text)
		{
			Text text;
			text = *(Text *)change.previousValue;

			bool textFound = false;
			for (auto& t : info->texts)
			{
				if (t.id == text.id)
				{
					t = text;
					textFound = true;
					break;
				}
			}
			if (!textFound)
			{
				std::cout << "(2392)Error: moved line was not found" << std::endl;
			}
		}
	}
}

void createBasedOnChanges(AppInfo *info, const std::vector<Change>& changes)
{
	for (auto& change : changes)
	{
		if (change.elementType == ElementType::Line)
		{
			Line line;
			line = *(Line *)change.previousValue;
			info->lines.push_back(line);
		}
		else if (change.elementType == ElementType::Circle)
		{
			Circle circle;
			circle = *(Circle *)change.previousValue;
			info->circles.push_back(circle);
		}
		else if (change.elementType == ElementType::Text)
		{
			Text text;
			text = *(Text *)change.previousValue;
			info->texts.push_back(text);
		}
	}
}

void deleteBasedOnChanges(AppInfo *info, const std::vector<Change>& changes)
{
	for (auto& change : changes)
	{
		if (change.elementType == ElementType::Line)
		{
			for (int i = info->lines.size()-1; i >= 0; --i)
			{
				if (info->lines[i].id == change.elementId)
				{
					// remove this line
					info->lines.erase(info->lines.begin()+i);
					break;
				}
			}
		}
		else if (change.elementType == ElementType::Circle)
		{
			for (int i = info->circles.size()-1; i >= 0; --i)
			{
				if (info->circles[i].id == change.elementId)
				{
					// remove this circle
					info->circles.erase(info->circles.begin()+i);
					break;
				}
			}
		}
		else if (change.elementType == ElementType::Text)
		{
			for (int i = info->texts.size()-1; i >= 0; --i)
			{
				if (info->texts[i].id == change.elementId)
				{
					// remove this text
					info->texts.erase(info->texts.begin()+i);
					break;
				}
			}
		}
		else
		{
			std::cout << "(4920)Error: Unknown ElementType: " << (int)change.elementType << std::endl;
		}
	}
}


void undo(AppInfo *info)
{
	if (info->history.timeFrames.size() && info->history.currentHistoryIndex >= 0)
	{
		auto& lastTF = info->history.timeFrames[info->history.currentHistoryIndex];
		if (lastTF.changeType == ChangeType::Create)
		{
			auto changes = info->history.collectChangesForTimeFrameIndex(info->history.currentHistoryIndex);
			deleteBasedOnChanges(info, changes);
		}
		else if (lastTF.changeType == ChangeType::Edit)
		{
			auto changes = info->history.collectPreviousStateForTimeFrameIndex(info->history.currentHistoryIndex);
			editBasedOnChanges(info, changes);
		}
		else if (lastTF.changeType == ChangeType::Delete)
		{
			auto changes = info->history.collectChangesForTimeFrameIndex(info->history.currentHistoryIndex);
			createBasedOnChanges(info, changes);
		}

		if (info->history.currentHistoryIndex >= 0)  info->history.currentHistoryIndex--;
	}
}

void redo(AppInfo *info)
{
	if (info->history.currentHistoryIndex == info->history.timeFrames.size()-1)
		return;

	info->history.currentHistoryIndex++;

	auto& currentTF = info->history.timeFrames[info->history.currentHistoryIndex];
	if (currentTF.changeType == ChangeType::Create)
	{
		auto changes = info->history.collectChangesForTimeFrameIndex(info->history.currentHistoryIndex);
		createBasedOnChanges(info, changes);
	}
	else if (currentTF.changeType == ChangeType::Edit)
	{
		auto changes = info->history.collectChangesForTimeFrameIndex(info->history.currentHistoryIndex);
		editBasedOnChanges(info, changes);
	}
	else if (currentTF.changeType == ChangeType::Delete)
	{
		auto changes = info->history.collectChangesForTimeFrameIndex(info->history.currentHistoryIndex);
		deleteBasedOnChanges(info, changes);
	}
}

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

	{
		bool success;
		app.defaults = loadDefaults("defaults", &success);
		if (!success)
		{
			app.error = "Failed To Load Defaults From File";
		}
	}

	app.font.loadFromFile("resources/Hack-Regular.ttf");

	app.previousState = State::CLine;
	onCreateEnter(&app);

	bool middleButtonPressed = false;
	sf::Vector2f posWhenMiddleButtonPressed;

	std::string commandLine;

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
				if (app.state == State::CommandLine)
				{
					app.state = app.previousState;
					commandLine = "";
				}

				if (e.mouseButton.button == sf::Mouse::Button::Middle)
				{
					middleButtonPressed = true;
					posWhenMiddleButtonPressed = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
					goto EndOfEvent;
				}

				if (e.mouseButton.button == sf::Mouse::Button::Left 
						&& getStateType(app.state) != StateType::Create)
				{
					onEditExit(&app);
					onCreateEnter(&app);
				}

				if (e.mouseButton.button == sf::Mouse::Button::Right
						&& getStateType(app.state) != StateType::Edit)
				{
					onCreateExit(&app);
					onEditEnter(&app);
				}
			}

			if (e.type == sf::Event::MouseButtonReleased)
			{
				if (e.mouseButton.button == sf::Mouse::Button::Middle)
				{
					middleButtonPressed = false;
					auto current = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
					app.camera.move(posWhenMiddleButtonPressed-current);
					goto EndOfEvent;
				}
			}

			if (e.type == sf::Event::MouseMoved)
			{
				if (middleButtonPressed)
				{
					auto current = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
					auto v = posWhenMiddleButtonPressed-current;
					app.camera.move(v * app.cameraZoom);
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
			else if (e.type == sf::Event::KeyPressed)
			{
				if (e.key.code == sf::Keyboard::SemiColon && e.key.shift)
				{
					app.previousState = app.state;
					app.state = State::CommandLine;
					commandLine = "";
					app.error = "";
				}
				else if (e.key.code == sf::Keyboard::LShift || e.key.code == sf::Keyboard::RShift)
					app.shiftPressed = true;
				else if (e.key.code == sf::Keyboard::RAlt || e.key.code == sf::Keyboard::LAlt)
					app.snapping = false;
			}
			else if (e.type == sf::Event::KeyReleased)
			{
				if (e.key.code == sf::Keyboard::LShift || e.key.code == sf::Keyboard::RShift)
					app.shiftPressed = false;
				else if (e.key.code == sf::Keyboard::RAlt || e.key.code == sf::Keyboard::LAlt)
					app.snapping = true;
			}

			if (app.state != State::CommandLine)
			{
				if (e.type == sf::Event::KeyPressed)
				{
					if (e.key.code == sf::Keyboard::C && e.key.control)
					{
						if (getStateType(app.state) == StateType::Create)     onCreateExit(&app);
						else if (getStateType(app.state) == StateType::Edit)  onEditExit(&app);

						onCreateEnter(&app);
						goto EndOfEvent;
					}
					else if (e.key.code == sf::Keyboard::Z && e.key.control && !e.key.shift)
					{
						if (getStateType(app.state) == StateType::Create)     onCreateExit(&app);
						else if (getStateType(app.state) == StateType::Edit)  onEditExit(&app);

						onCreateEnter(&app);
						undo(&app);

						goto EndOfEvent;
					}
					else if (e.key.code == sf::Keyboard::Z && e.key.control && e.key.shift)
					{
						if (getStateType(app.state) == StateType::Create)     onCreateExit(&app);
						else if (getStateType(app.state) == StateType::Edit)  onEditExit(&app);

						onCreateEnter(&app);
						redo(&app);

						goto EndOfEvent;
					}
				}

				// handle zoom and movement
				if (e.type == sf::Event::KeyPressed)
				{
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

				if (getStateType(app.state) == StateType::Create)
				{
					onCreateEvent(&app, e);
				}
				else if (getStateType(app.state) == StateType::Edit)
				{
					onEditEvent(&app, e);
				}
				else
				{
					std::cout << "(3091)Error: Unexpected Type: "  << (unsigned) getStateType(app.state) << std::endl;
				}
			}
			else if (app.state == State::CommandLine)
			{
				handleCommandLine(e, app, commandLine);
			}
		}

EndOfEvent:

		window.setView(app.defaultView);

		window.clear(sf::Color::White);

		drawGrid(&app);

		window.setView(app.camera);

		if (getStateType(app.state) == StateType::Edit || app.state == State::CommandLine)
			editBeforeDraw(&app);

		drawLines(app.lines, app.window);
		drawCircles(app.circles, app.window);
		drawTexts(app.texts, app.window);

		drawSnappedPoint(&app);

		window.setView(app.defaultView);
		drawAppMode(&app);
		drawGridSize(&app);
		drawCommandLine(&app, commandLine);

		window.display();
	}

	return 0;
}

