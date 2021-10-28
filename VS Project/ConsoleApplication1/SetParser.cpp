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
			if (!parseInt(ss, params))   return false;
		}
		else if (cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::LineColor;
			if (!parseColor(ss, params))   return false;
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
			if (!parseInt(ss, params))   return false;
		}
		else if (cmd.find("fillColor") != std::string::npos ||
				cmd.find("fill") != std::string::npos && cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::CircleFillColor;
			if (!parseColor(ss, params))   return false;
		}
		else if (cmd.find("borderColor") != std::string::npos ||
				cmd.find("border") != std::string::npos && cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::CircleBorderColor;
			if (!parseColor(ss, params))   return false;
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
			//std::count << "HellO" << std::endl;
			if (!parseInt(ss, params))   return false;
			//std::count << "LOL" << std::endl;
		}
		else if (cmd.find("color") != std::string::npos)
		{
			params.type = ParamType::TextColor;
			if (!parseColor(ss, params)) return false;
		}
		else if (cmd.find("font"))
		{
			params.type = ParamType::TextFont;
			if (!parseString(ss, params))  return false;
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
			if (!parseInt(ss, params))  return false;
		}
		else if (cmd.find("font"))
		{
			params.type = ParamType::UIFont;
			if (!parseString(ss, params))  return false;
		}
		else
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
