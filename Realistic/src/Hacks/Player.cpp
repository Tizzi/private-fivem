#include <Player.h>
#include <GameMath.h>
#include <invoker.hpp>
#include <discord.h>
#include <Vehicle.h>



 
inline void freezePlayer(CPlayerInfo* cPlayerInfo, bool v = true, uintptr_t type = 2) {
	 
	if (v)
		*(uintptr_t*)(cPlayerInfo + g_offsets->freeze) = type; //0x238
	else *(uintptr_t*)(cPlayerInfo + g_offsets->freeze) = 0;
}


inline bool animate_local_player(CPed* playerPed, std::string dict, std::string anim, bool restore)
{
	DWORD cPed = g_pattern->pointer_to_handle(playerPed); 
 	if (restore)
		return true;
	g_pattern->request_anim_dict(&dict[0]);
	if (!g_pattern->has_anim_dict_loaded(&dict[0]))
		return false;
	g_pattern->ai_task_play_anim(cPed, &dict[0], &anim[0], 8.f, 1.f, -1, 1, 1.f, 0, 0, 0, 0, 0);
	return true;
}

inline void shootPed(int player,CPed* Target,int WeaponHash)
{
	WEAPON::REQUEST_WEAPON_ASSET(WeaponHash, 31, 0);
	auto PedLoc = Target->m_visual_pos();
	auto WeaponDamage = WEAPON::GET_WEAPON_DAMAGE(WeaponHash, NULL);
	auto heading = Target->heading();
	auto startpos = PedLoc + heading;
	auto endpos = PedLoc;
	GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startpos.x, startpos.y, startpos.z,
		endpos.x, endpos.y, endpos.z, WeaponDamage, false, WeaponHash, player,
		false, true, 200.f);
}
inline void StartNetworkFxAtCoord(float x, float y, float z, const char* FxName, const char* FxAsset)
{
	STREAMING::REQUEST_NAMED_PTFX_ASSET(FxAsset);
	MOBILE::_CELL_CAM_MOVE_FINGER(5);
	GRAPHICS::_SET_PTFX_ASSET_NEXT_CALL(FxAsset);
	
	GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(FxName, x, y, z, 0.0, 0.0, 1, 3.0, 1, 1, 1);//1065353216
	//GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_ENTITY(ss3, PLAYER::PLAYER_PED_ID(), 0.0, 0.0, -0.5, 0.0, 0.0, 0.0, 1.0, false, false, false);
}
Ped ClonePed(Ped ped)
{
	if (ENTITY::DOES_ENTITY_EXIST(ped) && !ENTITY::IS_ENTITY_DEAD(ped))
	{
		return PED::CLONE_PED(ped, ENTITY::GET_ENTITY_HEADING(ped), 1, 1);
	}

	return 0;
}

Ped CreatePed(const char* PedName, Vector3 SpawnCoordinates, int ped_type, bool network_handle)
{
	Ped NewPed;
	int PedHash = rage::joaat_t(PedName);
	static bool requesting = false;
	if (STREAMING::IS_MODEL_IN_CDIMAGE(PedHash))
	{
		if (STREAMING::IS_MODEL_VALID(PedHash))
		{
			if (!requesting)
			{
				STREAMING::REQUEST_MODEL(PedHash);
				requesting = true;
			}
			
			if(!STREAMING::HAS_MODEL_LOADED(PedHash)) return -1;

			NewPed = PED::CREATE_PED(ped_type, PedHash, SpawnCoordinates.x, SpawnCoordinates.y, SpawnCoordinates.z, 0, network_handle, 1);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(PedHash);
			requesting = false;
			return NewPed;
		}
	}

	return -1;
}


