#include <Includes.h>
#include<minhook.h>			//
#include <scripthook.hpp>
#include <spoof_call.h>
#include <GameMath.h>
#include <3DRadar.h>
#include <invoker.hpp>
#include <Menus.h>

// Export special symbol to identify modules as ReShade instances
extern "C" __declspec(dllexport) const char* ReShadeVersion = "5.8.0";




HRESULT __stdcall Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	/*
	static uintptr_t gameoverlay_return_address = 0;

	if (!gameoverlay_return_address) {
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery(_ReturnAddress(), &info, sizeof(MEMORY_BASIC_INFORMATION));

		char mod[MAX_PATH];
		GetModuleFileNameA((HMODULE)info.AllocationBase, mod, MAX_PATH);
		
		if (strstr(mod, "graphics"))//rage-graphics-five.AddTextureOverride+1A28 //!strstr(mod, "graphics") && !strstr(mod, "FiveM")
			gameoverlay_return_address = (uintptr_t)(_ReturnAddress());
	}
	int iReturn = 0;
	if (gameoverlay_return_address != (uintptr_t)(_ReturnAddress()) && true)
	{
		//Log::Msg("gameoverlay_return_address : %llx | %llx", gameoverlay_return_address, _ReturnAddress());
		return true;
		//return oPresent(pSwapChain, SyncInterval, Flags);
		//iReturn = oPresent(pSwapChain, SyncInterval, Flags);
		//return iReturn;
	}*/
	static bool initIMGUI = true;
	if (initIMGUI)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			
			initiating(pSwapChain);

			initIMGUI = 0;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	if (0)//p_variables->p_resource.enable
	{
		swapchain_vtables = *(DWORD_PTR**)(g_pattern->g_swapchain);
		swapchain_vtables[8] = swapchain_backup;
		g_tls_hook->UnHook();
		return oPresent(pSwapChain, SyncInterval, Flags);
	}
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 20, ImVec2(10, 20), ImColor(249, 224, 9, 255) , std::to_string((INT8)ImGui::GetIO().Framerate / 2).c_str());
	


	
	LoadMainMenu();



	// Render imgui::notify
	// You should push style vars here
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
	ImGui::RenderNotifications();
	ImGui::PopStyleVar(1); // Don't forget to Pop()
	ImGui::PopStyleColor(1);
	ImGui::End();
	
	p_radar->Initialize();
	
	//g_obs->obsPresent(pSwapChain, rendertarget, 1); 

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &rendertarget, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	//return return_spoofer::spoof_call(jmp_rdx, oPresent, pSwapChain, SyncInterval, Flags);
	return oPresent(pSwapChain, SyncInterval, Flags);
}

//HWND ForeGroundWindow = NULL;


Pattern::HandleBullet_t hk_originalHandleBullet = 0;
int bCount = 0;

__int64 mHandleBullet(intptr_t BulletPtr, intptr_t CWeaponInfo, Vector3 StartPos, Vector3 EndPos, float Range, int weaponHash, bool a7 , bool a8) {
	

	//Log::Msg("Bulletptr: %llx || weaponinfo: %llx || startpos x: %llx || endpos: %llx || Range: %llx || WeapHash %llx || a7 %d || a8 %d", BulletPtr, CWeaponInfo,&StartPos, &EndPos, &Range, &weaponHash, a7, a8);
	//return g_pattern->HandleBullet(BulletPtr, CWeaponInfo, StartPos, EndPos, Range, weaponHash, a7, a8);
	//intptr_t result = g_pattern->HandleBullet(BulletPtr, CWeaponInfo, StartPos, EndPos, Range, weaponHash, a7, a8);

	bCount++;
	
	//return result;
	float hitChancePerc = (float)p_variables->p_aimbot.hitChance / 100;
	if (hitChancePerc * bCount >= 1 && a8)
	{
		CPed* best_target = (CPed*)p_variables->p_aimbot.currentClosestPedID;
		if (best_target)
		{
			//Log::Msg("redirecting..");
			Vector3 best_target_pos = Vector3{ 0,0,0 };

			switch (p_variables->p_aimbot.aimbot_bone)
			{
			case 0:
				best_target_pos = g_Math->GetBone(best_target, ePedBoneType::HEAD);
				break;
			case 1:
				best_target_pos = g_Math->GetBone(best_target, ePedBoneType::NECK);
				break;
			case 2:
				best_target_pos = g_Math->GetBone(best_target, ePedBoneType::ABDOMEN);
				break;
			case 3:
				best_target_pos = g_Math->GetBone(best_target, ePedBoneType::L_HAND);
				break;
			case 4:
				best_target_pos = g_Math->GetBone(best_target, ePedBoneType::R_HAND);
				break;
			}

			if (p_variables->p_aimbot.magic_aim)
				StartPos = best_target_pos + Vector3{0.07, 0.07f, 0.07f};
			EndPos = best_target_pos - Vector3{0.09f, 0.09f, 0.09f};
			//Log::Msg("Final..");
			bCount = 0;
			p_variables->p_aimbot.currentClosestPedID = 0;
		}

	}

	
	intptr_t result = g_pattern->HandleBullet(BulletPtr, CWeaponInfo, StartPos, EndPos, Range, weaponHash, a7, a8);
	

	return result;
}

