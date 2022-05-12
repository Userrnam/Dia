#include "CommandLine.hpp"

#include <sstream>
#include <iostream>
#include <string.h>


static bool handleSet(Command& command, std::stringstream& ss)
{
	Param params;

	// this is dumb
	auto ts = ss.str();
	std::stringstream newss(ts);
	std::string first;
	newss >> first;
	newss >> first;

	if (first.find("global") != std::string::npos)       command.scope = Command::Global;
	else if (first.find("default") != std::string::npos) command.scope = Command::Defaults;
	else command.scope = Command::Local;

	if (!parseParam(params, ss))  return false;

	command.paramType = params.type;
	memcpy(command.intParam, params.intParam, 4 * sizeof(int));
	memcpy(command.floatParam, params.floatParam, 4 * sizeof(float));

	command.stringParam[0] = params.strParam;

	return true;
}

// format is name scale in percent
static bool handleExport(Command& command, std::stringstream& ss)
{
	// read name
	if (ss >> command.stringParam[0])
	{
		std::string word;

		// read scale
		if (ss >> word)
		{
			try
			{
				command.intParam[0] = std::stoi(word);
			}
			catch (...)
			{
				return false;
			}
		}
		else
		{
			// default to 100 if scale was not specified
			command.intParam[0] = 100;
		}
	}
	else
	{
		return false;
	}

	return true;
}

static bool handleLoad(Command& command, std::stringstream& ss)
{
	std::string word;

	if (ss >> word)
	{
		command.stringParam[0] = word;
		return true;
	}
	else
	{
		return false;
	}
}

static bool handleSave(Command& command, std::stringstream& ss)
{
	std::string word;

	if (ss >> word)
	{
		command.stringParam[0] = word;
		return true;
	}
	else
	{
		return false;
	}
}

Command parseCommand(const std::string cmdLine, bool* success)
{
	Command command;

	// in case we read color
	command.intParam[3] = 255;
	std::stringstream ss(cmdLine);

	std::string type;
	if (ss >> type)
	{
		if (type == ":set")
		{
			command.type = Command::Set;
			*success = handleSet(command, ss);
		}
		else if (type == ":export")
		{
			command.type = Command::Export;
			*success = handleExport(command, ss);
		}
		else if (type == ":load")
		{
			command.type = Command::Load;
			*success = handleLoad(command, ss);
		}
		else if (type == ":save")
		{
			command.type = Command::Save;
			*success = handleSave(command, ss);
		}
		else if (type == ":reset_zoom")
		{
			command.type = Command::ResetZoom;
			*success = true;
		}
	}
	else
	{
		*success = false;
	}

	return command;
}
