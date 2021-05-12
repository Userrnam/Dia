#include "Defaults.hpp"

#include <fstream>
#include <sstream>

#include "SetParser.hpp"


bool isWSLine(const std::string& line)
{
	for (auto c : line)
	{
		if (c != ' ' && c != '\n' && c != '\t')  return false;
	}
	return true;
}

Defaults loadDefaults(const std::string& path, bool *success)
{
	Defaults res;
	*success = false;

	std::fstream f;
	f.open(path, std::ios::in);

	if (f.fail())  return res;

	while (f)
	{
		std::string line;
		std::getline(f, line);

		if (isWSLine(line))  continue;

		std::stringstream ss(line);

		Param params;
		if (!parseParam(params, ss))  return res;

		switch (params.type)
		{
			case ParamType::LineColor:
				res.line.color = sf::Color(params.intParam[0], params.intParam[1],
											   params.intParam[2], params.intParam[3]);
				break;
			case ParamType::LineWidth:
				res.line.width = params.intParam[0];
				break;

			case ParamType::CircleFillColor:
				res.circle.color = sf::Color(params.intParam[0], params.intParam[1],
											   params.intParam[2], params.intParam[3]);
				break;
			case ParamType::CircleBorderColor:
				res.circle.outlineColor = sf::Color(params.intParam[0], params.intParam[1],
											   params.intParam[2], params.intParam[3]);
				break;
			case ParamType::CircleBorderWidth:
				res.circle.outlineThickness = params.intParam[0];
				break;

			case ParamType::TextColor:
				res.text.color = sf::Color(params.intParam[0], params.intParam[1],
											   params.intParam[2], params.intParam[3]);
				break;

			case ParamType::TextSize:
				res.text.size = params.intParam[0];
				break;
		}
	}

	f.close();

	*success = true;

	return res;
}

