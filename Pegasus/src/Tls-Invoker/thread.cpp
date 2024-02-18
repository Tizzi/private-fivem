#include "thread.hpp"
#include "fiber.hpp"

void Thread::CallT(game_thread* this_ptr, int ops_to_execute) {
	//if (this_ptr->m_context.m_script_hash == 0x26FB4AB9 || this_ptr->m_context.m_script_hash == 1104607124 || this_ptr->m_context.m_script_hash == 3381724246)
	//Log::Msg("[+] name: %s || id: %d || script hash: %llx || opsto_execute %llx", this_ptr->m_name, this_ptr->m_context.m_thread_id, this_ptr->m_context.m_script_hash, ops_to_execute);
	if (this_ptr->m_context.m_script_hash == STARTUP || this_ptr->m_context.m_script_hash == MAIN_PERSISTENT) //0xe6b4a2e
	{
		
		//Log::Msg("[+] script id: %llx || opsto_execute %llx", this_ptr->m_context.m_thread_id, ops_to_execute);
		auto& thread = game_thread::GetActiveThread(g_pattern->dwTlsOffset);

		auto orig_thread = thread;

		thread = this_ptr;


		Fiber::Instance().GameFiber->on_tick();
		
		thread = orig_thread;
	}

}