#pragma once
#include <Includes.h>
#include <Log.h>
// Macros
#define ARRAY_LENGTH(array)	(sizeof(array) / sizeof(array[0]))
#define	SAFE_DELETE(memory)	{ delete memory; memory = NULL; }
#define	SAFE_RELEASE(p)	{ if ( (p) ) { (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(memory) if(memory) { delete [] memory; memory = NULL; }
#define SAFE_FREE(memory) if(memory) { free(memory); memory = NULL; }


class offsets_game
{
public:
	uintptr_t m_recoil, m_run_speed, freeze, m_player_id, m_vehicle_handling, m_gravity_vehicle, m_frame_flags, m_player_info, m_enttype, m_weapon_manager, m_armor, m_reload, m_spread, m_vehicle_manager, m_veh_lock_state, Range;
	bool isb2802 = false; char builtNumber[5] = "";
	const char* (__fastcall* GetPlayerNameFromScAddr)(void*);
	void Initialize()
	{
		char pName[MAX_PATH];
		
		if (SAFE_CALL(GetModuleFileNameA)(0, pName, MAX_PATH) != 0) {
			size_t slash = -1;

			for (size_t i = 0; i < strlen(pName); i++) {
				if (pName[i] == '/' || pName[i] == '\\') {
					slash = i;
				}
			}
			builtNumber[0] = pName[slash+8];
			builtNumber[1] = pName[slash+9];
			builtNumber[2] = pName[slash+10];
			builtNumber[3] = pName[slash+11];
			builtNumber[4] = '\0';
			//Log::Msg(pName);
			
		}
		if (!strcmp(builtNumber, Encrypt("2802")) || !strcmp(builtNumber, Encrypt("2944")))
		{
			this->m_armor = 0x150C;
			this->m_enttype = 0x1098;
			this->m_player_id = 0x88; 
			freeze = 0x250; 
			this->m_recoil = 0x02F4;// 0x02E4
			this->m_gravity_vehicle = 0x0C3C; 
			this->m_reload = 0x134;
			this->m_weapon_manager = 0x10B8;
			this->m_player_info = 0x10A8;
			this->m_spread = 0x0074;
			this->m_vehicle_manager = 0x0D10;
			this->m_frame_flags = 0x0218;
			this->m_run_speed = 0x0CF0;
			this->m_vehicle_handling = 0x0918;
			this->m_veh_lock_state = 0x1370;
			this->Range = 0x28C;
			isb2802 = true;
			Log::Msg(builtNumber);
		}
		if (!strcmp(builtNumber, Encrypt("TAPr")))
		{
			this->m_armor = 0x14B0;
			this->m_enttype = 0x10A8;
			freeze = 0x218;
			this->m_player_id = 0x68; 
			this->m_recoil = 0x2E8;
 			this->m_reload = 0x12C;
			this->m_weapon_manager = 0x10C8;
			this->m_player_info = 0x10B8;
			this->m_spread = 0x74;
			this->m_vehicle_manager = 0x0D28;
			this->m_frame_flags = 0x01F8;
			this->m_run_speed = 0x14C;
			this->m_vehicle_handling = 0x0918;
			this->m_gravity_vehicle = 0xB7C;
			this->m_veh_lock_state = 0x1390;
			this->Range = 0x25C;
			Log::Msg(builtNumber);
		}
		if (!strcmp(builtNumber, Encrypt("2060")))
		{
			this->m_vehicle_manager = 0xD30;
			this->m_enttype = 0x10B8;
			freeze = 0x218; 
			this->m_player_id = 0x88; 
			this->m_armor = 0x14E0;
			this->m_weapon_manager = 0x10D8;
			this->m_player_info = 0x10B8;
 			this->m_frame_flags = 0x0218;
			this->m_recoil = 0x2F4;
			this->m_spread = 0x84;
			this->m_reload = 0x134;
			this->m_run_speed = 0x0CF0;
			this->m_vehicle_handling = 0x938;
			this->m_veh_lock_state = 0x1390;
			this->Range = 0x28C;
			//this->WeaponName = 0x5F0;
			Log::Msg(builtNumber);
 		}
		if (!strcmp(builtNumber, Encrypt("2545")))
		{
			this->m_vehicle_manager = 0xD30;
			this->m_enttype = 0x10B8;
			freeze = 0x238;
			this->m_armor = 0x14E0 + 0x50;
			this->m_frame_flags = 0x0218;
			this->m_weapon_manager = 0x10D8;
			this->m_player_id = 0x88; 
 			this->m_run_speed = 0x0CF0;
			this->m_player_info = 0x10C8;
			this->m_recoil = 0x2F4;
			this->m_spread = 0x84;
			this->m_reload = 0x134;
			this->m_vehicle_handling = 0x938;
			this->m_veh_lock_state = 0x1390;
			this->Range = 0x28C;
			Log::Msg(builtNumber);
		}
		if (!strcmp(builtNumber, Encrypt("2612")))
		{
			this->m_vehicle_manager = 0xD30;
			this->m_enttype = 0x10B8;
			this->m_player_id = 0x88;
			this->m_armor = 0x1530;
			this->m_frame_flags = 0x0218;
			this->m_weapon_manager = 0x10D8;
			this->m_player_info = 0x10C8;
			freeze = 0x238;
			this->m_recoil = 0x2F4;
			this->m_run_speed = 0x0CF0;
			this->m_spread = 0x84;
			this->m_reload = 0x134;
			this->m_vehicle_handling = 0x938;
			this->m_veh_lock_state = 0x1390;
			this->Range = 0x28C;
			Log::Msg(builtNumber);
 		}
		if (!strcmp(builtNumber, Encrypt("2699")))
		{
			this->m_armor = 0x1530;
			this->m_enttype = 0x10B8;
			this->m_recoil = 0x2F4;
			this->m_gravity_vehicle = 0x0C3C;
			freeze = 0x238;
			this->m_player_id = 0x88;
			this->m_reload = 0x134;
			this->m_weapon_manager = 0x10D8;
			this->m_player_info = 0x10C8;
			this->m_spread = 0x84;
			this->m_vehicle_manager = 0xD30;
			this->m_frame_flags = 0x0218;
			this->m_run_speed = 0x0CF0;
			this->m_gravity_vehicle = 0x0C5C;
			this->m_vehicle_handling = 0x938;
			this->m_veh_lock_state = 0x1390;
			this->Range = 0x28C;
			Log::Msg(builtNumber);
 		}
		if (!strcmp(builtNumber, Encrypt("2189")))
		{
			this->m_vehicle_manager = 0xD30;
			this->m_player_id = 0x68;
			this->m_enttype = 0x10B8;
			this->m_armor = 0x14E0;
			this->m_run_speed = 0x0CF0;
			this->m_weapon_manager = 0x10D8;
			freeze = 0x218;
			this->m_player_info = 0x10C8;
			this->m_frame_flags = 0x0218;
			this->m_recoil = 0x2F4;
			this->m_spread = 0x84;
			this->m_reload = 0x134;
			this->m_vehicle_handling = 0x938;
			this->m_veh_lock_state = 0x1390;
			this->Range = 0x28C;
			Log::Msg(builtNumber);
		}
		if (!strcmp(builtNumber, Encrypt("2372")))
		{
			this->m_vehicle_manager = 0xD30;
			this->m_enttype = 0x10B8;
			this->m_armor = 0x14E0;
			this->m_run_speed = 0x0CF0;
			this->m_player_id = 0x88;
			this->m_weapon_manager = 0x10D8;
			freeze = 0x218;
			this->m_player_info = 0x10C8;
			this->m_recoil = 0x2F4;
			this->m_frame_flags = 0x0218;
			this->m_spread = 0x84;
			this->m_reload = 0x134;
			this->m_vehicle_handling = 0x938;
			this->m_veh_lock_state = 0x1390;
			this->Range = 0x28C;
			Log::Msg(builtNumber);
		}
	}
};

