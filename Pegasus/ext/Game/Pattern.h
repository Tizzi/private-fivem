#pragma once
#include <Includes.h>
#include <CPlayerGameStateDataNode.hpp>
#include <Memory.h>




class Pattern
{
public:
	const char* szModuleName = "";
	uintptr_t m_BaseAddy = (uintptr_t)GetModuleHandleA(0);

	typedef BOOL(__fastcall* write_player_game_state_data_node)(uintptr_t* PlayerNetObject, CPlayerGameStateDataNode* node);
	write_player_game_state_data_node Target_write_player_game_state_data_node = nullptr;
	uintptr_t g_ViewPort, g_worldFactory, g_gravFly, g_swapchain, g_replayInterface, g_camViewAngles, g_world2screen, g_bonemask;
	uintptr_t BlipLists = 0;

	using pointer_handler_t = int32_t(*)(class CPed* cPed);
	pointer_handler_t pointer_to_handle = nullptr;

	typedef DWORD64(__fastcall* oGameThread_t)(DWORD64 a1);
	oGameThread_t oGameThread = NULL;

	DWORD64 dwThreadCollectionPtr = 0;
	DWORD64 tickFuncPtr = 0;
	DWORD64 g_allResources = 0;
	UINT32 dwTlsOffset = 0x848;

	using handle_to_pointer_t = int64_t(*)(DWORD handle);
	handle_to_pointer_t handle_to_pointer = nullptr;

	typedef __int64(__fastcall* HandleBullet_t)(intptr_t BulletPtr, intptr_t CWeaponInfo, Vector3 StartPos, Vector3 EndPos, float Range, int weaponHash, char a7, char a8);
	HandleBullet_t HandleBullet = nullptr;

	typedef void(__fastcall* tGet_BONE_POS)(uint64_t, __m128*, int32_t);
	tGet_BONE_POS tGet_BONE_POS_func = nullptr;

	using give_weapon_to_ped_t = void(*)(int32_t ped, uintptr_t hash, int ammo, bool equip_now);
	give_weapon_to_ped_t give_weapon_to_ped = nullptr;

	typedef BOOL(__fastcall* fpNetworkRequestControlOfEntity)(DWORD entity);
	fpNetworkRequestControlOfEntity pNetworkRequestControlOfEntity = nullptr;

	typedef __int64(__fastcall* World_To_Screen_t)(const Vector3* vPos, const float* x, const float* y);
	World_To_Screen_t World_To_Screen = nullptr;

	typedef __int64(__fastcall* repair_vehicle_t)(DWORD vehicle);
	repair_vehicle_t repair_vehicle = nullptr;


	//typedef DWORD64(__fastcall* fpShootSingleBulletBetweenCoords)(PVector3 pos1, PVector3 pos2, int damage, BOOL unk0, DWORD hash, DWORD owner, BOOL isAudible, BOOL isVisible, float speed, DWORD IgnoreEntity, BOOL unk1, BOOL unk2, BOOL unk3, BOOL unk4);
	//fpShootSingleBulletBetweenCoords shoot_bullets = NULL;

	using SetCursorMode = void(*)(bool onoff);
	SetCursorMode cursormode = nullptr;

	using print_console_t = void(*)(std::string resource, std::string message);
	print_console_t print_console = nullptr;

	typedef void(__fastcall* ClearPedTask_t)(DWORD Ped);
	ClearPedTask_t ClearPedtask = nullptr;

	const char* (__fastcall* GetPlayerNameFromScAddr)(void*);

	typedef __m128(__fastcall* GET_ENTITY_BONE_INDEX_BY_NAME_t)(CPed* cPed, const char* bone_name);
	GET_ENTITY_BONE_INDEX_BY_NAME_t GET_ENTITY_BONE_INDEX_BY_NAME = nullptr;

