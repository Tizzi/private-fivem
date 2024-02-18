#pragma once
#include <Includes.h>

class PlayerFuncs
{
public: 
	void Initialize(struct timer timer);
};
inline void shootPed(int player, CPed* Target, int WeaponHash);
inline void StartNetworkFxAtCoord(float x, float y, float z, const char* FxName, const char* FxAsset);
inline PlayerFuncs* g_player_func = new PlayerFuncs;