inline offsets_game* g_offsets = new offsets_game;

 

enum eAnimationFlags
{
	ANIM_FLAG_NORMAL = 0,
	ANIM_FLAG_REPEAT = 1,
	ANIM_FLAG_STOP_LAST_FRAME = 2,
	ANIM_FLAG_UPPERBODY = 16,
	ANIM_FLAG_ENABLE_PLAYER_CONTROL = 32,
	ANIM_FLAG_CANCELABLE = 120,
};

 
enum ePedBoneType
{
	HEAD,
	L_FOOT,
	R_FOOT,
	L_ANKLE,
	R_ANKLE,
	L_HAND,
	R_HAND,
	NECK,
	ABDOMEN
};
#define SKEL_LFoot 2
#define SKEL_RFoot 1
#define SKEL_PELVIS 6
#define SKEL_HEAD 5
#define SKEL_NECK 5
#define SKEL_RHand 3
#define SKEL_LANKLES 23
#define SKEL_LHand 4
#define SKEL_SPINE 30

/*#define SKEL_LFoot 2
#define SKEL_RFoot 1
#define SKEL_PELVIS 6
#define SKEL_HEAD 5
#define SKEL_NECK 5
#define SKEL_RHand 3
#define SKEL_LANKLES 23
#define SKEL_LHand 4
#define SKEL_SPINE 30*/

