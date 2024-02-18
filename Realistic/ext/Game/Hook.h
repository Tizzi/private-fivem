#pragma once
#include <includes.h>

class Hook_Thread_Class
{
public:
	DWORD64 g_ThreadCollection; 
	void Init(); 
	void find_game_thread(); 
};

inline Hook_Thread_Class* g_Game_Hook = new Hook_Thread_Class; 