	typedef Vector3(__fastcall* GET_WORLD_POSITION_OF_ENTITY_BONE_t)(CPed* cPed, unsigned int Bone_Mask);
	GET_WORLD_POSITION_OF_ENTITY_BONE_t GET_WORLD_POSITION_OF_ENTITY_BONE = nullptr;

	typedef void(__fastcall* oSet_Plate_t)(DWORD cVehicle, const char* plate_text);
	oSet_Plate_t oSet_Plate = nullptr;

	typedef void(__fastcall* oSet_Ped_Ammo_t)(DWORD cPed, float ammo);
	oSet_Ped_Ammo_t oSet_Ped_ammo = nullptr;

	typedef void(__fastcall* TASK_PLAY_ANIM_t)(DWORD ped, const char* animDictionary, const char* animationName, float blendInSpeed, float blendOutSpeed, int duration, int flag, float playbackRate, BOOL lockX, BOOL lockY, BOOL lockZ);
	TASK_PLAY_ANIM_t Task_Play_anim = nullptr;

	typedef void(__fastcall* TASK_SHOOT_AT_ENTITY_t)(int ped, class CPed* target, int duration, DWORD firingpattern);
	TASK_SHOOT_AT_ENTITY_t TASK_SHOOT_AT_ENTITY = nullptr;

	typedef void(__fastcall* TASK_SHOOT_AT_COORD_t)(class CPed* ped, float x, float y, float z, int duration, DWORD firingpattern);
	TASK_SHOOT_AT_COORD_t TASK_SHOOT_AT_COORD = nullptr;

	typedef __int64(__fastcall* TASK_WARP_PED_INTO_VEHICLE_t)(class CPed* cPed, class CVehicleManager* cVehicleManager, int seat_index);
	TASK_WARP_PED_INTO_VEHICLE_t TASK_WARP_PED_INTO_VEHICLE = nullptr;

	typedef char(__fastcall* HasEntityClearLosToEntity_t)(class CPed* Ped1, class CPed* Ped2, char a3, int flags, char a5);
	HasEntityClearLosToEntity_t HasEntityClearLosToEntityNative = nullptr;

	typedef DWORD64(__fastcall* fpSetPedIntoVehicle)(DWORD ped, DWORD vehicle, int seat);
	fpSetPedIntoVehicle pSetPedIntoVehicle = nullptr;

	typedef DWORD64(__fastcall* SetVehicleForwardSpeed_t)(DWORD vehicle, float Speed);
	SetVehicleForwardSpeed_t SetVehicleForwardSpeed;

	typedef DWORD64(__fastcall* ClearPedBlood_t)(uintptr_t ped);
	ClearPedBlood_t ClearPedBlood = nullptr;

	typedef DWORD64(__fastcall* FreezeEntity_t)(uintptr_t ped, bool a1);
	FreezeEntity_t FreezeEntity = nullptr;

	typedef bool(__cdecl* fpIsPedInAnyVehicle)(uintptr_t ped, BOOL atGetIn);
	fpIsPedInAnyVehicle is_ped_in_any_vehicle = nullptr;

	//48 89 5C 24 ? 57 48 83 EC 20 41 8A F8 84 C9
	typedef BOOL(__cdecl* fpNetworkSetInSpectatorMode)(bool toggle, uintptr_t ped);
	fpNetworkSetInSpectatorMode network_set_in_spectator_mode = nullptr;

	//E9 ? ? ? ? 8B 85 ? ? ? ? A8 40 48 8D 64 24 ? 48 89 2C 24 48 BD ? ? ? ? ? ? ? ? 48 87 2C 24 48 89 4C 24
	typedef void(__cdecl* fpAddExplosion)(Vector3 pos, int type, float damage, bool audible, bool invis, float shake, bool nodmg);
	fpAddExplosion add_explosion = nullptr;
	//Native_Invoker
	uintptr_t rage_native_table;