#define SKEL_Pelvis 0x2e28
#define SKEL_L_Foot 0x3779
#define SKEL_R_Foot 0xcc4d
#define SKEL_Spine_Root 0xe0fd
#define SKEL_R_Hand 0xdead
#define SKEL_Neck_1 0x9995
#define SKEL_Head 0x796e
#define SKEL_L_Hand 0x49d9
struct Net
{
public:
	char IP[99]; 
};

//inline Net* net_ip = (Net*)*(uintptr_t*)(GetModuleHandleA("net.dll") + 0x168e0);

struct ResourceImpl_struct {
	char pad_0001[0xC0];
	std::string m_name;
};

enum class ResourceState
{
	Uninitialized,
	Stopped,
	Starting,
	Started,
	Stopping
};

enum FRAME_FLAGS {
	EXPLO_AMMO = (1 << 11),
	FIRE_AMMO = (1 << 12),
	EXPLO_MEELE = (1 << 13),
	SUEPER_JUMP = (1 << 14)
};

class CObjectNavigation
{
public:
	Vector3 m_set_visual_pos(Vector3 end_pos)
	{
		if (!this) return Vector3(0, 0, 0);
		return *(Vector3*)(this + 0x50) = end_pos;
	}
	Vector3 m_get_visual_pos()
	{
		if (!this) return Vector3(0, 0, 0);
		return *(Vector3*)(this + 0x50);
	}
	Vector4 m_rot(Vector4 rot)
	{
		if (!this) return Vector4(0, 0, 0, 0);
		return *(Vector4*)(this + 0x30) = rot; 
	}
	Vector3 m_get_rot()
	{
		if (!this) return { 0, 0, 0 };
		return *(Vector3*)(this + 0x30); 
	}
	Vector3 m_rot_vec3(Vector3 rot)
	{
		if (!this) return { 45, 45, 45 }; 
		return *(Vector3*)(this + 0x30) = rot; 
	}
	Vector3 m_heading(Vector3 heading)
	{
		if (!this) return { 0, 0, 0 };
		return *(Vector3*)(this + 0x70) = heading;
	}
	float fHeading()
	{
		if (!this) return 0;
		return *(float*)(this + 0x0020); 
	} //0x0020 
	float fHeading2()
	{
		if (!this) return 0;
		return *(float*)(this + 0x0024);
	} //0x0024 
 
	float set_heading(float heading)
	{
		if (!this) return 0;
		return *(float*)(this + 0x20) = heading; 
	}
};

class VTable_cPed
{
public: 
	float fHeading()
	{
		return 0; 
	}
};

class  cFixedAmmoCount
{
public:
	float SetAmmo(float caca)
	{
		if (!this) return 0;
		return *(uint32_t*)(this + 0x18) = caca;
	}


};

class cAmmoCount
{
public:

	cFixedAmmoCount* FixedAmmoCount()
	{
		if (!this) return 0;
		return (cFixedAmmoCount*)(*(uint64_t*)(this + 0x0));
	}
};
class cAmmoInfo
{
public:
	cAmmoCount* AmmoCount()
	{
		if (!this) return 0;
		return (cAmmoCount*)(*(uint64_t*)(this + 0x8));
	}
	float SetSpeedBullet(float value)
	{
		if (!this) return 0;
		return *(float*)(this + 0x58) = value;
	}
};

class CWeaponInfo
{
public:
	uint64_t GetHash()
	{
		if (!this) return NULL;
		return *(uint64_t*)(this + 0x10);
	}

	uint64_t SetHash(DWORD caca)
	{
		if (!this) return NULL;
		return *(uint64_t*)(this + 0x10) = caca;
	}
	float m_shake_recoil(float recoil)
	{
		if (!this) return 0;
		return *(float*)(this + g_offsets->m_recoil) = recoil;
	}

