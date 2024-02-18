#pragma once
#include <Includes.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <invoker.hpp>

struct timer
{
public:
	bool tick100 = false;
	bool tick200 = false;
	bool tick500 = false;
	bool tick1000 = false;
	int delta = 0;
};

class Game_Math
{
public:
	ImVec2 WindowSize = ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	inline ImVec2 WorldToScreen(Vector3 vPos);
	ImVec2 GetBone_W2S_Test(CPed* cPed, const char* bone_name);
	inline Vector3 GetBone(CPed* cPed, unsigned int Bone_Mask);
	ImVec2 GetBone_W2S(CPed* cPed, unsigned int Bone_Mask); 
	Vector2 WorldToScreenVec2(Vector3 vPos); 
	ImVec2 WorldToRadar(Vector3 Location, FLOAT RadarX, FLOAT RadarY, float RadarSize, float RadarZoom);
	void set_aim_angels(Vector3 point, float deltaTime, int smoothSet = 0);
	void MouseAim(Vector3 AimCoord, int smooth = 0);
	float pythag(ImVec2 src, ImVec2 dst);
	float degToRad(float degs);
	PVector3 Prot_to_direction(PVector3 rot);
	Vector3 rot_to_direction(Vector3 rot);
	bool raycast(Entity* ent, Cam camera);
	CVehicleManager* GetBestVehicle();
 };
inline static bool RequestControlOnce(Entity entity)
{
	if (!NETWORK::NETWORK_IS_IN_SESSION() || NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity))
	{
		//ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Already Have Controlled") });
		return true;//offline mode
	}

	int netID = NETWORK::NETWORK_GET_NETWORK_ID_FROM_ENTITY(entity);
	NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netID, true);
	bool success = NETWORK::NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(netID);
	//bool success = g_pattern->pNetworkRequestControlOfEntity(entity);
	//if (!success) ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Not Allow To Control This Car") });
	if (NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity))
		return true;
	return success;
}



inline Game_Math* g_Math = new Game_Math;