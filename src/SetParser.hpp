#pragma once

#include <sstream>


enum class Target
{
	Line = 1,
	Circle = 2,
	Text = 3
};

#define _TARGET_NUMBER(stateId, state)\
	state << 3 | ((unsigned)stateId)

enum class ParamType
{
	LineWidth  = _TARGET_NUMBER(Target::Line, 1),
	LineColor  = _TARGET_NUMBER(Target::Line, 2),

	CircleBorderWidth = _TARGET_NUMBER(Target::Circle, 1),
	CircleFillColor   = _TARGET_NUMBER(Target::Circle, 2),
	CircleBorderColor = _TARGET_NUMBER(Target::Circle, 3),

	TextSize  = _TARGET_NUMBER(Target::Text, 1),
	TextColor = _TARGET_NUMBER(Target::Text, 2),
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