	float m_spread(float spread)
	{
		if (!this) return 0;
		return *(float*)(this + g_offsets->m_spread) = spread;
	}

	float m_reload_anim(float reload)
	{
		if (!this) return 0;
		return *(float*)(this + g_offsets->m_reload) = reload;
	}
	float SetRange(float value)
	{
		if (!this) return 0;
		return *(float*)(this + g_offsets->Range) = value;
	}
	cAmmoInfo* AmmoInfo()
	{
		if (!this) return 0;
		return (cAmmoInfo*)(*(uint64_t*)(this + 0x60));
	}
	float SetSpeedBullet(float value)
	{
		if (!this) return 0;
		return *(float*)(this + 0x58) = value;
	}
	uint32_t  m_bullet_in_patch(uint32_t bullet)
	{
		if (!this) return 0;
		return *(uint32_t*)(this + 0x0120) = bullet;
	}
	char* getweaponname() {
		if (!this) return 0;
		return (*reinterpret_cast<char**>((DWORD64)this + 0x05F0));
	}
	float m_damage_get()
	{
		if (!this) return 0;
		return *(float*)(this + 0xB0); 
	}
};

class CWeaponManager
{
public:
	CWeaponInfo* m_weapon_info()
	{
		if (!this) return 0;
		return (CWeaponInfo*)*(uintptr_t*)(this + 0x20); 
	}
};
enum class eGameState : int32_t
{
	Invalid = -1,
	Playing,
	Died,
	Arrested,
	FailedMission,
	LeftGame,
	Respawn,
	InMPCutscene
};
class CPlayerInfo
{
public:
	float m_run_speed(float speed)
	{
		if (!this) return 0;
		return *(float*)(this + g_offsets->m_run_speed) = speed;
	}
	Vector3 m_heading(Vector3 heading)
	{
		if (!this) return { 0, 0, 0 };
		return *(Vector3*)(this + 0x70) = heading;
	}
	float fHeading(float head)
	{
		if (!this) return 0;
		return *(float*)(this + 0x20) = head; 
	}
	int m_player_id()
	{
		if (!this) return 0;
		return *(int*)(this + g_offsets->m_player_id); 
	}
	uint32_t m_frame_flags(uint32_t flags)
	{
		if (!this) return 0;
		return *(uint32_t*)(this + g_offsets->m_frame_flags) = flags; 
	}
	eGameState m_game_state()
	{
		if (!this) return eGameState::Invalid;
		return *(eGameState*)(this + 230);
	}
};

class CVehicleHandlingInfo
{
public:
	float m_acceleration(float acc)
	{
		if (!this) return 0;
		return *(float*)(this + 0x004C) = acc;
	}
};

class CModelInfo
{
public:
	char pad_0000[24]; //0x0000
	uint32_t Hash; //0x0018
	char pad_001C[20]; //0x001C
	Vector3 MinDim; //0x0030
	char pad_003C[4]; //0x003C
	Vector3 MaxDim; //0x0040
	char pad_004C[588]; //0x004C
	char Name[12]; //0x0298
	char Manufacture[12]; //2A4
	char pad_02A0[688]; //0x02A0

public:
	uint8_t passenger_capacity()
	{
		if (!this) return 0;
		return *(uint8_t*)(this + 0x2D8);
	}
	uint32_t vehicle_type()
	{
		if (!this) return 0;
		return *(uint32_t*)(this + 0x340);
	}
	uint8_t max_seats()
	{
		if (!this) return 0;
		return *(uint8_t*)(this + 0x34A);
	}
	uint32_t vehicle_class()
	{
		if (!this) return 0;
		return *(uint32_t*)(this + 0x548);
	}
}; //Size: 0x0550

