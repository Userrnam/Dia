#pragma once

#include <string>


struct Command
{
	enum Target
	{
		Line, Circle, Text
	};

	enum Param
	{
		LineWidth,
		LineColor,

		CircleBorderWidth,
		CircleFillColor,
		CircleBorderColor,

		TextSize,
	};

	enum Type
	{
		Set, Export
	};

	bool global;

	Type type;
	Target target;
	Param paramType;

	int intParam[4];
	std::string stringParam[4];
};

Command parseCommand(const std::string cmd, bool *success);

