#pragma once

#include <sstream>


enum class Target
{
	Line   = 1,
	Circle = 2,
	Text   = 3
};

#define _TARGET_NUMBER(stateId, state)\
	state << 3 | ((unsigned)stateId)

enum class ParamType
{
	LineWidth,
	LineColor,

	CircleBorderWidth,
	CircleFillColor,
	CircleBorderColor,

	TextSize,
	TextColor,
};

inline Target getTarget(ParamType type)
{
	return (Target)((unsigned)type & 0b111);
}

struct Param
{
	ParamType type;
	int intParam[4];
};

bool parseParam(Param& param, std::stringstream& ss);

