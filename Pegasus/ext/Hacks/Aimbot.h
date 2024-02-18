#pragma once
#include <Includes.h>

class Aimbot
{
public:
	inline CPed* BestTarget();
	void Initialize(struct timer timer);
};

inline Aimbot* p_aimbot_func = new Aimbot;