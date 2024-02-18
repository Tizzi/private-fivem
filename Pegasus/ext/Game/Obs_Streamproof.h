#pragma once
#include <Includes.h>

class ObsStreamproof
{
public:
	int obsPresent(void* pSwapChain, void* pBackBuffer, bool b);
};

inline ObsStreamproof* g_obs = new ObsStreamproof; 