	typedef void(__cdecl* fpSetEntityCoordsNoOffset)(CPed* entity, Vector3 pos, BOOL x, BOOL y, BOOL z);
	fpSetEntityCoordsNoOffset set_entity_coords_no_offset = nullptr;

	uintptr_t rage_handles;

	//SetPedModel = 40 55 53 57 48 8B EC 48 83 EC 30 8B F9
	typedef int(__cdecl* fpSetPlayerModel)(DWORD player, DWORD model);
	fpSetPlayerModel set_player_model = nullptr;

	typedef bool(__cdecl* fpGetPedLastWeaponImpactCoord)(DWORD ped, Vector3 out);
	fpGetPedLastWeaponImpactCoord get_ped_last_weapon_impact = nullptr;

	//48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 50 F3 0F 10 02
	typedef int(__cdecl* fpCreateVehicle)(DWORD model, Vector3* pos, float heading, BOOL networked, BOOL unk2);
	fpCreateVehicle create_vehicle = nullptr;
	DWORD64 g_isScriptNetworked;

	typedef BOOL(__cdecl* fpShootSingleBulletBetweenCoords)(Vector3 pos1, Vector3 pos2, int damage, bool unk0, DWORD hash, DWORD owner, bool isAudible, bool isVisible, float speed, int unk1, bool unk2, bool unk3, int unk4);
	fpShootSingleBulletBetweenCoords shoot_single_bullet_between_coords = nullptr;

	//48 89 5C 24 ? 48 89 7C 24 ? 55 48 8B EC 48 83 EC 50 8B 45 18
	typedef BOOL(__cdecl* fpRequestModel)(DWORD hash);
	fpRequestModel request_model = nullptr;

	//48 89 7C 24 ? 55 48 8B EC 48 83 EC 20 8B 45 18 BF
	typedef bool(__cdecl* fpHasModelLoaded)(DWORD hash);
	fpHasModelLoaded has_model_loaded = nullptr;

	typedef BOOL(__cdecl* fpSetModelAsNoLongerNeeded)(DWORD hash);
	fpSetModelAsNoLongerNeeded set_model_as_no_longer_needed = nullptr;

	typedef BOOL(__cdecl* fpCreateObject)(DWORD model, float x, float y, float z, bool offset, int unk0, int unk1, Vector3 out, bool isNetworked, bool unk2, bool dynamic);	//unk0 = 1; unk1 = -1
	fpCreateObject create_object = nullptr;

	uintptr_t no_reload;
	DWORD64 FrameCount = 0x0;

	using g_Disconnect_t = void(*)(const char* reason);
	g_Disconnect_t g_Disconnect = nullptr;

	typedef std::string(__fastcall* GetPath_a)(__int64 _this);
	typedef int(__fastcall* GetState_a)(__int64 _this);
	typedef char(__fastcall* StopResource_a)(__int64 _this);
	typedef char(__fastcall* StartResource_a)(__int64 _this);
	typedef BOOL(__cdecl* fpRequestAnimDict)(char* dict);
	fpRequestAnimDict request_anim_dict = nullptr;
	typedef bool(__cdecl* fpHasAnimDictLoaded)(char* dict);
	fpHasAnimDictLoaded has_anim_dict_loaded = nullptr;
	typedef BOOL(__cdecl* fpAiTaskPlayAnim)(DWORD ped, char* dict, char* anim, float speed, float speedMult, int duration, int flg, float playRate, bool unk0, BOOL lockX, BOOL lockY, BOOL lockZ, bool unk1);
	fpAiTaskPlayAnim ai_task_play_anim = nullptr;
	DWORD64 WayPointRead;
	Vector2 WayPointPos = Vector2(0, 0);

	bool HasEntityClearLosToEntity(class CPed* Ped1, class CPed* Ped2, uint32_t flags);

	const char* GET_PLAYER_NAME(__int64 num);

	DWORD64 GetCameraViewAngles();

	void Initialize();

	uintptr_t GetPedHandle();

};

inline Pattern* g_pattern = new Pattern;