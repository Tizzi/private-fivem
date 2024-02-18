#pragma once
#include <Includes.h>
#include <invoker.hpp>
#include <GameMath.h>
#include <hashes.hpp>

class World_Func
{
public:
	void Initialize(struct timer timer);
};

inline World_Func* p_world_func = new World_Func;