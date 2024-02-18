#include <Hook.h>

void Hook_Thread_Class::Init()
{
	// get a free thread slot func
	this->g_ThreadCollection = g_Memory->find_pattern(0, Encrypt("48 8B ? ? ? ? ? 8B CA 4C 8B 0C C8 45"), 7, 0);
	//dwThreadCollectionPtr = Memory::PatternScan("48 8B ? ? ? ? ? 8B CA 4C 8B 0C C8 45", NULL, 7);
	//Log::Msg("g_ThreadCollection %llx", g_ThreadCollection);
	if (!g_ThreadCollection)
	{
		SAFE_CALL(MessageBoxA)(0, Encrypt("Unable to find Thread Pool"), Encrypt("sys"), 0);
	}
}

void Hook_Thread_Class::find_game_thread()
{
	if (this->g_ThreadCollection)
	{
		auto x1 = *(DWORD64*)g_ThreadCollection;
		Log::Msg("x1 %llx", x1);
		auto x2 = *(DWORD64*)x1;
		Log::Msg("x2 %llx", x2);
		// Create thread
		DWORD64 threadID = g_pattern->oGameThread(x1);
		//Log::Msg("threadID %llx", threadID);
		if (threadID)
		{
			//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)0x0, 0, 0, 0);
		}

	}
 }