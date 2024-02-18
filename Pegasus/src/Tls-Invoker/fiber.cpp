#include "fiber.hpp"
#include "tick.hpp"
#include <memory.h>
#include <GameMath.h>
#include <Aimbot.h>
#include <Vehicle.h>
#include <Weapon.h>
#include <Player.h>
#include <World.h>
#include <invoker.hpp>
#include <resources.h>




static struct timer timer;
inline void ckTimer()
{
	auto currentTick = GetTickCount();
	static auto lastTick100 = currentTick, lastTick200 = currentTick, lastTick500 = currentTick, lastTick1000 = currentTick;
	static auto lastTick = currentTick;

	timer.delta = currentTick - lastTick;
	lastTick = currentTick;

	if (currentTick - lastTick100 > 100) {
		lastTick100 = currentTick;
		timer.tick100 = true;
	}
	else timer.tick100 = false;
	if (currentTick - lastTick200 > 200) {
		lastTick200 = currentTick;
		timer.tick200 = true;
	}
	else timer.tick200 = false;
	if (currentTick - lastTick500 > 500) {
		lastTick500 = currentTick;
		timer.tick500 = true;
	}
	else timer.tick500 = false;
	if (currentTick - lastTick1000 > 1000) {
		lastTick1000 = currentTick;
		timer.tick1000 = true;
	}
	else timer.tick1000 = false;
}
inline void fpsBooster()
{
	CPedFactory* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
	auto localPlayer = g_pedFactory->m_local_ped();
	auto coords = localPlayer->m_visual_pos();
	/*
		CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
	CPedInterface* pedinterface = ReplayInterface->m_ped_interface();
	for (int i = 0; i < pedinterface->m_pedmax(); i++)
	{
		CPed* cPed = pedinterface->m_pedlist()->m_ped(i);
		if (!cPed) continue;
		Entity entityid = g_pattern->pointer_to_handle(cPed);
		if (!ENTITY::IS_ENTITY_ON_SCREEN(entityid))
			ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&entityid);
		//PED::SET_PED_AO_BLOB_RENDERING(entityid, false);
	}*/

	STREAMING::OVERRIDE_LODSCALE_THIS_FRAME(0.4f);
	GRAPHICS::DISABLE_OCCLUSION_THIS_FRAME();
	GRAPHICS::SET_DISABLE_DECAL_RENDERING_THIS_FRAME();
	GRAPHICS::REMOVE_PARTICLE_FX_IN_RANGE(coords.x, coords.y, coords.z, 10.0);
	GRAPHICS::SET_ARTIFICIAL_LIGHTS_STATE(false);
	GRAPHICS::DISABLE_SCREENBLUR_FADE();
	STREAMING::CLEAR_HD_AREA();
	AUDIO::CLEAR_ALL_BROKEN_GLASS();
	//UI::CLEAR_SMALL_PRINTS();
	//UI::CLEAR_PRINTS();
	static bool oneTimeExec = true;
	if (oneTimeExec)
	{
		oneTimeExec = false;
		GAMEPLAY::_SET_RAIN_FX_INTENSITY(0.001);
		GAMEPLAY::SET_WIND_SPEED(0.001);
		GRAPHICS::_CASCADE_SHADOWS_CLEAR_SHADOW_SAMPLE_TYPE();
		GRAPHICS::CASCADE_SHADOWS_SET_AIRCRAFT_MODE(false);
		GRAPHICS::CASCADE_SHADOWS_ENABLE_ENTITY_TRACKER(true);
		GRAPHICS::CASCADE_SHADOWS_SET_DYNAMIC_DEPTH_MODE(false);
		GRAPHICS::CASCADE_SHADOWS_SET_ENTITY_TRACKER_SCALE(0.f);
		GRAPHICS::CASCADE_SHADOWS_SET_DYNAMIC_DEPTH_VALUE(0.f);
		GRAPHICS::CASCADE_SHADOWS_SET_CASCADE_BOUNDS_SCALE(0.f);
	}
}
void HookNative()
{
	Log::Msg(Encrypt("Tester Mode"));
	//HookNat(0x7FDD1128, TriggerServer(0x7FDD1128));
	//HookNat(0xDACE671663F2F5DB, Decor(0xDACE671663F2F5DB));
	//HookNat(0x4700A416E8324EF3, MyCustomHandler(0x4700A416E8324EF3));
	//SetNativeHandler(0x34616828CD07F1A1, GetNativeHandler(0x01FEE67DB37F59B2));
	SetNativeHandler(0x34616828CD07F1A1,GetNativeHandler(0x6F972C1AB75A1ED0));//IsPedShooting to //ispedonanytrain
	
	breakNative(0x048746E388762E11);//NetworkIsInSpectatorMode
	//breakNative(0xD24D37CC275948CC);//GetHashKey
	breakNative(0xBE8CD9BE829BBEBF);//GetEntityProofs
	breakNative(0xB721981B2B939E07);//GetPlayerInvincible
	breakNative(0xD95CC5D2AB15A09F);//GetEntityCanBeDamaged
	//breakNative(0x7CE1CCB9B293020E);//GetVehicleNumberPlateText
	breakNative(0x128F79EDCECE4FD5);//CanPedRagdoll
	//breakNative(0x47D6F43D77935C75);//IsEntityVisible
	breakNative(0x5A47B3B5E63E94C6);//GetEntityAlpha
	breakNative(0x1DD55701034110E5);//GetEntityHeightAboveGround
	breakNative(0xA200EB1EE790F448);//GetFinalRenderedCamCoord
	breakNative(0x3133B907D8B32053);//GetWeaponDamage
	//breakNative(0x3BE0BB12D25FB305);//GetWeaponDamageType
	SetNativeHandler(0x3BE0BB12D25FB305, GetNativeHandler(0x6F972C1AB75A1ED0));//GetWeaponDamageType
	//breakNative(0x9F47B058362C84B5);//GetEntityModel						///
	breakNative(0xC9D55B1A358A5BF7);//IsPedModel
	breakNative(0x423E8DE37D934D89);//IsVehicleModel			//
	SetNativeHandler(0x7EE53118C892B513,GetNativeHandler(0x6F972C1AB75A1ED0));//GetPedConfigFlag
	breakNative(0xD5037BA82E12416F);//GetEntitySpeed
	if (p_variables->p_settings.bypassWeaponCheck)
	{
		breakNative(0x8DECB02F88F428BC);//HasPedGotWeapon			//
		breakNative(0x0A6DB4965674D243);//GetSelectedPedWeapon
		breakNative(0xF46CDC33180FDA94);//GetWeapontypeModel		//
	}

	SetNativeHandler(0xB0760331C7AA4155,GetNativeHandler(0x6F972C1AB75A1ED0));//GetIsTaskActive
	SetNativeHandler(0x1F0B79228E461EC9, GetNativeHandler(0x6F972C1AB75A1ED0));//IsEntityPlayingAnim
	SetNativeHandler(0x9483AF821605B1D8, GetNativeHandler(0x6F972C1AB75A1ED0));//GetPedArmour
	//breakNative(0xF1B760881820C952);//GetDistanceBetweenCoords
	breakNative(0x2A3D7CDA);//GetPlayerWeaponDamageModifier
	breakNative(0xF2E3912B);//GetPlayerInvincible_2
	//breakNative(0xDAFCB3EC);//GetEntityModel

}//GetDistanceBetweenCoords //GetVehiclePedIsUsing



