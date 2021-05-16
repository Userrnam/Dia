#include <fstream>

#include "LoadSave.hpp"
#include "utils.hpp"


void writeString(std::fstream& f, const std::string& s)
{
	uint32_t size = s.size();
	f.write((char*)&size, sizeof(uint32_t));
	f.write(s.data(), s.size());
}

bool readString(std::fstream& f, std::string& s, uint32_t strMax = 1024)
{
	// read string size
	uint32_t stringSize;
	f.read((char*)&stringSize, sizeof(uint32_t));
	if (f.fail())   return false;

	if (stringSize > strMax)   return false;

	s.resize(stringSize);

	union Hack
	{
		char* p; const char* cp;
	};

	Hack h;
	h.cp = s.data();

	f.read(h.p, stringSize);

	if (f.fail())   return false;

	return true;
}

bool loadProject(const std::string& path, AppInfo *app)
{
	AppInfo info;

	std::fstream f;

	f.open(path, std::ios::in | std::ios::binary);

	if (f.fail())   return false;

	// load lines
	uint32_t size;
	f.read((char*)&size, sizeof(uint32_t));

	if (size > 30000)   return false;

	for (int i = 0; i < size; ++i)
	{
		Line line;
		f.read((char*)&line, sizeof(Line));

		if (f.fail())   return false;

		info.elementId++;
		line.id = info.elementId;

		info.lines.push_back(line);
	}

	// load circles
	f.read((char*)&size, sizeof(uint32_t));

	if (size > 30000)   return false;

	for (int i = 0; i < size; ++i)
	{
		Circle circle;
		f.read((char*)&circle, sizeof(Circle));

		if (f.fail())   return false;

		info.elementId++;
		circle.id = info.elementId;

		info.circles.push_back(circle);
	}

	// load texts
	f.read((char*)&size, sizeof(uint32_t));

	if (size > 30000)   return false;

	bool warnedAboutFont = false;

	for (int i = 0; i < size; ++i)
	{
		Text text;
		std::string s;
		std::string fontName;
		sf::Vector2f pos;
		sf::Color textColor;
		uint32_t textSize;

		// read string
		if (!readString(f, s))  return false;

		// read fontName
		if (!readString(f, fontName))  return false;

		// read pos
		f.read((char*)&pos, sizeof(sf::Vector2f));
		if (f.fail())   return false;
		
		// read color
		f.read((char*)&textColor, sizeof(uint32_t));
		if (f.fail())   return false;

		// read size
		f.read((char*)&textSize, sizeof(uint32_t));
		if (f.fail())   return false;

		text.text.setString(s);
		text.text.setCharacterSize(textSize);
		text.text.setPosition(pos);
		auto *font = getFont(app, fontName);
		if (font)
		{
			text.text.setFont(*font);
		}
		else
		{
			font = getFont(app, app->defaults.text.fontName);
			if (font)
			{
				text.text.setFont(*font);
			}
			else
			{
				// put random font
				for (auto _font : app->fonts)
				{
					text.text.setFont(*_font.second);
					break;
				}
			}
			if (!warnedAboutFont)
			{
				app->error = "Warning Some Fonts Were Replaced";
				warnedAboutFont = true;
			}
		}

		text.text.setFillColor(textColor);

		updateBoundingBox(&text);

		info.elementId++;
		text.id = info.elementId;

		info.texts.push_back(text);
	}

	f.close();

	// update app data
	app->lines = info.lines;
	app->circles = info.circles;
	app->texts = info.texts;
	app->elementId = app->lines.size() + app->circles.size() + app->texts.size();

	return true;
}

bool saveProject(AppInfo* info, const std::string& path)
{
	std::fstream f;
	f.open(path, std::ios::out | std::ios::binary);

	if (f.fail())   return false;

	// save lines
	uint32_t size = info->lines.size();
	f.write((char*)&size, sizeof(uint32_t));
	for (auto& line : info->lines)
	{
		f.write((char*)&line, sizeof(Line));
	}

	// save circles
	size = info->circles.size();
	f.write((char*)&size, sizeof(uint32_t));
	for (auto& circle : info->circles)
	{
		f.write((char*)&circle, sizeof(Circle));
	}

	// create inversed hash map for fonts
	std::unordered_map<const sf::Font*, std::string> invFonts;
	for (auto font : info->fonts)
	{
		invFonts[font.second] = font.first;
	}

	// save text as string, position, size
	size = info->texts.size();
	f.write((char*)&size, sizeof(uint32_t));
	for (auto& text : info->texts)
	{
		// save string
		writeString(f, text.text.getString());

		// save font
		std::string fontName = invFonts[text.text.getFont()];
		writeString(f, fontName);

		// save position
		sf::Vector2f pos = text.text.getPosition();
		f.write((char*)&pos, sizeof(sf::Vector2f));

		// save color
		sf::Color color = text.text.getFillColor();
		f.write((char*)&color, sizeof(sf::Color));

		// save size
		uint32_t size = text.text.getCharacterSize();
		f.write((char*)&size, sizeof(uint32_t));
	}

	if (f.fail())   return false;

	f.close();

	return true;
}
