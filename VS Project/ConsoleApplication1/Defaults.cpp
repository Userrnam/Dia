#include "Defaults.hpp"

#include <fstream>
#include <sstream>

#include "AppInfo.hpp"
#include "SetParser.hpp"


bool isWSLine(const std::string& line)
{
	for (auto c : line)
	{
		if (c != ' ' && c != '\n' && c != '\t')  return false;
	}
	return true;
}

bool loadDefaults(AppInfo *info, const std::string& path)
{
	std::fstream f;
	f.open(path, std::ios::in);

	if (f.fail())  return  false;

	while (f)
	{
		std::string line;
		std::getline(f, line);

		// comment
		if (line[0] == '#')  continue;
		if (isWSLine(line))  continue;

		std::stringstream ss(line);

		Param params;
		if (!parseParam(params, ss))  return false;

		switch (params.type)
		{
		case ParamType::LineColor:
			info->defaults.line.color = sf::Color(params.intParam[0], params.intParam[1],
				params.intParam[2], params.intParam[3]);
			break;
		case ParamType::LineWidth:
			info->defaults.line.width = params.intParam[0];
			break;

		case ParamType::CircleFillColor:
			info->defaults.circle.color = sf::Color(params.intParam[0], params.intParam[1],
				params.intParam[2], params.intParam[3]);
			break;
		case ParamType::CircleBorderColor:
			info->defaults.circle.outlineColor = sf::Color(params.intParam[0], params.intParam[1],
				params.intParam[2], params.intParam[3]);
			break;
		case ParamType::CircleBorderWidth:
			info->defaults.circle.outlineThickness = params.intParam[0];
			break;

		case ParamType::TextColor:
			info->defaults.text.color = sf::Color(params.intParam[0], params.intParam[1],
				params.intParam[2], params.intParam[3]);
			break;
		case ParamType::TextSize:
			info->defaults.text.size = params.intParam[0];
			break;
		case ParamType::TextFont:
			{
				auto font = getFont(info, params.strParam);
				if (!font)
					return false;
				info->defaults.text.fontName = params.strParam;
				info->defaults.text.font = font;
			}
			break;

		case ParamType::UISize:
			info->defaults.ui.size   = params.intParam[0];
			break;
		case ParamType::UIFont:
			{
				auto font = getFont(info, params.strParam);
				if (!font)
					return false;
				info->defaults.ui.fontName = params.strParam;
				info->defaults.ui.font = font;
			}
			break;
		}
	}

	f.close();

	return true;
}