/*
void hookBullet()
{
	//Pattern::HandleBullet_t HandleBulletcall = (Pattern::HandleBullet_t)((uintptr_t)GetModuleHandleA(0) + 0xFF732A);
	//g_pattern->HandleBullet
	if (int status = MH_CreateHook(g_pattern->HandleBullet, mHandleBullet, (void**)&hk_originalHandleBullet) != MH_OK || MH_EnableHook(g_pattern->HandleBullet) != MH_OK)
	{
		Log::Msg("hk fail %llx %llx", g_pattern->HandleBullet, mHandleBullet);
	}else
		Log::Msg("hk ok");

}
*/
Pattern::write_player_game_state_data_node Orin_write_player_game_state_data_node = nullptr;
void HK_write_player_game_state_data_node(uintptr_t* player, CPlayerGameStateDataNode* node)
{
	//Log::Msg("before call original m_is_invincible: %d", node->m_is_invincible);
	bool Successed = Orin_write_player_game_state_data_node(player, node);
	//Log::Msg("call oringinal write note :player: %llx|| nodes: %llx || Successed: %d", player, node, Successed);
	if (1)//setting spoof data nodes
	{
		//Log::Msg("original m_is_invincible: %d", node->m_is_invincible);
		//Log::Msg("original m_player_team: %d", node->m_player_team);
		//Log::Msg("original m_max_health: %d", node->m_max_health);
		//Log::Msg("original m_max_armor: %d", node->m_max_armor);
		//Log::Msg("original m_friendly_fire_allowed: %d", node->m_friendly_fire_allowed);
		//Log::Msg("original m_decor_count: %d", node->m_decor_count);
		//Log::Msg("original m_player_state: %d", node->m_player_state);
		node->m_never_target = true;
		node->m_player_is_targetable_by_team = false;
		node->m_is_invincible = false;
		node->m_bullet_proof = false;
		node->m_collision_proof = false;
		node->m_explosion_proof = false;
		node->m_fire_proof = false;
		node->m_melee_proof = false;
		node->m_steam_proof = false;
		node->m_water_proof = false;
		node->m_player_team = 10;
	}

	if (1)//setting hide spectate
	{
		node->m_is_spectating = false;
		node->m_spectating_net_id = 0;
	}
}

bool HookDataNodes()
{
	MH_Initialize();
	if (MH_CreateHook(g_pattern->Target_write_player_game_state_data_node, HK_write_player_game_state_data_node, (void**)&Orin_write_player_game_state_data_node) != MH_OK
		|| MH_EnableHook(g_pattern->Target_write_player_game_state_data_node) != MH_OK)
	{
		return false;
	}
	return true;
}

