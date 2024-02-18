#include <Memory.h>
#define BaseAddShift 0x13000
uintptr_t Memory::find_pattern_ex(uintptr_t baseaddy, const char* pattern, size_t index)
{
	static auto patternToByte = [](const char* pattern)
	{
		auto       bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else
				bytes.push_back(strtoul((const char*)current, &current, 16));
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)baseaddy;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)baseaddy + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto       patternBytes = patternToByte(pattern);
	baseaddy += BaseAddShift;
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(baseaddy);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	size_t nFoundResults = 0;
	//Log::Msg("baseaddy: %llx || sizeOfImage: %llx", baseaddy, sizeOfImage);
	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;

		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			if (index != 0)
			{
				if (nFoundResults < index)
				{
					nFoundResults++;
					found = false;
				}
				else
					return reinterpret_cast<uint64_t>(&scanBytes[i]);
			}
			else
				return reinterpret_cast<uint64_t>(&scanBytes[i]);
		}
	}
	//Log::Msg("NotFound");
	return NULL;
}
uintptr_t scanMemoryRegion(uintptr_t address, size_t size, std::vector<int> Pattern, size_t index) {
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(address);
	const auto s = Pattern.size();
	const auto d = Pattern.data();
	size_t nFoundResults = 0;
	for (auto i = 0ul; i < size - s; ++i)
	{
		bool found = true;

		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			if (index != 0)
			{
				if (nFoundResults < index)
				{
					nFoundResults++;
					found = false;
				}
				else
					return reinterpret_cast<uint64_t>(&scanBytes[i]);
			}
			else
				return reinterpret_cast<uint64_t>(&scanBytes[i]);
		}
	}

	return false; // Pattern not found
}
uintptr_t Memory::find_pattern_reg(uintptr_t baseaddy, const char* pattern, size_t index)
{
	static auto patternToByte = [](const char* pattern)
	{
		auto       bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else
				bytes.push_back(strtoul((const char*)current, &current, 16));
		}
		return bytes;
	};


	auto       patternBytes = patternToByte(pattern);
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	MEMORY_BASIC_INFORMATION memoryInfo;
	const void* address = (HMODULE)baseaddy + BaseAddShift;//systemInfo.lpMinimumApplicationAddress;
	//(VirtualQueryEx(GetModuleHandleA(0), address, &memoryInfo, sizeof(memoryInfo)) != 0UL) {
	while (SAFE_CALL(VirtualQuery)(address, &memoryInfo, sizeof(memoryInfo)) != 0UL) {//(address < systemInfo.lpMaximumApplicationAddress) {
		//Log::Msg("BaseAddress: %llx || RegionSize: %llx", memoryInfo.BaseAddress, memoryInfo.RegionSize);

		if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && memoryInfo.Protect != PAGE_GUARD) {

			auto result = scanMemoryRegion((uintptr_t)memoryInfo.BaseAddress, memoryInfo.RegionSize, patternBytes, index);
			return result;
		}
		else
			//Log::Msg("PAGE_GUARD %llx || BaseAddress: %llx || RegionSize: %llx", memoryInfo.Protect, memoryInfo.BaseAddress, memoryInfo.RegionSize);
			address = (const BYTE*)memoryInfo.BaseAddress + memoryInfo.RegionSize;

	}
	//Log::Msg("NOTFOUND");

	return NULL;
}
uintptr_t Memory::relative_addr(uintptr_t addr, int length)
{
	DWORD Offset = *(DWORD*)(addr + (length - 4));
	return addr + length + Offset;
}

uintptr_t Memory::find_pattern(const char* szModuleName, const char* Pattern, int relative_addr_length, size_t index)
{
	
	auto mdl = GetModuleHandleA(szModuleName);
	if (!mdl) return 0;
	auto ret = find_pattern_ex((uintptr_t)mdl, Pattern, index);

	if (relative_addr_length != 0)
		ret = relative_addr(ret, relative_addr_length);
	if (!ret)
	{
		//Log::Msg("modl: %s || %s", szModuleName, Pattern);
		//Log::Msg("Fail: %s + 0x%llx", szModuleName, ret - (uintptr_t)mdl);
		exit(0);
	}
	
	return ret;
}

uintptr_t Memory::resolvePtr(const char* szModuleName, int64_t targetValue, BYTE compareByte) {
	uintptr_t baseaddy = (uintptr_t)GetModuleHandleA(szModuleName);
	const auto dosHeader = (PIMAGE_DOS_HEADER)baseaddy;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)baseaddy + dosHeader->e_lfanew);
	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;

	int64_t currentAddress = baseaddy + BaseAddShift;

	while (currentAddress < baseaddy + sizeOfImage) {
		auto cp = *(int64_t*)currentAddress;
		if (cp == targetValue) {
			if (*(BYTE*)cp == compareByte)
				return currentAddress;
		}
		currentAddress++;
	}

	return NULL;
}