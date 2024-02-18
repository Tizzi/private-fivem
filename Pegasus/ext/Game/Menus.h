#pragma once
#include <Includes.h>
#include "font_awesome_5.h"
#include "../keyauth/auth.hpp"
#include <invoker.hpp>
#include <Renderer.h>
#include <resources.h>
#include <GameMath.h>

using namespace std;
using namespace KeyAuth;

inline auto name = Encrypt("Learniiing");
inline auto ownerid = Encrypt("JVuWQlrYYB");
inline auto secret = Encrypt("65453ea0f4125000cdf5dcebaf731c7d69b6fad5ae01377eb02effc8811e6b54");
inline auto version = Encrypt("1.05");
inline auto url = Encrypt("https://keyauth.win/api/1.2/");
inline auto sslPin = Encrypt("ssl pin key (optional)");
inline api KeyAuthApp(name.decrypt(), ownerid.decrypt(), secret.decrypt(), version.decrypt(), url.decrypt(), sslPin.decrypt());

static float Height_Y = 0;
static float Width_X = 0;
static int subtab = 0;
static int tab = 5;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//#define HELPMARKER(str) ImGui::SameLine(); ImGui::TextColored(ImColor(220, 190, 0, 255), "[?]"); if (ImGui::IsItemHovered()) ImGui::SetTooltip(str)
#define HELPMARKER(str) if (ImGui::IsItemHovered()) ImGui::SetTooltip(str)
static Config_Manager* cfg_manager = new Config_Manager;
static std::vector<std::string> cfgs = cfg_manager->GetConfigs();


static bool realshow = false;
static bool loading = false;
static bool loadig_open = false;
static bool Authenticated = false;
static bool auth_login = true;
static bool auth_register = false;

		//Authenticated = true;
		//auth_login = false;

void initiating(IDXGISwapChain* pSwapChain);
void LoadMainMenu();
inline int PiePopupSelectMenu(const ImVec2& center, const char* popup_id, const char** items, int items_count, int* p_selected);
//inline void radialMenu();
//inline void Login_register();
inline void MainMenu();
inline std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);

#pragma region HideConsole
inline void xxx()
{
	HWND Stealth;
	SAFE_CALL(AllocConsole)();
	Stealth = SAFE_CALL(FindWindowA)("ConsoleWindowClass", NULL);
	SAFE_CALL(ShowWindow)(Stealth, 0);
}
#pragma endregion

inline LRESULT __stdcall WndHandling(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(wnd_proc, hWnd, uMsg, wParam, lParam);
}

/*
LRESULT __stdcall WndHandling(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (auth_login || auth_register)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}


	return CallWindowProc(wnd_proc, hWnd, uMsg, wParam, lParam);
}*/