bool IsReady() {
	if (GetModuleHandleA("d3d11.dll") != NULL && GetModuleHandleA(Encrypt("socialclub.dll")) != NULL)// && GetModuleHandleA(Encrypt("adhesive.dll")) != NULL)
		return true;
	else
		return false;
}
/*
auto oReturn = g_Memory->find_pattern(Encrypt("medal-hook64.dll"), Encrypt("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 05"), 0, 0);
Log::Msg("medal-hook64 %llx", oReturn);
*/
#define ACCES "ACCESS_VALID"
static bool inited = false;
void Hook_Thread()
{
	xxx();
	name.clear(); ownerid.clear(); secret.clear(); version.clear(); url.clear(); sslPin.clear();
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	p_variables->misc.TESTER = true;
	//KeyAuthApp.data.ip = "true";
	if (p_variables->misc.TESTER)
	{
		while (!IsReady()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		MH_Initialize();
		HookDataNodes();
	}

	while (!inited)//wait for game vtab and 3ddevice created and hook it asap
	{
		swapchain_vtables = *(DWORD_PTR**)(g_pattern->g_swapchain);
		if (swapchain_vtables)
		{
			swapchain_vtables = (DWORD_PTR*)(swapchain_vtables[0]);
			swapchain_backup = swapchain_vtables[8];//present vfunction
			SAFE_CALL(VirtualProtect)((LPVOID)swapchain_vtables, sizeof(DWORD_PTR), (DWORD)PAGE_EXECUTE_READWRITE, (PDWORD)&dwOld);//unprotect
			oPresent = reinterpret_cast<OriginalFnPresent>(swapchain_vtables[8]);//save original func pointer to use later
			swapchain_vtables[8] = (uintptr_t)&Present;//cast our function to their present vfunc pointer
			SAFE_CALL(VirtualProtect)((LPVOID)swapchain_vtables, sizeof(DWORD_PTR), (DWORD)dwOld, (PDWORD)&dwOld);//restore memory protection
			KeyAuthApp.init();
			inited = true;//all done
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
	}



	
	// /////////////
	//Log::Msg("hook inited");
	//hookBullet();
	//Log::Msg("%llx", mHandleBullet);
	//kaka.init();

	//Pattern::HandleBullet_t HandleBulletcall = (Pattern::HandleBullet_t)((uintptr_t)GetModuleHandleA(0) + 0xFF9D24);
	
	//medaltv hook medal-hook64.dll+8BA8 | "medal-hook64.dll"+8340
	//48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ? 0F B6 05

	
	while (0)//!init some test 
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

}


BOOL APIENTRY DllMain(HMODULE hmod, DWORD dwreason, LPVOID lpreversed)
{
	dwreason;
	lpreversed;
	if (dwreason == DLL_PROCESS_ATTACH)
	{
		Log::Init(hmod);//init log module
		SAFE_CALL(_beginthreadex)(0, 0, (_beginthreadex_proc_type)Hook_Thread, 0, 0, 0);//create thread for d3d hook
		auto tickCount = SAFE_CALL(GetTickCount)();//init timer
		invokerInitPattern();//scan fastpathmap and gmappingtable for invoker
		g_offsets->Initialize();//scan build number and apply offset for each build
		g_pattern->Initialize();//scan basic patterns
		
		//NativeInvoke::initiating();
		g_tls_hook->Initialize();//Script thread hook
		tickCount = SAFE_CALL(GetTickCount)() - tickCount;//count init time
		Log::Msg(Encrypt("Patt time: %d"), tickCount);
		cfg_manager->Load(Encrypt("default.ini").decrypt());//load default config
		locationManager->loadLocations(Encrypt("C:\\RealisticT\\cfg\\locations.json"));
	}
	if (dwreason == DLL_PROCESS_DETACH)
	{
		swapchain_vtables = *(DWORD_PTR**)(g_pattern->g_swapchain);
		swapchain_vtables[8] = swapchain_backup;
		g_tls_hook->UnHook();
		FreeLibraryAndExitThread(hmod, 0);
		
	}
	return 1;
}



/*
inline static string a_replaceAll(string subject, const string& search,
	const string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}*/
/*Download updated version
string __DownloadURL(string URL) {
	HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	HINTERNET urlFile;
	string rtn;
	if (interwebs) {
		urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
		if (urlFile) {
			char buffer[2000];
			DWORD bytesRead;
			do {
				InternetReadFile(urlFile, buffer, 2000, &bytesRead);
				rtn.append(buffer, bytesRead);
				memset(buffer, 0, 2000);
			} while (bytesRead);
			InternetCloseHandle(interwebs);
			InternetCloseHandle(urlFile);
			string p = a_replaceAll(rtn, "|n", "\r\n");
			return p;
		}
	}
	InternetCloseHandle(interwebs);
	string p = a_replaceAll(rtn, "|n", "\r\n");
	return p;
}
*/