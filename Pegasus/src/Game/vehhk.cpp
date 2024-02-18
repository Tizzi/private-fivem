#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <vehhk.h>
#ifdef _WIN64
#define XIP Rip
#else
#define XIP Eip
#endif
uintptr_t LeoHook::og_fun = 0;
uintptr_t LeoHook::hk_fun = 0;
PVOID LeoHook::VEH_Handle = nullptr;
DWORD LeoHook::oldProtection = 0;


bool LeoHook::Hook(uintptr_t original_fun, uintptr_t hooked_fun)
{
	LeoHook::og_fun = original_fun;
	LeoHook::hk_fun = hooked_fun;

	int a = 10;
	//We cannot hook two functions in the same page, because we will cause an infinite callback
	if (AreInSamePage((const uint8_t*)og_fun, (const uint8_t*)hk_fun))
		return false;
	int b = a;
	//Register the Custom Exception Handler
	VEH_Handle = AddVectoredExceptionHandler(0, (PVECTORED_EXCEPTION_HANDLER)LeoHandler);
	int c = b;
	//Toggle PAGE_GUARD flag on the page
	if (VEH_Handle && SAFE_CALL(VirtualProtect)((LPVOID)og_fun, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &oldProtection))
		return true;
	int d = c;
	return false;
}
void LeoHook::Apply()
{
	DWORD dwOld;
	SAFE_CALL(VirtualProtect)((LPVOID)og_fun, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld); //Reapply the PAGE_GUARD flag
}

void LeoHook::Deapply()
{
	DWORD dwOld;
	SAFE_CALL(VirtualProtect)((LPVOID)og_fun, 1, PAGE_EXECUTE_READ, &dwOld); //Remove the PAGE_GUARD flag
}
bool LeoHook::Unhook()
{
	DWORD old;
	if (VEH_Handle && //Make sure we have a valid Handle to the registered VEH
		SAFE_CALL(VirtualProtect)((LPVOID)og_fun, 1, oldProtection, &old) && //Restore old Flags
		RemoveVectoredExceptionHandler(VEH_Handle)) //Remove the VEH
		return true;

	return false;
}

LONG WINAPI LeoHook::LeoHandler(EXCEPTION_POINTERS* pExceptionInfo)
{
	/*
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ILLEGAL_INSTRUCTION)
	{
		Log::Msg("STATUS_ILLEGAL_INSTRUCTION: %llx ", pExceptionInfo->ContextRecord->XIP);
		return EXCEPTION_CONTINUE_EXECUTION;//bypass veh hook detect
	}
	*/
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION)
	{
		Log::Msg("crashed: %llx add: %llx", pExceptionInfo->ContextRecord->XIP, pExceptionInfo->ContextRecord->R15);
		//exit(0);
		return EXCEPTION_CONTINUE_SEARCH;//handle crash
	}


	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) //We will catch PAGE_GUARD Violation
	{

		if (pExceptionInfo->ContextRecord->XIP == (uintptr_t)og_fun) //Make sure we are at the address we want within the page
		{
			//Log::Msg("Bulletptr: %llx || weaponinfo: %llx || startpos x: %llx || endpos: %llx || Range: %llx || WeapHash %llx || a7 %d || a8 %d",
			//	pExceptionInfo->ContextRecord->Rcx, pExceptionInfo->ContextRecord->Rdx, pExceptionInfo->ContextRecord->R8, pExceptionInfo->ContextRecord->R9, &pExceptionInfo->ContextRecord->Xmm0.Low, pExceptionInfo->ContextRecord->Rsp +0x30, pExceptionInfo->ContextRecord->Rdi, pExceptionInfo->ContextRecord->Rdi);
			pExceptionInfo->ContextRecord->XIP = (uintptr_t)hk_fun; //Modify EIP/RIP to where we want to jump to instead of the original function
			//pExceptionInfo->ContextRecord->EFlags |= 0x100;
			//Log::Msg("im Leo");
		}
		else
		{
			//Log::Msg("Not Original %llx", pExceptionInfo->ContextRecord->XIP);

			//pExceptionInfo->ContextRecord->EFlags |= 0x100; //Will trigger an STATUS_SINGLE_STEP exception right after the next instruction get executed. In short, we come right back into this exception handler 1 instruction later
		}
		return EXCEPTION_CONTINUE_EXECUTION; //Continue to next instruction
	}

	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) //We will also catch STATUS_SINGLE_STEP, meaning we just had a PAGE_GUARD violation
	{
		DWORD dwOld;

		SAFE_CALL(VirtualProtect)((LPVOID)og_fun, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld); //Reapply the PAGE_GUARD flag because everytime it is triggered, it get removes



		return EXCEPTION_CONTINUE_EXECUTION; //Continue the next instruction
	}

	return EXCEPTION_CONTINUE_SEARCH; //Keep going down the exception handling list to find the right handler IF it is not PAGE_GUARD nor SINGLE_STEP
}

bool LeoHook::AreInSamePage(const uint8_t* Addr1, const uint8_t* Addr2)
{
	MEMORY_BASIC_INFORMATION mbi1;
	if (!VirtualQuery(Addr1, &mbi1, sizeof(mbi1))) //Get Page information for Addr1
		return true;

	MEMORY_BASIC_INFORMATION mbi2;
	if (!VirtualQuery(Addr2, &mbi2, sizeof(mbi2))) //Get Page information for Addr1
		return true;

	if (mbi1.BaseAddress == mbi2.BaseAddress) //See if the two pages start at the same Base Address
		return true; //Both addresses are in the same page, abort hooking!

	return false;
}
