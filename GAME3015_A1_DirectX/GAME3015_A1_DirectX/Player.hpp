#pragma once
#include "Command.hpp"
#include "../../Common/d3dUtil.h"
#include <map>

class CommandQueue;

class Player
{
public:
	Player();
	void handleEvent(CommandQueue& commands, const GameTimer& gt);
	void handleRealtimeInput(CommandQueue& commands, const GameTimer& gt);
};

