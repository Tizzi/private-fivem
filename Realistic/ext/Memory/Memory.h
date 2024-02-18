#pragma once
#include <Includes.h>

class Memory
{
public:

	uintptr_t find_pattern_ex(uintptr_t baseaddy, const char* pattern, size_t index);

	uintptr_t relative_addr(uintptr_t addr, int length);

	uintptr_t find_pattern(const char* szModuleName, const char* Pattern, int Index, size_t index);

	uintptr_t sys_calls();

	uintptr_t resolvePtr(const char* szModuleName, int64_t targetValue, BYTE compareByte);

	uintptr_t find_pattern_reg(uintptr_t baseaddy, const char* pattern, size_t index);

};

inline Memory* g_Memory = new Memory;