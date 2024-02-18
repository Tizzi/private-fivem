#include <Obs_Streamproof.h>

using ObsOrigin = int(__cdecl*)(void*, void*, bool);
ObsOrigin oReturn = nullptr; 

int ObsStreamproof::obsPresent(void* pSwapChain, void* pBackBuffer, bool b)
{
	oReturn = (ObsOrigin)g_Memory->find_pattern(Encrypt("graphics-hook64.dll"), Encrypt("48 89 5C 24 20 56 48 83 EC 30 0F B6 05 D3 9B 07 00 48 8B F2"), 0, 0); 
	int iRet = oReturn(pSwapChain, pBackBuffer, b);

	return iRet; 
}