class CVehicleManager
{
public:
	CModelInfo* modelInfo()
	{
		if (!this) return 0;
		return *reinterpret_cast<CModelInfo**>(this + 0x20);
	}
	CObjectNavigation* m_navigation_manager()
	{
		if (!this) return 0;
		return (CObjectNavigation*)*(uintptr_t*)(this + 0x30);
	}
	CPlayerInfo* m_player_info()
	{
		if (!this) return 0;
		return (CPlayerInfo*)*(uintptr_t*)(this + g_offsets->m_player_info);
	}
	VTable_cPed* m_vtables()
	{
		if (!this) return 0;
		return (VTable_cPed*)*(uintptr_t*)(this + 0x1B7C1C8); 
	}
	CVehicleHandlingInfo* m_vehicle_handling()
	{
		if (!this) return 0;
		return (CVehicleHandlingInfo*)*(uintptr_t*)(this + g_offsets->m_vehicle_handling); 
	}
	Vector3 m_set_visual_pos(Vector3 Pos)
	{
		if (!this) return Vector3(0, 0, 0);
		return *(Vector3*)(this + 0x90) = Pos;
	}
	Vector3 m_visual_pos()
	{
		if (!this) return { 0, 0, 0 }; 
		return *(Vector3*)(this + 0x90); 
	}
	float get_speed() 
	{
		if (!this) return 0;
		Vector3 m_velocity = *(Vector3*)(this + 0x07D0);
		return sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y + m_velocity.z * m_velocity.z); 
	}
	Vector3 GetVelocity()
	{
		if (!this) return Vector3{ 0,0,0 };
		return *(Vector3*)(this + 0x07D0);
	}

	Vector3 SetVelocity()
	{
		if (!this) return Vector3{ 0,0,0 };
		return *(Vector3*)(this + 0x07D0) = Vector3(0, 0, 0);
	}
	float m_gravity(float grav)
	{
		if (!this) return 0;
		return *(float*)(this + 0x0C3C) = grav;
	}
	float m_body_health(float health)//0x0840
	{
		if (!this) return 0;
		return *(float*)(this + 0x0820) = health;
	}
	float m_engine_health(float health)//0x0908
	{
		if (!this) return 0;
		return *(float*)(this + 0x08E8) = health;
	}
	float body_health()
	{
		if (!this) return 0;
		return *(float*)(this + 0x0820);
	}
	float engine_health()
	{
		if (!this) return 0;
		return *(float*)(this + 0x08E8);
	}
	uint32_t m_door_lock_status(UINT32 locked)
	{
		if (!this) return 0;
		return *(UINT32*)(this + g_offsets->m_veh_lock_state) = locked;
	}
	uint32_t door_lock_status()
	{
		if (!this) return 0;
		return *(UINT32*)(this + g_offsets->m_veh_lock_state);
	}
};

enum PedTask {
	TASK_NONE,
	TASK_FOOT = 1 << 4,
	TASK_UNK = 1 << 5,
	TASK_DRIVING = 1 << 6
	
};
class CAttackers
{
public:
	uintptr_t m_attacker0; //0x0000 
	char pad_0x0008[0x10]; //0x0008
	uintptr_t m_attacker1; //0x0018 
	char pad_0x0020[0x10]; //0x0020
	uintptr_t m_attacker2; //0x0030 
}; //Size=0x0038
class CPedClothing;
class CPed
{
public:

	CObjectNavigation* m_navigation_manager()
	{
		if (!this) return 0;
		return (CObjectNavigation*)*(uintptr_t*)(this + 0x30); 
	}

	CVehicleManager* m_vehicle_manager()
	{
		if (!this) return 0;
		return (CVehicleManager*)*(uintptr_t*)(this + g_offsets->m_vehicle_manager); 
	}

	CPlayerInfo* m_player_info()
	{
		if (!this) return 0;
		return (CPlayerInfo*)* (uintptr_t*)(this + g_offsets->m_player_info); 
	}

