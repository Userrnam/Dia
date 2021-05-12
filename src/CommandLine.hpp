#pragma once

#include <string>
#include "SetParser.hpp"

struct Command
{
	enum Type
	{
		Set, Export, Save, Load
	};

	enum Scope
	{
		Global, Local, Defaults
	};

	Scope scope;

	Type type;
	ParamType paramType;

	int intParam[4];
	std::string stringParam[4];
};

Command parseCommand(const std::string cmd, bool *success);

