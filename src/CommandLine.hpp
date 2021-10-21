#pragma once

#include <string>
#include "SetParser.hpp"

struct Command
{
	enum Type
	{
		Set, Export, Save, Load, NoneType, ResetZoom,
	};

	enum Scope
	{
		Global, Local, Defaults, NoneScope
	};

	Scope scope = NoneScope;

	Type type = NoneType;
	ParamType paramType = ParamType::None;

	int intParam[4] = {};
	float floatParam[4] = {};
	std::string stringParam[4];
};

Command parseCommand(const std::string cmd, bool* success);