	CWeaponManager* m_weapon_manager()
	{
		if (!this) return 0;
		return (CWeaponManager*)*(uintptr_t*)(this + g_offsets->m_weapon_manager); //0x10B8
	}
	bool isPlayer()
	{
		return *(uint32_t*)(this + g_offsets->m_player_info) != 0;
	}
	bool IsPedOrFalse()
	{
		if (!this) return 0;
		if (m_player_info())
			return false;
		else
			return true;
		/*
		auto ped_type = this->m_entityType();

		ped_type = ped_type << 11 >> 25;

		if (ped_type != 2) return true; 
		else return false;*/
	}
	bool IsAnimals()
	{
		if (!this) return 0;
		auto ped_type = this->m_entityType();

		ped_type = ped_type << 11 >> 25;

		if (ped_type > 27) return true;
		else return false;
	}
	int GetPedClothing() {
		CPedClothing* PedCloth = *reinterpret_cast<CPedClothing**>((char*)this + 0x48);
		if (PedCloth)
			return *reinterpret_cast<int*>((char*)PedCloth + 0xEC);
		else
			return 0;
	}
	const char* GetName()
	{
		auto playerinfo = this->m_player_info();
		if (!playerinfo)return "NPC";
		void* addr = (void*)(playerinfo + 0x20);
		const char* name = g_offsets->GetPlayerNameFromScAddr(addr);
		return name;
	}
	int GetPedClothing2() {
		CPedClothing* PedCloth = *reinterpret_cast<CPedClothing**>((char*)this + 0x48);
		if (PedCloth)
			return *reinterpret_cast<int*>((char*)PedCloth + 0x124);
		else
			return 0;
	}
	bool IsFriend(int id = 0)
	{
		auto playerinfo = this->m_player_info();
		int playerid = id;
		if (playerinfo && !id)
			playerid = playerinfo->m_player_id();

		auto it = std::find(p_variables->p_friend.Friend.begin(), p_variables->p_friend.Friend.end(), playerid);
		if (it != p_variables->p_friend.Friend.end())
			return true;
		return false;
	}
	bool IsStaff()
	{
		auto playerinfo = this->m_player_info();
		int playerid = 0;
		if (playerinfo)
			playerid = playerinfo->m_player_id();

		auto it = std::find(p_variables->p_friend.Staff.begin(), p_variables->p_friend.Staff.end(), playerid);
		if (it != p_variables->p_friend.Staff.end())
			return true;
		return false;
	}
	void markAsStaff()
	{
		auto playerinfo = this->m_player_info();
		if (playerinfo)
			p_variables->p_friend.Staff.push_back(playerinfo->m_player_id());
	}
	BYTE isGodMode()
	{
		if (!this) return 0;
		return *(BYTE*)(this + 0x189);
	}
	uint8_t m_visible(BYTE val)
	{
		//uint8_t m_invisible; //0x002C
		if (!this) return 0;
		return *(uint8_t*)(this + 0x2C) = val;
	}
	uint8_t is_visible()
	{
		if (!this) return 0;
		return *(uint8_t*)(this + 0x2C);
	}
	bool IsVisible() {
		return *reinterpret_cast<BYTE*>((char*)this + 0xAC) > 0 ? true : false;
	}
	void SetAlpha(float alpha) {
		(*(BYTE*)((DWORD64)this + 0xAC)) = alpha;
	}
	BYTE GetAlpha() {
		return (*(BYTE*)((DWORD64)this + 0xAC));
	}
	bool in_vehicle()
	{
		if (!this) return 0;
		auto vehicle = this->m_vehicle_manager();
		if (vehicle)
			return true;
		else return false;

	}
	bool isInVehicle()
	{
		if (!this) return 0;
		uint8_t ped_task = *(uint8_t*)(this + 0x144B);
		if (ped_task == TASK_DRIVING)
			return true;
		
	}
	bool isInVehicle_()
	{
		if (!this) return 0;
		return *(UINT32*)(this + 0xE52) > 0;
	}

	uint32_t m_entityType()
	{
		if (!this) return 0;
		return *(uint32_t*)(this + g_offsets->m_enttype);
	}

	Vector3 m_heading(Vector3 heading)
	{
		if (!this) return { 0,0 ,0 };
		return *(Vector3*)(this + 0x70) = heading;
	}

	Vector3 heading()
	{
		if (!this) return { 0,0 ,0 };
		return *(Vector3*)(this + 0x70);
	}

	float m_health(float Health)
	{
		if (!this) return 0;
		return *(float*)(this + 0x0280) = Health;
	}
	float health()
	{
		if (!this) return 0;
		return *(float*)(this + 0x0280);
	}
	float m_healthmax()
	{
		if (!this) return 0;
		return *(float*)(this + 0x284);
	}
	float m_armor(float Armor)
	{
		if (!this) return 0;
		return *(float*)(this + g_offsets->m_armor) = Armor;
	}
	float armor()
	{
		if (!this) return 0;
		return *(float*)(this + g_offsets->m_armor);
	}
	BYTE m_damage_bits(BYTE on_off)
	{
		if (!this) return 0;
		return *(BYTE*)(this + 0x189) = on_off;
	}

	Vector3 m_visual_pos()
	{
		if (!this) return Vector3{ 0,0,0 };
		return *(Vector3*)(this + 0x90);
	}

