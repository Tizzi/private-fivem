#include <scripthook.hpp>
#include <spoof_call.h>
#include <thread.hpp>
#include <fiber.hpp>

using script_thread_tick_t = uintptr_t(*)(game_thread* thread, int ops_to_execute);
uintptr_t ptr_gta_script_thread_tick = 0;
script_thread_tick_t original_native_thread = nullptr;

uintptr_t hooked_native_thread(game_thread* this_ptr, int ops_to_execute) {
	Thread::Instance().CallT(this_ptr, ops_to_execute);//get this pointer and ops_to_execute


	//Log::Msg("[+] name: %s || id: %d || script hash: %llx || opsto_execute %llx", this_ptr->m_name, this_ptr->m_context.m_thread_id, this_ptr->m_context.m_script_hash, ops_to_execute);
	//return return_spoofer::spoof_call(jmp_rdx, original_native_thread, this_ptr, ops_to_execute);
	return original_native_thread(this_ptr, ops_to_execute);
}
void Hooks::Thread() {
	Fiber::Instance().Call();
	/*
	BYTE scrIndex;
	if (!strcmp(g_offsets->builtNumber, "2802"))
		scrIndex = 32 + 8 * 3;
	else if (!strcmp(g_offsets->builtNumber, "2944"))
		scrIndex = 64 + 8 * 3;
	else
		scrIndex = 32 + 8 * 4;
	*/
	//SwapVtab Hook
	//Log::Msg("[+] scrMeta >> %llx", scrMeta);
	ptr_gta_script_thread_tick = g_pattern->tickFuncPtr;//(script_thread_tick_t)(scrMeta + scrIndex);
	//Log::Msg("[+] ptr_gta_script_thread_tick >> %llx", ptr_gta_script_thread_tick);
	original_native_thread = (script_thread_tick_t)(*(uint64_t*)ptr_gta_script_thread_tick);
	//Log::Msg("[+] original_native_thread >> %llx", original_native_thread);
	*(intptr_t*)ptr_gta_script_thread_tick = (intptr_t)&hooked_native_thread;
	//Log::Msg("[+] &hooked_native_thread >> %llx", &hooked_native_thread);
}
void Hooks::Initialize() {
	//Log::Msg("[+] H000ks >> Initializing h000ks...");

	this->Thread();

}
void Hooks::UnHook() {
	Log::Msg("[+] H000ks >> un-h000ks...");
	*(intptr_t*)ptr_gta_script_thread_tick = (intptr_t)original_native_thread;

}