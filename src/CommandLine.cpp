#include "CommandLine.hpp"

#include <sstream>
#include <iostream>


bool parseInt(std::stringstream& ss, Command& command)
{
	std::string param;
	if (ss >> param)
	{
		try
		{
			command.param[0] = std::stoi(param);
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

bool parseColor(std::stringstream& ss, Command& command)
{
	std::string param;
	for (int i = 0; i < 4; ++i)
	{
		if (ss >> param)
		{
			try
			{
				command.param[i] = std::stoi(param);
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

Command parseCommand(const std::string cmdLine, bool *success)
{
	Command command;

	command.param[3] = 255;

	*success = false;

	std::stringstream ss(cmdLine);

	std::string cmd;
	ss >> cmd;

	command.global = (cmd.find("global") != std::string::npos);
	if (cmd.find("line") != std::string::npos)
	{
		command.target = Command::Line;
		if (cmd.find("width") != std::string::npos)
		{
			command.paramType = Command::LineWidth;
			if (!parseInt(ss, command))   return command;
		}
		else if (cmd.find("color") != std::string::npos)
		{
			command.paramType = Command::LineColor;
			if (!parseColor(ss, command))   return command;
		}
		else
		{
			return command;
		}
	}
	else if (cmd.find("circle") != std::string::npos)
	{
		command.target = Command::Circle;
		if (cmd.find("borderWidth") != std::string::npos)
		{
			command.paramType = Command::CircleBorderWidth;
			if (!parseInt(ss, command))   return command;
		}
		else if (cmd.find("fillColor") != std::string::npos)
		{
			command.paramType = Command::CircleFillColor;
			if (!parseColor(ss, command))   return command;
		}
		else if (cmd.find("borderColor") != std::string::npos)
		{
			command.paramType = Command::CircleBorderColor;
			if (!parseColor(ss, command))   return command;
		}
		else
		{
			return command;
		}
	}
	else if (cmd.find("text") != std::string::npos)
	{
		command.target = Command::Text;
		if (cmd.find("size"))
		{
			command.paramType = Command::TextSize;
			if (!parseInt(ss, command))   return command;
		}
		else
		{
			return command;
		}
	}
	else
	{
		return command;
	}

	*success = true;

	return command;
}

