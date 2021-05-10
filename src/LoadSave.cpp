#include <fstream>

#include "LoadSave.hpp"
#include "utils.hpp"


// FIXME allow different fonts
AppInfo loadProject(const std::string& path, bool *success, sf::Font *font)
{
	AppInfo info;

	*success = false;

	std::fstream f;

	f.open(path, std::ios::in|std::ios::binary);

	if (f.fail())   return info;

	// load lines
	uint32_t size;
	f.read((char*)&size, sizeof(uint32_t));

	if (size > 10000)   return info;

	for (int i = 0; i < size; ++i)
	{
		Line line;
		f.read((char*)&line, sizeof(Line));

		if (f.fail())   return info;

		info.lines.push_back(line);
	}

	// load circles
	f.read((char*)&size, sizeof(uint32_t));

	if (size > 10000)   return info;

	for (int i = 0; i < size; ++i)
	{
		Circle circle;
		f.read((char*)&circle, sizeof(Circle));

		if (f.fail())   return info;

		info.circles.push_back(circle);
	}

	// load texts
	f.read((char*)&size, sizeof(uint32_t));

	if (size > 10000)   return info;

	for (int i = 0; i < size; ++i)
	{
		Text text;
		std::string s;
		sf::Vector2f pos;
		uint32_t textSize;
		
		// read string size
		uint32_t stringSize;
		f.read((char*)&stringSize, sizeof(uint32_t));
		if (f.fail())   return info;

		if (stringSize > 1000)   return info;

		s.resize(stringSize);

		union Hack
		{
			char *p; const char *cp;
		};

		Hack h;
		h.cp = s.data();

		f.read(h.p, stringSize);

		if (f.fail())   return info;

		// read pos
		f.read((char*)&pos, sizeof(sf::Vector2f));

		if (f.fail())   return info;

		// read size
		f.read((char*)&textSize, sizeof(uint32_t));

		if (f.fail())   return info;

		text.text.setString(s);
		text.text.setCharacterSize(textSize);
		text.text.setPosition(pos);
		text.text.setFont(*font);
		text.text.setFillColor(sf::Color::Black);

		updateBoundingBox(&text);

		info.texts.push_back(text);
	}

	f.close();

	*success = true;

	return info;
}

bool saveProject(AppInfo *info, const std::string& path)
{
	std::fstream f;
	f.open(path, std::ios::out|std::ios::binary);

	if (f.fail())   return false;

	uint32_t size = info->lines.size();
	f.write((char*)&size, sizeof(uint32_t));
	for (auto& line : info->lines)
	{
		f.write((char*)&line, sizeof(Line));
	}

	size = info->circles.size();
	f.write((char*)&size, sizeof(uint32_t));
	for (auto& circle : info->circles)
	{
		f.write((char*)&circle, sizeof(Circle));
	}

	// save text as string, position, size
	size = info->texts.size();
	f.write((char*)&size, sizeof(uint32_t));
	for (auto& text : info->texts)
	{
		// save string
		std::string s = text.text.getString();
		size = s.size();
		f.write((char*)&size, sizeof(uint32_t));
		f.write(s.data(), s.size());
		
		// save position
		sf::Vector2f pos = text.text.getPosition();
		f.write((char*)&pos, sizeof(sf::Vector2f));

		uint32_t size = text.text.getCharacterSize();
		f.write((char*)&size, sizeof(uint32_t));
	}

	if (f.fail())   return false;

	f.close();

	return true;
}

