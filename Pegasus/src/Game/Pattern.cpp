#include <Pattern.h>
#include <resources.h>

void Pattern::Initialize()
{
	this->g_swapchain = g_Memory->find_pattern(0, Encrypt("48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8"), 7, 0);
	this->g_ViewPort = g_Memory->find_pattern(0, Encrypt("48 8B 15 ?? ?? ?? ?? 48 8D 2D ?? ?? ?? ?? 48 8B CD"), 7, 0);
	this->g_worldFactory = g_Memory->find_pattern(0, Encrypt("48 8B 05 ? ? ? ? 33 D2 48 8B 40 08 8A CA 48 85 C0 74 16 48 8B"), 7, 0);//48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81
	this->g_replayInterface = g_Memory->find_pattern(0, Encrypt("48 8D 0D ?? ?? ?? ?? 48 ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 8A D8 E8 ?? ?? ?? ?? 84 DB 75 13 48 8D 0D ?? ?? ?? ?? 48 8B D7 E8 ?? ?? ?? ?? 84 C0 74 BC 8B 8F"), 7, 0);
	this->g_camViewAngles = g_Memory->find_pattern(0, Encrypt("48 8B 05 ? ? ? ? 48 8B 98 ? ? ? ? EB"), 7, 0);
	//this->pNetworkRequestControlOfEntity = (fpNetworkRequestControlOfEntity)g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ?? 57 48 83 EC 20 8B D9 E8 ?? ?? ?? ?? 84 C0"), 0, 0);
	if(!strcmp(g_offsets->builtNumber, Encrypt("2189")))
		this->HasEntityClearLosToEntityNative = (HasEntityClearLosToEntity_t)g_Memory->find_pattern(0, Encrypt("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 55 41 54 41 55 41 56 41 57 48 8D A8 E8 FE FF FF"), 0, 0); //F6 FF FF
	else
		this->HasEntityClearLosToEntityNative = (HasEntityClearLosToEntity_t)g_Memory->find_pattern(0, Encrypt("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 55 41 54 41 55 41 56 41 57 48 8D A8 E8 F6 FF FF"), 0, 0); //F6 FF FF

	this->pointer_to_handle = (pointer_handler_t)g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 8B 15 ? ? ? ? 48 8B F9 48 83 C1 10 33 DB"), 0, 0);
	this->handle_to_pointer = (handle_to_pointer_t)g_Memory->find_pattern(0, Encrypt("83 F9 FF 74 31 4C 8B 0D"), 0, 0);
	//this->g_world2screen = g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ?? 55 56 57 48 83 EC 70 65 4C 8B 0C 25"), 0, 0);
	//this->World_To_Screen = reinterpret_cast<World_To_Screen_t>(g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ?? 55 56 57 48 83 EC 70 65 4C 8B 0C 25"), 0, 0));
	//tGet_BONE_POS_func = reinterpret_cast<tGet_BONE_POS>(g_Memory->find_pattern(0,Encrypt("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 60 48 8B 01 41 8B E8 48 8B F2 48 8B F9 33 DB"), NULL, NULL));
	//this->g_bonemask = g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 60 48 8B 01 41 8B E8 48 8B F2 48 8B F9 33 DB"), 0, 0);
	//this->ClearPedtask = (ClearPedTask_t)g_Memory->find_pattern(0, _xor_("40 53 48 83 EC 30 E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 0F 84").c_str(), 0, 0);
	//this->oSet_Ped_ammo = (oSet_Ped_Ammo_t)g_Memory->find_pattern(0, _xor_("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC 20 41 8B F0 8B FA E8 ?? ?? ?? ?? 48 8B D8").c_str(), 0, 0);
	//this->GET_ENTITY_BONE_INDEX_BY_NAME = (GET_ENTITY_BONE_INDEX_BY_NAME_t)g_Memory->find_pattern(0, _xor_("48 89 5C 24 10 48 89 74 24 18 57 48 83 EC ? 48 8B 01 83 4C 24 30 ?").c_str(), 0, 0);
	//this->GET_WORLD_POSITION_OF_ENTITY_BONE = (GET_WORLD_POSITION_OF_ENTITY_BONE_t)g_Memory->find_pattern(0, _xor_("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 4C 8B 01 0F 29 74 24 20").c_str(), 0, 0);
	//give_weapon_to_ped = (give_weapon_to_ped_t)g_Memory->find_pattern(0, _xor_("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 41 8A E9 41 8B F0 8B FA E8 ? ? ? ?").c_str(), 0, 0);
	//this->no_reload = g_Memory->find_pattern(0, _xor_("41 2B C9 3B C8 0F").c_str(), 0, 0);
	DWORD64 WaypointPatt = g_Memory->find_pattern(0, _xor_("74 1F F3 0F 10 05 ?? ?? ?? ?? F3 0F 11 03").c_str(), 0, 0);
	this->WayPointRead = WaypointPatt + *(DWORD*)(WaypointPatt + 6) + 0xA;
	this->repair_vehicle = (repair_vehicle_t)g_Memory->find_pattern(0, _xor_("40 53 48 83 EC 20 E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 74 28 4C 8B 10").c_str(), 0, 0);
	this->oSet_Plate = (oSet_Plate_t)g_Memory->find_pattern(0, Encrypt("40 53 48 83 EC 20 48 8B DA E8 ?? ?? ?? ?? 48 85 C0 74 10 48 8B 48 48"), 0, 0);
	//this->SetVehicleForwardSpeed = (SetVehicleForwardSpeed_t)g_Memory->find_pattern(0, Encrypt("40 53 48 83 EC 40 0F 29 74 24 ? 48 8B D9 0F 28 F1 E8 ? ? ? ? 48 8B 03"), 0, 0);
	//ClearPedBlood = (ClearPedBlood_t)g_Memory->find_pattern(0, Encrypt("40 53 48 83 EC 20 8A 91 ? ? ? ? 48 8B D9 80 FA FF 74 51"), 0, 0); 
	//FreezeEntity = (FreezeEntity_t)g_Memory->find_pattern(0, Encrypt("48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 40 8A F2 E8 ? ? ? ? 48 8B D8 48 85 C0 74 69"), 0, 0); 
	//is_ped_in_any_vehicle = (fpIsPedInAnyVehicle)g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 40 8A FA E8 ? ? ? ? 32 DB"), 0, 0); 
	network_set_in_spectator_mode = (fpNetworkSetInSpectatorMode)g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ? 57 48 83 EC 20 41 8A F8 84 C9"), 0, 0);
	//add_explosion = (fpAddExplosion)g_Memory->find_pattern(0, Encrypt("E9 ? ? ? ? 8B 85 ? ? ? ? A8 40 48 8D 64 24 ? 48 89 2C 24 48 BD ? ? ? ? ? ? ? ? 48 87 2C 24 48 89 4C 24"), 0, 0); 
	//set_entity_coords_no_offset = (fpSetEntityCoordsNoOffset)g_Memory->find_pattern(0, Encrypt("48 8B C4 48 89 58 08 48 89 70 10 57 48 81 EC ? ? ? ? F3 0F 10 15"), 0, 0); 
	//set_player_model = (fpSetPlayerModel)g_Memory->find_pattern(0, Encrypt("40 55 53 57 48 8B EC 48 83 EC 30 8B F9"), 0, 0); 
	//this->create_vehicle = (fpCreateVehicle)g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 50 F3 0F 10 02"), 0, 0);
	g_isScriptNetworked = g_Memory->find_pattern(0, Encrypt("48 8B C8 FF 52 30 84 C0 74 05 48"), 0, 0) + 0x06;
	//this->TASK_SHOOT_AT_ENTITY = (TASK_SHOOT_AT_ENTITY_t)g_Memory->find_pattern(0, _xor_("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 20 57 41 56 41 57 48 83 EC ? 33 FF 41 8B F1").c_str(), 0, 0);
	//this->TASK_SHOOT_AT_COORD = (TASK_SHOOT_AT_COORD_t)g_Memory->find_pattern(0, _xor_("48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? 00 00 00 33 FF 45 8B F9 41 8B F0 44 8B F1").c_str(), 0, 0);
	//this->shoot_single_bullet_between_coords = (fpShootSingleBulletBetweenCoords)g_Memory->find_pattern(0, _xor_("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 55 48 8D ? ? 48 81 EC ? ? ? ? F3 0F 10 01 F3 0F 10 49 ? F3 0F 10 51").c_str(), 0, 0);
	//this->HandleBullet = (HandleBullet_t)(g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ? 57 48 83 EC 30 41 8B 00"), 0, 0));
	//this->GetPlayerNameFromScAddr = (decltype(this->GetPlayerNameFromScAddr))g_Memory->find_pattern(Encrypt("citizen-playernames-five.dll"), Encrypt("40 57 48 83 EC ? 48 8B 05"), 0, 0);
	this->GetPlayerNameFromScAddr = (decltype(this->GetPlayerNameFromScAddr))g_Memory->find_pattern(0, Encrypt("E8 ? ? ? ? 48 8D 94 24 ? ? ? ? 49 8D 4E 68"), 5, 0);
	this->BlipLists = g_Memory->find_pattern(0, Encrypt("4C 8D 05 ? ? ? ? 0F B7 C1"), 7, 0);
	g_offsets->GetPlayerNameFromScAddr = this->GetPlayerNameFromScAddr;
	auto tickFunc = g_Memory->find_pattern(0, Encrypt("80 B9 ? 01 00 00 00 8B FA 48 8B D9 74 05"), 0, 0) - 0xF;
	dwTlsOffset = *(uint32_t*)(g_Memory->find_pattern(0, Encrypt("48 8B 04 D0 4A 8B 14 00 48 8B 01 F3 44 0F 2C 42 20"), 0, 0) - 4);
	tickFuncPtr = g_Memory->resolvePtr(0, tickFunc, 0xE9);
	this->Target_write_player_game_state_data_node = (write_player_game_state_data_node)g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 30 0F B7 81"), 0, 0);
	//this->oGameThread = (oGameThread_t)g_Memory->find_pattern(0, Encrypt("48 89 5C 24 ?? 57 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 8B 83 ?? ?? ?? ??"), 0, 0);
	//dwThreadCollectionPtr = g_Memory->find_pattern(0, Encrypt("48 8B ? ? ? ? ? 8B CA 4C 8B 0C C8 45"), 7, 0);
	//dwThreadCollectionPtr = g_Memory->find_pattern(0, Encrypt("63 6F 6D 6D 6F 6E 3A 2F 64 61 74 61 2F 73 63 72 69 70 74 4D 65 74 61 64 61 74 61"), 0, 0);
	//Log::Msg("dwThreadCollectionPtr = 0x%llx", dwThreadCollectionPtr);
	//Log::Msg("tickFuncPtr = 0x%llx", tickFuncPtr);


	const uint64_t cscModule = (uint64_t)GetModuleHandleA(Encrypt("citizen-resources-core.dll"));
	g_allResources = (uintptr_t)GetProcAddress((HMODULE)cscModule, "?OnInitializeInstance@ResourceManager@fx@@2V?$fwEvent@PEAVResourceManager@fx@@@@A") + 0x20;
}

bool Pattern::HasEntityClearLosToEntity(class CPed* Ped1, class CPed* Ped2, uint32_t flags) {
	return this->HasEntityClearLosToEntityNative(Ped1, Ped2, 4, flags, false);
}

DWORD64 Pattern::GetCameraViewAngles()
{
	if (g_camViewAngles)
		return *(DWORD64*)(g_camViewAngles + 0x0);
}

uintptr_t Pattern::GetPedHandle()
{
	auto* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
	return this->pointer_to_handle(g_pedFactory->m_local_ped());
}