void on_Tls_Thread() {				//you are on Tls thread Safe to run native without crash

	static bool doonce = true;
	if (doonce)//p_variables->p_resource.enable && 
	{
		doonce = false;
		///////////////not safe////////////
		if(p_variables->misc.TESTER)
			HookNative();
		//SetNativeHandler(0x91310870, Print());
		//InitBase();
		///////////////////////////////////
	}
	if (p_variables->p_resource.enable)
	{
		p_variables->p_resource.enable = false;
		//const char* jsons = R"([{"multiline":true,"color":[255,255,255],"mode":"all","args":["^3TranMC^0","Hello!EveryOne..."]}])";//work

		//json jsonString;
		//jsonString["name"] = "hunger";
		//jsonString["val"] = 1000000;
		//json jsonArray = json::array();
		//jsonArray.push_back(jsonString);
	
		//std::string dataaa = jsonArray.dump(2);


		//TriggerEvent(true, "FIREAC:DeleteEntitys", "VEHCILE");
		//TriggerEvent(true, "FIREAC:DeleteEntitys", "PEDS");
		//CFX::TriggerEventInternal("chat:addMessage", sbuffer->data(), sbuffer->size());
		
		//ImGui::InsertNotification({ ImGuiToastType_Success, 3000, (const char*)sbuffer->data() });
		//CFX::TriggerEventInternal("PGX:RevivePlayer", payload, 0);
		//CFX::TriggerEventInternal("esx_ambulancejob:revive", payload, 0);
		//CFX::TriggerEventInternal("paramedic:revive", payload, 0);
		//CFX::TriggerEventInternal("playerSpawned", payload, 0);
		//CFX::TriggerEventInternal("esx_inventoryhud:openPlayerInventory", "", 0);
		//ÆºxinchaoÉ
		//TriggerEvent("esx_status:set", "hunger", 1000000)
		//esx_inventoryhud:openPlayerInventory

	}


	auto PlayerPedID = PLAYER::PLAYER_PED_ID();
	if (PED::IS_PED_IN_ANY_VEHICLE(PlayerPedID, false))
		p_variables->p_vehicle.imOnVehicle = true;
	else
		p_variables->p_vehicle.imOnVehicle = false;

	//DrawTxt("strbodyH.c_str()",  600, 300, 1.0f, 0.25f, p_variables->p_color.Blue);


	ckTimer();
	p_aimbot_func->Initialize(timer);
	p_vehicle_func->Initialize(timer);
	g_player_func->Initialize(timer);
	g_weapon_func->Initialize(timer);
	p_world_func->Initialize(timer);
	if (p_variables->misc.FPSBOOSTER)
		fpsBooster();

	if (p_variables->p_menu.open)
	{
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);
	}
		
	
	tick::pNativeInvoker.on_tick();
	Fiber::Instance().GameFiber->wait(0);
}

void Fiber::Call()
{
	//Log::Msg("[+]Initializing GameFiber...");
	
	this->GameFiber = std::make_unique<fiber::GameFiber_Task>(nullptr, on_Tls_Thread);
}