	Vector3 velocity()
	{
		if (!this) return Vector3(0, 0, 0);
		return *(Vector3*)(this + 0x320);//0x07D0
	}

	Vector3 m_velocity(Vector3 velo)
	{
		if (!this) return Vector3(0, 0, 0);
		return *(Vector3*)(this + 0x320) = velo;//0x07D0
	}

	Vector3 m_set_visual_pos(Vector3 Pos)
	{
		if (!this) return Vector3(0, 0, 0); 
		return *(Vector3*)(this + 0x90) = Pos; 
	 }

	void NoGravity(bool a1) {
		
		if (a1)
			*reinterpret_cast<float*>((char*)this + 0xC1C) = 0.0f;
		else
			*reinterpret_cast<float*>((char*)this + 0xC1C) = 9.800000191f;
	}
	unsigned char Set_Ragdoll(bool value)
	{
		if (!this) return 0;
		if (value)
		{
			return *(unsigned char*)(this + g_offsets->m_enttype) = 0x0;
		}
		else
		{
			return *(unsigned char*)(this + g_offsets->m_enttype) = 11<<20;
		}

	}
	CAttackers* Attackers()
	{
		if (!this) return 0;
		return (CAttackers*)(*(DWORD64*)(this + 0x288));
	}
};

class CPedList
{
public:
	CPed* m_ped(int index)
	{
		if (!this) return 0;
		return (CPed*)(*(DWORD64*)(this + (index * 0x10)));
	}
};

class CPedInterface
{
public:
	DWORD64 m_pedmax()
	{
		if (!this) return 0;
		return (DWORD64)(*(DWORD64*)(this + 0x108));
	}
	DWORD64 m_pedcount()
	{
		if (!this) return 0;
		return (DWORD64)(*(DWORD64*)(this + 0x110));
	}

	CPedList* m_pedlist()
	{
		if (!this) return 0;
		return (CPedList*)(*(DWORD64*)(this + 0x100));
	}
};

class CVehicleList
{
public:
	CVehicleManager* m_getvehicle(int index)
	{
		if (!this) return 0;
		return (CVehicleManager*)(*(DWORD64*)(this + (index * 0x10)));
	}

};



class CVehicleInterface
{
public:
	DWORD64 m_vehiclemax()
	{
		if (!this) return 0;
		return (DWORD64)(*(DWORD64*)(this + 0x188));
	}
	DWORD64 m_vehiclecount()
	{
		if (!this) return 0;
		return (DWORD64)(*(DWORD64*)(this + 0x190));
	}
	CVehicleList* m_vehiclelist()
	{
		if (!this) return 0;
		return (CVehicleList*)(*(DWORD64*)(this + 0x180));
	}
};
class CObject
{
public:
	uintptr_t model_info()
	{
		if (!this) return 0;
		return *(uintptr_t*)(this + 0x20);
	}
	uint32_t hash()
	{
		if (!this) return 0;
		if(!this->model_info())return 0;
		return *(uint32_t*)(this->model_info() + 0x18);
	}
	Vector3 location()
	{
		if (!this) return Vector3(0, 0, 0);
		return *(Vector3*)(this + 0x90);//0x07D0
	}

};
class CObjectList
{
public:
	CObject* m_getobject(int index)
	{
		if (!this) return 0;
		return (CObject*)(*(DWORD64*)(this + (index * 0x10)));
	}

};
struct CObjectInterface
{
	char pad_0x0000[0x158]; //0x0000
	CObjectList* list_obj; //0x0158
	__int32 max_objects; //0x0160
	char pad_0x0164[0x4]; //0x0164
	__int32 cur_objects; //0x0168
	char pad_0x016C[0x5C]; //0x016C

}; //replay_objectlist_interface_ptr; //Size=0x01C8

class CReplayInterface
{
public:

	CPedInterface* m_ped_interface()
	{
		if (!this) return 0;
		return (CPedInterface*)(*(DWORD64*)(this + 0x0018));
	}

	CVehicleInterface* m_vehicleinterface()
	{
		if (!this) return 0;
		return (CVehicleInterface*)(*(DWORD64*)(this + 0x0010));
	}

	CObjectInterface* m_objectinterface()
	{
		if (!this) return 0;
		return (CObjectInterface*)(*(DWORD64*)(this + 0x0028));
	}

};

