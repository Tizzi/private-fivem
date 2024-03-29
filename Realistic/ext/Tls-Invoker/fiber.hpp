#pragma once
#include <Includes.h>
#include <singeton.hpp>



namespace fiber
{
	using script_func_t = void(*)();

	class GameFiber_Task
	{
	public:
		explicit GameFiber_Task(HMODULE hmod, script_func_t func) :
			m_hmodule(hmod),
			m_function(func)
		{ }

		~GameFiber_Task() noexcept
		{
			if (m_script_fiber)
			{
				DeleteFiber(m_script_fiber);
			}
		}

		HMODULE get_hmodule()
		{
			return m_hmodule;
		}

		script_func_t get_function()
		{
			return m_function;
		}

		void on_tick()
		{
			if (GetTickCount64() < m_wake_at)
				return;

			if (!m_main_fiber)
			{
				m_main_fiber = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(nullptr);
			}

			if (m_script_fiber)
			{
				current_fiber_script = this;
				SwitchToFiber(m_script_fiber);
				current_fiber_script = nullptr;
			}
			else
			{

				m_script_fiber = CreateFiber(0, [](PVOID param)
					{
						auto this_script = static_cast<GameFiber_Task*>(param);
						while (true)
						{
							this_script->m_function();
						}

					}, this);
			}
		}
		void wait(std::uint32_t time)
		{
			m_wake_at = GetTickCount64() + time;
			SwitchToFiber(m_main_fiber);
		}

		inline static GameFiber_Task* get_current_script()
		{
			return current_fiber_script;
		}
	private:
		HMODULE m_hmodule{};
		script_func_t m_function{};

		std::uint32_t m_wake_at{};
		void* m_script_fiber{};
		void* m_main_fiber{};

		inline static GameFiber_Task* current_fiber_script{};
	};
}

class Fiber : public Singleton<Fiber>
{
public:
	std::unique_ptr<fiber::GameFiber_Task> GameFiber;

	void Call();
	void OnNativeThread();
};