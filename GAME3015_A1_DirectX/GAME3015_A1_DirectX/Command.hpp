#pragma once

#include "Category.hpp"
#include "../../Common/GameTimer.h"
#include <functional>
#include <cassert>

class SceneNode; 

struct Command
{
	Command();

	std::function<void(SceneNode&, const GameTimer& gt)> action;
	unsigned int category; 

};

template <typename GameObject, typename Function>
std::function<void(SceneNode&, const GameTimer& gt)> derivedAction(Function fn)
{
	return [=](SceneNode& node, const GameTimer& gt)
	{
		assert(dynamic_cast<GameObject*>(&node) != nullptr);

		fn(static_cast<GameObject&>(node), gt);
	};
}