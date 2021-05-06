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

	bool global;
	Target target;
	Param paramType;
	int param[4];
};

Command parseCommand(const std::string cmd, bool *success);

