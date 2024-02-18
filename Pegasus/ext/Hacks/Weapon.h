#pragma once
#include <Includes.h>

class WeaponFuncs
{
public:
	void Initialize(struct timer timer);
};

inline WeaponFuncs* g_weapon_func = new WeaponFuncs;