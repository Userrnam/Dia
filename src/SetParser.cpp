#include "SetParser.hpp"


static bool parseInt(std::stringstream& ss, Param& params)
{
	std::string param;
	if (ss >> param)
	{
		try
		{
			params.intParam[0] = std::stoi(param);
		}
		catch (...)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

static bool parseFloat(std::stringstream& ss, Param& params)
{
	std::string param;
	if (ss >> param)
	{
		try
		{
			params.floatParam[0] = std::stof(param);
		}
		catch (...)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

static bool parseColor(std::stringstream& ss, Param& params)
{
	std::string param;
	params.intParam[3] = 255;
	for (int i = 0; i < 4; ++i)
	{
		if (ss >> param)
		{
			try
			{
				params.intParam[i] = std::stoi(param);
				if (params.intParam[i] > 255 || params.intParam[i] < 0)
				{
					return false;
				}
			}
			catch (...)
			{
				return false;
			}
		}
		else
		{
			if (i != 3)  return false;
		}
	}

	return true;
}

bool parseString(std::stringstream& ss, Param& params)
{
	std::string word;
	if (ss >> word)
	{
		params.strParam = word;
		return true;
	}
	return false;
}

bool parseParam(Param& params, std::stringstream& ss)
{
	std::string cmd;
	ss >> cmd;

	if (cmd.find("line") != std::string::npos)
	{
		if (cmd.find("width") != std::string::npos)
		{
			params.type = ParamType::LineWidth;
			return parseInt(ss, params);
		}
		else if (cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::LineColor;
			return parseColor(ss, params);
		}
		else
		{
			return false;
		}
	}
	else if (cmd.find("circle") != std::string::npos)
	{
		if (cmd.find("borderWidth") != std::string::npos ||
				cmd.find("border") != std::string::npos && cmd.find("width") != std::string::npos)
		{
			params.type = ParamType::CircleBorderWidth;
			return parseInt(ss, params);
		}
		else if (cmd.find("fillColor") != std::string::npos ||
				cmd.find("fill") != std::string::npos && cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::CircleFillColor;
			return parseColor(ss, params);
		}
		else if (cmd.find("borderColor") != std::string::npos ||
				cmd.find("border") != std::string::npos && cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::CircleBorderColor;
			return parseColor(ss, params);
		}
		else
		{
			return false;
		}
	}
	else if (cmd.find("text") != std::string::npos)
	{
		if (cmd.find("size") != std::string::npos)
		{
			params.type = ParamType::TextSize;
			return parseInt(ss, params);
		}
		else if (cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::TextColor;
			return parseColor(ss, params);
		}
		else if (cmd.find("font"))
		{
			params.type = ParamType::TextFont;
			return parseString(ss, params);
		}
		else
		{
			return false;
		}
	}
	else if (cmd.find("ui") != std::string::npos)
	{
		if (cmd.find("size") != std::string::npos)
		{
			params.type = ParamType::UISize;
			return parseInt(ss, params);
		}
		else if (cmd.find("font"))
		{
			params.type = ParamType::UIFont;
			return parseString(ss, params);
		}
		else
		{
			return false;
		}
	}
	else if (cmd.find("ux") != std::string::npos)
	{
		if (cmd.find("mouse_wheel_sensetivity") != std::string::npos)
		{
			params.type = ParamType::UXMouseWheelSensetivity;
			return parseFloat(ss, params);
		}
		return false;
	}
	else
	{
		return false;
	}
	
	return true;
}