class CPedFactory
{
public: 
	CPed* m_local_ped()
	{
		if (!this) return 0;
		return (CPed*)*(uintptr_t*)(this + 0x8);
	}
};
class camBaseDirector // sig 48 39 0D ? ? ? ? 75 58 8B 42 20
{
public:
	char pad_0000[704]; //0x0000
	class camBaseCamera* camera; //0x02C0
}; //Size: 0x02C8

class camBaseCamera
{
public:
	char pad_0000[16]; //0x0000
	class camBaseCameraMetadata* camMetadata; //0x0010
}; //Size: 0x0018

class camBaseCameraMetadata {
};

class camFirstPersonShooterCameraMetadata : camBaseCameraMetadata
{
public:
	char pad_0000[36]; //0x0000
	float MinFov; //0x0024 - slider minimum
	float NearClip; //0x0028 - camera clipping
	char pad_002C[40]; //0x002C
	float MinPitch; //0x0054 - down
	float MaxPitch; //0x0058 - up
	char pad_005C[464]; //0x005C
	float MaxFov; //0x022C - slider maximum
	char pad_0230[12]; //0x0230
	float PhoneFov; //0x023C - phone out
	char pad_0240[80]; //0x0240
	float LadderPitch; //0x0290 - climbing ladders
}; //Size: 0x0294

class camFollowPedCameraMetadata : camBaseCameraMetadata
{
public:
	char pad_0000[48]; //0x0000
	float Fov; //0x0030
	float NearClip; //0x0034
}; //Size: 0x0038

class camFollowVehicleCameraMetadata : camBaseCameraMetadata
{
public:
	char pad_0000[48]; //0x0000
	float Fov; //0x0030
	float NearClip; //0x0034
}; //Size: 0x0038

class camCinematicMountedCameraMetadata : camBaseCameraMetadata
{
public:
	void* N00001D18; //0x0000
	int32_t HashedName; //0x0008 - can be used to differentiate between types (might be the same for all camBaseCameraMetadata, idk)
	char pad_000C[72]; //0x000C
	float BaseFov; //0x0054
	float BaseNearClip; //0x0058
	float TripleHeadNearClip; //0x005C - i have no idea
	float RelativeAttachPositionX; //0x0060 - you can offset the camera
	float RelativeAttachPositionY; //0x0064
	float RelativeAttachPositionZ; //0x0068
	char pad_006C[772]; //0x006C
	bool IsBehindVehicleGlass; //0x0370
	bool N00001F43; //0x0371
	bool ShouldDisplayReticule; //0x0372
	bool ShouldMakeFollowPedHeadInvisible; //0x0373 - allows you to make your head somewhat visible if you offset the camera
	bool ShouldMakePedInAttachSeatInvisible; //0x0374
}; //Size: 0x0375

class Blip_t
{
public:
	int32_t m_id; //0x0000
	uint16_t m_blip_array_index; //0x0004
	char pad_0006[4]; //0x0006
	bool m_active; //0x000A
	uint8_t N00000197; //0x000B
	int32_t m_entity_id; //0x000C
	float m_x; //0x0010
	float m_y; //0x0014
	float m_z; //0x0018
	char pad_001C[4]; //0x001C
	uint32_t m_display_bits; //0x0020
	uint32_t m_render_bits; //0x0024
	char* m_message; //0x0028
	char pad_0030[8]; //0x0030
	uint32_t m_description; //0x0038
	char pad_003C[4]; //0x003C
	int32_t m_icon; //0x0040
	int16_t m_flash_interval; //0x0044
	int16_t m_flash_timer; //0x0046
	uint32_t m_color; //0x0048
	uint32_t m_secondary_color; //0x004C
	float m_scale_x; //0x0050
	float m_scale_y; //0x0054
	float m_rotation; //0x0058
	uint8_t m_mission_bits; //0x005C
	uint8_t m_priority; //0x005D
	uint8_t m_display_id; //0x005E
	uint8_t m_alpha; //0x005F
	int8_t m_category; //0x0060
	int8_t m_show_number; //0x0061
	char pad_0062[14]; //0x0062
}; //Size: 0x0070
static_assert(sizeof(Blip_t) == 0x70, "Blip_t is not sized properly.");
class CBlipEntry
{
public:
	Blip_t* m_pBlip; //0x0000 

};//Size=0x0008

class CBlipList
{
public:
	CBlipEntry m_Blips[1500]; //0x0000

};//Size=0x2F18
