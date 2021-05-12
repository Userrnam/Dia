#pragma once

#include "Elements.hpp"
#include "Selection.hpp"
#include "History.hpp"
#include "Defaults.hpp"


#define _STATE_NUMBER(stateId, state)\
	state << 3 | ((unsigned)stateId)

enum class StateType
{
	Create = 1,
	Edit   = 2,
	Cmd    = 3,
	None   = 4,
};

enum class State
{
	None         = _STATE_NUMBER(StateType::None, 1),
	CommandLine  = _STATE_NUMBER(StateType::Cmd, 1),

	EPoint                = _STATE_NUMBER(StateType::Edit, 1),
	EMovingLinePoint      = _STATE_NUMBER(StateType::Edit, 2),
	EMovingCirclePoint    = _STATE_NUMBER(StateType::Edit, 3),
	EMovingLine           = _STATE_NUMBER(StateType::Edit, 4),
	EMovingSelection      = _STATE_NUMBER(StateType::Edit, 5),
	ESelectElement        = _STATE_NUMBER(StateType::Edit, 6),
	ESelectEnd            = _STATE_NUMBER(StateType::Edit, 7),
	ESelectionRectangle   = _STATE_NUMBER(StateType::Edit, 8),
	EChangingCircleRadius = _STATE_NUMBER(StateType::Edit, 9),
	EMovingText           = _STATE_NUMBER(StateType::Edit, 10),
	EMovingCopy           = _STATE_NUMBER(StateType::Edit, 11),
	EEditText             = _STATE_NUMBER(StateType::Edit, 12),

	CLine         = _STATE_NUMBER(StateType::Create, 1),
	CNewLine      = _STATE_NUMBER(StateType::Create, 2),
	CCircle       = _STATE_NUMBER(StateType::Create, 3),
	CNewCircle    = _STATE_NUMBER(StateType::Create, 4),
	CText         = _STATE_NUMBER(StateType::Create, 5),
	CNewText      = _STATE_NUMBER(StateType::Create, 6),
};

inline StateType getStateType(State state)
{
	return (StateType)((unsigned)state & 0b111);
}

struct AppInfo
{
	// window
	sf::RenderWindow *window;
	sf::Vector2i windowSize;

	History history;
	uint64_t elementId = 0;

	// TODO defautlts
	int characterSize = 30;
	int gridSize = 64;
	sf::Font font;
	sf::Vector2f fontCharSize;

	// key pressed
	bool shiftPressed = false;
	bool snapping     = true;

	sf::Vector2f snappedPos;

	Defaults defaults;

	// cammera
	sf::View camera;
	float cameraZoom = 1.0f;
	sf::View defaultView;

	// Drawables
	std::vector<Line> lines;
	std::vector<Circle> circles;
	std::vector<Text> texts;

	State state = State::CLine;
	State previousState = State::None;

	std::string error;

	// copy paste from EditMode
	// TODO clean up
	sf::Vector2f *pVec = 0;

	Selection selection;
	sf::FloatRect selectionRectangle;

	CopyInfo copyInfo;

	sf::Vector2f movingSelectionReferencePoint;

	State possibleNextState = State::None;

	Circle *pCircle = nullptr;
	Text   *pText   = nullptr;
	Line   *pLine   = nullptr;

	sf::Vector2f point;
	sf::Vector2f referencePoint;
};