inline void UpdateBlips()
{
	CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
	CPed* LocalPlayer = ped_factory->m_local_ped();
	CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
	CPedInterface* pedinterface = ReplayInterface->m_ped_interface();
	static bool cleanOnce = false;
	static std::vector<int> headId(256, -1);

	if (p_variables->radar2d.playerBlips) {
		cleanOnce = true;
		for (int id = 0; id < pedinterface->m_pedmax(); id++) {
			CPed* cPed = pedinterface->m_pedlist()->m_ped(id);
			if (!cPed) continue;
			if (cPed == LocalPlayer && !p_variables->radar3d.include_self) continue;
			if (cPed->IsPedOrFalse() && !p_variables->ped_esp.pedBlips) continue;
			uintptr_t ped = g_pattern->pointer_to_handle(cPed);
			//Ped ped = PLAYER::GET_PLAYER_PED(id);
			Blip blip = HUD::GET_BLIP_FROM_ENTITY(ped);

			// HEAD DISPLAY NAMETAGS
			if (!p_variables->color.NameTags)
			{
				// Create head display (this is safe to be spammed)
				headId[id] = HUD::CREATE_FAKE_MP_GAMER_TAG(ped, cPed->GetName(), false, false, "", false);
				int wantedLvl = 0;// PLAYER::GET_PLAYER_WANTED_LEVEL(id);

				// Wanted level display
				if (wantedLvl) {
					HUD::SET_MP_GAMER_TAG_VISIBILITY(headId[id], 7, true, 0); // Add wanted sprite
					HUD::SET_MP_GAMER_TAG_VISIBILITY(headId[id], wantedLvl, 1, 0); // Set wanted number
				}
				else {
					HUD::SET_MP_GAMER_TAG_VISIBILITY(headId[id], 7, false, 0);
				}
			}
			else
				HUD::DISPLAY_PLAYER_NAME_TAGS_ON_BLIPS(true);


			// BLIP PROCESSING

			if (!HUD::DOES_BLIP_EXIST(blip)) { // Add blip and create head display on player
				bool veh = PED::IS_PED_IN_ANY_VEHICLE(ped, false);
				if (veh || !p_variables->ped_esp.OnlyCarBlips) //add blips for peds if OnlyShow car off
				{
					blip = HUD::ADD_BLIP_FOR_ENTITY(ped);
					HUD::SET_BLIP_SPRITE(blip, 1);
					HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, true); // Player Blip indicator
					//HUD::SET_BLIP_NAME_TO_PLAYER_NAME(blip, id); // update blip name
					HUD::SET_BLIP_ALPHA(blip, 233);
				}
			}
			else { // update blip
				if (HUD::GET_BLIP_ALPHA(blip) != 233) continue;//no update for blips that we havent created
				Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(ped, false);
				int blipSprite = HUD::GET_BLIP_SPRITE(blip);
				//HUD::SET_BLIP_AS_SHORT_RANGE(blip,false);
				
				if (!ENTITY::GET_ENTITY_HEALTH(ped)) { // dead
					if (blipSprite != 274) {
						HUD::SET_BLIP_SPRITE(blip, 274);
						HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // Player Blip indicator
					}
				}
				else if (veh) {//ped on vehicle
					int vehClass = VEHICLE::GET_VEHICLE_CLASS(veh);
					int vehModel = ENTITY::GET_ENTITY_MODEL(veh);

					// Add logic for different vehicle classes and models
					if (vehClass == 15) { // Helicopters
						if (blipSprite != 602) {//612
							HUD::SET_BLIP_SPRITE(blip, 602);
							HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // Player Blip indicator
						}
					}
					else if (vehClass == 8) { // Motorcycles
						if (blipSprite != 522) {
							HUD::SET_BLIP_SPRITE(blip, 522);
							HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // Motorcycles
						}
					}
					else if (vehClass == 16) { // Plane
						static UINT32 m1 = rage::joaat("f16c"), m2 = rage::joaat("WARTHOG"), m3 = rage::joaat("besra"), m4 = rage::joaat("hydra"),m5 = rage::joaat("lazer");
						if (vehModel == m1 || vehModel == m2) { // Fighter Jets
							if (blipSprite != 640) {
								HUD::SET_BLIP_SPRITE(blip, 640);
								HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // fighter jet
								HUD::SET_BLIP_AS_SHORT_RANGE(blip, false);
							}
							HUD::SET_BLIP_AS_SHORT_RANGE(blip, false);
						}
						if (vehModel == m3 || vehModel == m4 || vehModel == m5) { // Fighter Jets
							if (blipSprite != 424) {
								HUD::SET_BLIP_SPRITE(blip, 424);
								HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // fighter jet
							}
							HUD::SET_BLIP_AS_SHORT_RANGE(blip, false);
						}
						else if (blipSprite != 584) {
							HUD::SET_BLIP_SPRITE(blip, 584);
							HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // normal plane
						}
					}
					else if (vehClass == 14) { // Boat
						if (blipSprite != 427) {
							HUD::SET_BLIP_SPRITE(blip, 427);
							HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // Boat
						}
					}
					else if (vehClass == 19)
					{
						static UINT32 m10 = rage::joaat("rhino"), m11 = rage::joaat("AMTRAK"), m12 = rage::joaat("M3A3"), m13 = rage::joaat("BTR82A"), m14 = rage::joaat("T72B3"), m15 = rage::joaat("type75");
						if (vehModel == m10|| vehModel == m11 || vehModel == m12 || vehModel == m13 ||
							vehModel == m14 || vehModel == m15
							) { // Tank
							if (blipSprite != 421) {
								HUD::SET_BLIP_SPRITE(blip, 421);
								HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // tank
							}
						}
						else
						{// if (vehModel == rage::joaat("insurgent") || vehModel == rage::joaat("insurgent2") || vehModel == rage::joaat("insurgent3")) { // Technical
							if (blipSprite != 426) {
								HUD::SET_BLIP_SPRITE(blip, 426);
								HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // weaponize car
							}
						}
					}
					else if (vehModel == rage::joaat("taxi")) { // Cab/ Taxi
						if (blipSprite != 198) {
							HUD::SET_BLIP_SPRITE(blip, 198);
							HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // taxi
						}
					}
					else
					{
						if (blipSprite != 225) {
							HUD::SET_BLIP_SPRITE(blip, 225);
							HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, false); // other cars
						}
					}

					// Show number in case of passengers
					int passengers = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(veh);

					if (passengers) {
						if (!VEHICLE::IS_VEHICLE_SEAT_FREE(veh, -1)) {
							passengers = passengers + 1;
						}
						HUD::SHOW_NUMBER_ON_BLIP(blip, passengers);
					}
					else {
						HUD::HIDE_NUMBER_ON_BLIP(blip);
					}
				}
				else {
					// Remove leftover number
					HUD::HIDE_NUMBER_ON_BLIP(blip);
					if (blipSprite != 1) { // default blip
						HUD::SET_BLIP_SPRITE(blip, 1);
						HUD::SHOW_HEADING_INDICATOR_ON_BLIP(blip, true);							// Player Blip indicator
					}
				}
				if (cPed->IsFriend())
					HUD::SET_BLIP_COLOUR(blip, 69);
				else if(cPed->isPlayer())
					HUD::SET_BLIP_COLOUR(blip, 59);
					//HUD::SET_BLIP_AS_FRIENDLY(blip, false);
				HUD::SET_BLIP_ROTATION(blip, static_cast<int>(ENTITY::GET_ENTITY_HEADING(veh))); // update rotation
				HUD::SET_BLIP_SCALE(blip, 0.85); // set scale

				// set player alpha
				if(0)//no change alpha
				if (HUD::IS_PAUSE_MENU_ACTIVE()) {
					HUD::SET_BLIP_ALPHA(blip, 255);
				}
				else {
					PVector3 playerCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(-1), true);
					PVector3 targetCoords = ENTITY::GET_ENTITY_COORDS(ped, true);
					float distance = static_cast<float>(floor(sqrt(pow(playerCoords.x - targetCoords.x, 2) + pow(playerCoords.y - targetCoords.y, 2)) / -1) + 900);

					if (distance < 0) {
						distance = 0;
					}
					else if (distance > 255) {
						distance = 255;
					}
					HUD::SET_BLIP_ALPHA(blip, static_cast<int>(distance));
				}
			}

		}
	}
	else if (cleanOnce)
	{
		for (int id = 0; id < pedinterface->m_pedmax(); id++) {
			CPed* cPed = pedinterface->m_pedlist()->m_ped(id);
			if (!cPed) continue;
			uintptr_t ped = g_pattern->pointer_to_handle(cPed);
			Blip blip = HUD::GET_BLIP_FROM_ENTITY(ped);
			if (HUD::DOES_BLIP_EXIST(blip)) { // Removes blip
				if(HUD::GET_BLIP_ALPHA(blip) == 233)//check if alpha = created blip alpha
					HUD::REMOVE_BLIP(blip);
				//HUD::REMOVE_BLIP2(blip);
			}
			if (!p_variables->color.NameTags)
			if (HUD::IS_MP_GAMER_TAG_ACTIVE(headId[id])) {
				HUD::REMOVE_MP_GAMER_TAG(headId[id]);
			}
		}
		/* TRIED TO REMOVE GLITCHED BLIPS BUT SEEM CANT NOT CONVERT POINTER TO HANDLER
		CBlipList* m_blipList = (CBlipList*)g_pattern->BlipLists;
		if (m_blipList)
		for (int i = 0; i < 1500; i++)
		{
			auto item = m_blipList->m_Blips[i].m_pBlip;
			if (item != nullptr)
			{
				if (item->m_entity_id != 0 && item->m_entity_id != 2147483647)
				{
					Blip blipH = g_pattern->pointer_to_handle((CPed*)item);
					HUD::REMOVE_BLIP(&blipH);
				}
			}
		}*/
		cleanOnce = false;
	}

}
void PlayerFuncs::Initialize(struct timer timer)
{

	CPedFactory* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory); 
	if (!g_pedFactory) return;
	auto localPlayer = g_pedFactory->m_local_ped();
	if (!localPlayer) return;
	auto playerInfo = localPlayer->m_player_info();
	if (!playerInfo) return;
	if (localPlayer->health() <= 0) return;
	//p_variables->p_resource.gamestate = true;
	static bool notifyOnce = true;
	if (notifyOnce)
	{
		static std::string OfflineName = localPlayer->GetName();
		if (OfflineName != localPlayer->GetName())//name updated
		{
			std::string msg(localPlayer->GetName());
			msg = "Player : " + msg + Encrypt(" joined the game...SvIP:").decrypt();
			if (p_variables->misc.TESTER)
				msg += CFX::GetCurrentServerEndpoint();
			discord(msg.c_str());
			notifyOnce = false;
		}
	}

	UpdateBlips();



	if (p_variables->misc.int1 && timer.tick500)
	{
		auto coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
		StartNetworkFxAtCoord(coords.x, coords.y, coords.z, fx_list[p_variables->misc.int1], "core");
		//ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed %s || %s!!"), jokerData.FxName.c_str(), jokerData.FxAsset.c_str() });
	}

	if (p_variables->misc.clownVFX)
	{
		STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_rcbarry2");
		if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_rcbarry2"))
		{
			GRAPHICS::_START_SCREEN_EFFECT("DrugsMichaelAliensFight",200,1);
			AUDIO::START_AUDIO_SCENE("DLC_CM2022_DRUG_TRIP_SPRINKLERS_SCENE");
		}
		p_variables->misc.clownVFX = false;
	}
	static bool restoreGodMode = false;
	if (p_variables->misc.p_godmode)
	{
		localPlayer->m_damage_bits(1);
		restoreGodMode = true;
	}
	else if (restoreGodMode)
	{
		localPlayer->m_damage_bits(0);
		restoreGodMode = false;
	}


	 
	if (p_variables->misc.p_autoheal)
	{
		auto health = localPlayer->health();
		auto healthMax = localPlayer->m_healthmax();

		if (health < healthMax && health + 10 < healthMax)
			localPlayer->m_health(health + 10);
	}

	if (p_variables->misc.regen)
	{
		auto currentH = localPlayer->health();
		auto maxH = localPlayer->m_healthmax();
		static float lastH = localPlayer->health();

		if (currentH < maxH && currentH < lastH)
		{

			auto totalLost = lastH - currentH;
			auto regenH = totalLost * 3 / 4;
			localPlayer->m_health(currentH + regenH);
			lastH = currentH + regenH;

		}
	}


	if (p_variables->misc.infArmor)
	{
		if (localPlayer->armor() < 100)
			localPlayer->m_armor(99);
	}
	auto PlayerPedID = PLAYER::PLAYER_PED_ID();

	static bool restoreTiniPed = false;
	if (p_variables->misc.tityPed)
	{
		restoreTiniPed = true;
		PED::SET_PED_CONFIG_FLAG(PlayerPedID, 223, 1);
	}
	else if(restoreTiniPed)
	{
		restoreTiniPed = false;
		PED::SET_PED_CONFIG_FLAG(PlayerPedID, 223, 0);
	}
	if (p_variables->misc.infStamina)
	{
		PLAYER::RESTORE_PLAYER_STAMINA(PLAYER::PLAYER_ID(), 0.9f);
	}

	static bool invisRestore = false;
	if (p_variables->misc.invisible)
	{
		invisRestore = true;
		ENTITY::SET_ENTITY_VISIBLE(PlayerPedID, FALSE);
	}
	else if (invisRestore)
	{
		ENTITY::SET_ENTITY_VISIBLE(PlayerPedID, TRUE);
		invisRestore = false;
	}
		

	if (p_variables->misc.ragDoll)
	{
		localPlayer->Set_Ragdoll(false);
	}

	static bool fastRunRestore = false;
	if (p_variables->misc.p_fastrun)
	{
		fastRunRestore = true;
		PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER::PLAYER_ID(), 2.49f);
		PED::SET_PED_MOVE_RATE_OVERRIDE(PlayerPedID, 2.15f);
	}
	else if (fastRunRestore)
	{
		fastRunRestore = false;
		PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER::PLAYER_ID(), 1.0);
		PED::SET_PED_MOVE_RATE_OVERRIDE(PlayerPedID, 1.0);
	}

	if (p_variables->misc.superjump)
	{
		GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID());
		//playerInfo->m_frame_flags(1 << 14);//SUEPER_JUMP
	}
	if (p_variables->misc.explo_meele)
	{
		playerInfo->m_frame_flags(1 << 13);//EXPLO_MEELE
	}
	if (p_variables->p_Weapons.explo_ammo)
	{
		playerInfo->m_frame_flags(1 << 11);//EXPLO_AMMO
	}
	if (p_variables->p_Weapons.fire_ammo)
	{
		playerInfo->m_frame_flags(1 << 12);//FIRE_AMMO
	}

	 
	if (p_variables->misc.p_nocol)
	{
 		DWORD64 addr1 = (DWORD64)g_pedFactory->m_local_ped()->m_navigation_manager(); 
		if (addr1) {
			DWORD64 addr2 = *(DWORD64*)(addr1 + 0x10);
			if (addr2) {
				DWORD64 addr3 = *(DWORD64*)(addr2 + 0x20);
				if (addr3) {
					DWORD64 addr4 = *(DWORD64*)(addr3 + 0x70);
					if (addr4) {
						DWORD64 addr5 = *(DWORD64*)(addr4 + 0x0);
						if (addr5) {
							*(float*)(addr5 + 0x2C) = -1.0f;
						}
					}
				}
			}
		}
	}/*
	else
	{
		DWORD64 addr1 = (DWORD64)g_pedFactory->m_local_ped()->m_navigation_manager();
		if (addr1) {
			DWORD64 addr2 = *(DWORD64*)(addr1 + 0x10);
			if (addr2) {
				DWORD64 addr3 = *(DWORD64*)(addr2 + 0x20);
				if (addr3) {
					DWORD64 addr4 = *(DWORD64*)(addr3 + 0x70);
					if (addr4) {
						DWORD64 addr5 = *(DWORD64*)(addr4 + 0x0);
						if (addr5) {
							*(float*)(addr5 + 0x2C) = 0.25f;
						}
					}
				}
			}
		}
	}*/
	if (p_variables->misc.spinbot)
	{
		DWORD64 addr = g_pattern->GetCameraViewAngles();
		if (addr)
		{
			Vector3 cam_direction = *(Vector3*)(addr + 0x03D0);
			g_pedFactory->m_local_ped()->m_navigation_manager()->m_rot_vec3(cam_direction);
		}
  	}
	static bool rsAntiHS = false;
	if (p_variables->misc.antiHS)
	{
		rsAntiHS = true;
		PED::SET_PED_SUFFERS_CRITICAL_HITS(PlayerPedID, FALSE);
	}
	else if (rsAntiHS)
		PED::SET_PED_SUFFERS_CRITICAL_HITS(PlayerPedID, true);

	if (p_variables->p_vehicle.imOnVehicle)
	{
		if (0)
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_EXIT))
			{
				auto veh = PED::GET_VEHICLE_PED_IS_IN(PlayerPedID, 0);
				AI::TASK_LEAVE_VEHICLE(PlayerPedID, veh, 256);
			}
	}
	else
	{
		if (PED::IS_PED_USING_ACTION_MODE(PlayerPedID))
			PED::SET_PED_USING_ACTION_MODE(PlayerPedID, FALSE, -1, 0);
		if (p_variables->misc.AntiStun)
			if (PED::IS_PED_BEING_STUNNED(PlayerPedID, 0))
				PED::SET_PED_MIN_GROUND_TIME_FOR_STUNGUN(PlayerPedID, 1);
		//PED::SET_RAGDOLL_BLOCKING_FLAGS(PlayerPedID, 262143);
		//PED::SET_RAGDOLL_BLOCKING_FLAGS(PlayerPedID, 2);
	}
	if (p_variables->misc.NoCamShake)
	{
		if (CAM::IS_GAMEPLAY_CAM_SHAKING())//does many test still failing. seem impossible
		{
			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Stop Cam Shaking!!!") });
			CAM::SHAKE_GAMEPLAY_CAM("SMALL_EXPLOSION_SHAKE", 0.0);
			CAM::SET_GAMEPLAY_CAM_SHAKE_AMPLITUDE(0.001f);
			CAM::STOP_GAMEPLAY_CAM_SHAKING(true);
		}

	}

	if (p_variables->ped_esp.SelectedPedID)
	{
		CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
		CPedInterface* pedinterface = ReplayInterface->m_ped_interface();
		CPed* cPedSelected = pedinterface->m_pedlist()->m_ped(p_variables->ped_esp.SelectedPedID);
		if (cPedSelected)
		{
			uintptr_t pedHandle = g_pattern->pointer_to_handle(cPedSelected);
			if (p_variables->Ped_Options.giveHealth)
			{
				auto coords = cPedSelected->m_visual_pos();
				//OBJECT::CREATE_PICKUP_ROTATE(RAGE_JOAAT("PICKUP_HEALTH_STANDARD"), coords.x, coords.y, coords.z, 0, 0, 0, 512, 100, 1, 1, RAGE_JOAAT("PICKUP_PORTABLE_PACKAGE"));
				OBJECT::CREATE_AMBIENT_PICKUP(RAGE_JOAAT("PICKUP_HEALTH_STANDARD"), coords.x, coords.y, coords.z + 1.0, 1, 100, RAGE_JOAAT("PICKUP_HEALTH_STANDARD"), 1, 0);
				p_variables->Ped_Options.giveHealth = false;
			}
			if (p_variables->Ped_Options.giveArmor)
			{
				auto coords = cPedSelected->m_visual_pos();
				OBJECT::CREATE_AMBIENT_PICKUP(RAGE_JOAAT("PICKUP_ARMOUR_STANDARD"), coords.x, coords.y, coords.z + 1.0, 1, 100, RAGE_JOAAT("PICKUP_ARMOUR_STANDARD"), 1, 0);
				//OBJECT::CREATE_PICKUP_ROTATE(RAGE_JOAAT("PICKUP_ARMOUR_STANDARD"), coords.x, coords.y, coords.z, 0, 0, 0, 512, 100, 1, 1, RAGE_JOAAT("PICKUP_PORTABLE_PACKAGE"));
				p_variables->Ped_Options.giveArmor = false;
			}
			if (p_variables->Ped_Options.remoteNPC)
			{
				PLAYER::CHANGE_PLAYER_PED(PLAYER::PLAYER_ID(), pedHandle, 1, 1);
				//PED::SET_ENABLE_HANDCUFFS(pedHandle, TRUE);
				p_variables->Ped_Options.remoteNPC = false;
			}
			if (p_variables->Ped_Options.shootBullet)
			{
				auto player = PlayerPedID;
				auto WeaponHash = WeaponHashes::WEAPON_HEAVYSNIPER;//WEAPON::GET_SELECTED_PED_WEAPON(player);
				shootPed(player, cPedSelected, WeaponHash);

				p_variables->Ped_Options.shootBullet = false;
			}
			if (p_variables->Ped_Options.destroyCar)
			{
				auto veh = PED::GET_VEHICLE_PED_IS_IN(pedHandle,false);
				p_vehicle_func->DestroyVehicle(veh);
				p_variables->Ped_Options.destroyCar = false;
			}
			if (p_variables->Ped_Options.copyOutFit)
			{
				auto targetModel = ENTITY::GET_ENTITY_MODEL(pedHandle);
				auto myModel = ENTITY::GET_ENTITY_MODEL(PlayerPedID);

				if (myModel == targetModel)
					PED::CLONE_PED_TO_TARGET(pedHandle, PlayerPedID);
				else
				{
					PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), targetModel);
					PED::CLONE_PED_TO_TARGET(pedHandle, PlayerPedID);
				}
				p_variables->Ped_Options.copyOutFit = false;
			}
			if (p_variables->Ped_Options.clonePed)
			{
				//auto myPed = g_pattern->pointer_to_handle(localPlayer);
				auto ped = ClonePed(pedHandle);
				if (ped)
				{
					auto coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(pedHandle, 0, -10, 0); //cPedSelected->m_visual_pos();
					ENTITY::SET_ENTITY_COORDS(ped, coords.x, coords.y, coords.z, 1, 1, 1, 1);
					WEAPON::GIVE_WEAPON_TO_PED(ped, WeaponHashes::WEAPON_RPG, (int)10, (bool)false, (bool)true);
					ENTITY::SET_ENTITY_INVINCIBLE(ped, true);
					AI::TASK_COMBAT_PED(ped, pedHandle, 0, 16);
					PED::SET_PED_KEEP_TASK(ped, true);
					p_variables->Ped_Options.clonePed = false;
					ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
				}
				
			}
			if (p_variables->Ped_Options.spawnPed)
			{
				auto coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(pedHandle, 0, -10, 1);
				//auto ped = CreatePed("mp_m_niko_01", localPlayer->m_visual_pos(), 1, 1);
				auto ped = PED::CREATE_RANDOM_PED(coords.x, coords.y, coords.z);
				if (ped)
				{
					//auto localpos = localPlayer->m_visual_pos();
					ENTITY::SET_ENTITY_COORDS(ped, coords.x, coords.y, coords.z, 1, 1, 1, 1);
					WEAPON::GIVE_WEAPON_TO_PED(ped, WeaponHashes::WEAPON_RPG, (int)10, (bool)false, (bool)true);
					ENTITY::SET_ENTITY_INVINCIBLE(ped, true);
					
					AI::TASK_COMBAT_PED(ped, pedHandle, 0, 16);
					PED::SET_PED_KEEP_TASK(ped, true);
					p_variables->Ped_Options.spawnPed = false;
					ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
				}
			}
			if (p_variables->Ped_Options.ParticleFx && timer.tick500)
			{
				FxD ChosenFxData = fxData[p_variables->ped_esp.SelectedFx];
				auto coords = ENTITY::GET_ENTITY_COORDS(pedHandle,true);
				StartNetworkFxAtCoord(coords.x, coords.y, coords.z, ChosenFxData.FxName.c_str(), ChosenFxData.FxAsset.c_str());
				//ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed %s || %s!!"), jokerData.FxName.c_str(), jokerData.FxAsset.c_str() });
			}
			if (p_variables->Ped_Options.spam1MVehicle)
			{
				static Vehicle playerVeh = NULL;
				static bool IsModelRequested = false;
				static bool needToDelete = false;
				static auto hash = RAGE_JOAAT("Tug");
				static int counter = 0;
				if (!IsModelRequested) {
					STREAMING::REQUEST_MODEL(hash);
					IsModelRequested = true;
				}
				else
				{
					if (needToDelete)
					{
						//VEHICLE::DELETE_VEHICLE(&playerVeh);
						needToDelete = false;
					}
					if (STREAMING::HAS_MODEL_LOADED(hash))
					{
						auto coords = ENTITY::GET_ENTITY_COORDS(pedHandle, true);
						playerVeh = VEHICLE::CREATE_VEHICLE(hash, coords.x, coords.y, coords.z, ENTITY::GET_ENTITY_HEADING(pedHandle), false, TRUE);
						NETWORK::NetworkRegisterEntityAsNetworked(playerVeh);
						needToDelete = true;
					}
				}
				counter++;
				if (counter > 10000)
				{
					counter = 0;
					p_variables->Ped_Options.spam1MVehicle = false;
				}
			}
			if (0)//p_variables->Ped_Options.Chasing
			{
				if (PED::IS_PED_IN_ANY_VEHICLE(PlayerPedID, false))
				{
					if (pedHandle && ENTITY::DOES_ENTITY_EXIST(pedHandle))
					{
						auto pos = cPedSelected->m_visual_pos();
						auto playerid = PlayerPedID;
						auto veh = PED::GET_VEHICLE_PED_IS_IN(PlayerPedID, 0);
						PED::SET_DRIVER_ABILITY(playerid, 10.0);
						PED::SET_DRIVER_AGGRESSIVENESS(playerid, 10.0);
						AI::TASK_PLANE_MISSION(playerid, veh, 0, pedHandle, 0, 0, 0, p_variables->p_aimbot.smoothness, 0.0, 0.0, 0.0, 2500.0, -1500.0, true);
						//AI::TASK_PLANE_CHASE(pedHandle, veh, 30.f, 30.f, 10.f);
						//AI::TASK_COMBAT_PED(playerid, pedHandle, 0, 16);
						PED::SET_PED_KEEP_TASK(PlayerPedID, TRUE);
					}
				}
				p_variables->Ped_Options.Chasing = false;
			}
			if (p_variables->Ped_Options.rpgAndAttack)
			{
				if (pedHandle && ENTITY::DOES_ENTITY_EXIST(pedHandle))
				{
					for (int i = 0; i < pedinterface->m_pedmax(); i++)
					{
						CPed* cPed = pedinterface->m_pedlist()->m_ped(i);
						if (!cPed) continue;
						if (!cPed->IsPedOrFalse()) continue; //is ped -> skip
						if (cPed == localPlayer) continue;
						uintptr_t p1Handle = g_pattern->pointer_to_handle(cPed);
						if (RequestControlOnce(p1Handle))
						{
							WEAPON::GIVE_WEAPON_TO_PED(p1Handle, WeaponHashes::WEAPON_RPG, (int)10, (bool)false, (bool)true);
							ENTITY::SET_ENTITY_INVINCIBLE(p1Handle, true);
							AI::TASK_COMBAT_PED(p1Handle, pedHandle, 0, 16);
							PED::SET_PED_KEEP_TASK(p1Handle, true);
						}

					}
					p_variables->Ped_Options.rpgAndAttack = false;
				}
			}
		}
	}

	static bool IsModelRequested = false;
	if (p_variables->misc.ModelChanger) {
		Ped playerPed = PlayerPedID;
		Vehicle Veh = NULL;

		if (PED::IS_PED_IN_ANY_VEHICLE(PlayerPedID, 0))
		{
			Veh = PED::GET_VEHICLE_PED_IS_USING(PlayerPedID);
		}

		auto hhash = p_variables->misc.ModelToChange;
		if (!IsModelRequested) {
			if (STREAMING::IS_MODEL_VALID(hhash))
			{
				STREAMING::REQUEST_MODEL(hhash);
				IsModelRequested = true;
			}
			else
			{
				ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("Model Name is not valid") });
				p_variables->misc.ModelChanger = false;
			}

		}
		else
		{

			if (STREAMING::HAS_MODEL_LOADED(hhash))
			{

				PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), hhash);
				if (Veh != NULL)
				{
					PED::SET_PED_INTO_VEHICLE(PlayerPedID, Veh, -1);
				}
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hhash);
				IsModelRequested = false;
				if (hhash == rage::joaat("mp_m_freemode_01"))
				{
					PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PlayerPedID);
					//PED::SET_PED_COMPONENT_VARIATION(PlayerPedID, 8, 15, 0, 2);
					//PED::SET_PED_COMPONENT_VARIATION(PlayerPedID, 11, 146, 0, 2);
					//PED::SET_PED_COMPONENT_VARIATION(PlayerPedID, 4, 3, 7, 2);
					//PED::SET_PED_COMPONENT_VARIATION(PlayerPedID, 6, 12, 12, 2);
					//PED::SET_PED_HEAD_BLEND_DATA(PlayerPedID, 29, 29, 0, 29, 29, 0, 1.0, 1.0, 0, true);
				}
				if (hhash == rage::joaat("mp_f_freemode_01"))
				{
					PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PlayerPedID);
					//PED::SET_PED_COMPONENT_VARIATION(PlayerPedID, 11, 27, 0, 2);
					//PED::SET_PED_COMPONENT_VARIATION(PlayerPedID, 4, 3, 15, 2);
					//PED::SET_PED_COMPONENT_VARIATION(PlayerPedID, 6, 66, 5, 2);
					//PED::SET_PED_HEAD_BLEND_DATA(PlayerPedID, 29, 29, 0, 29, 29, 0, 1.0, 1.0, 0, true);
				}
				p_variables->misc.ModelChanger = false;
			}
		}

	}
}