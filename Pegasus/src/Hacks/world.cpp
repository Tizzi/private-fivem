#include <World.h>
#include <Player.h>
#include <Vehicle.h>

static PVector3 location;
static PVector3 other;
float dist = 10;
static double multiplier = 3.0;

void DoNetwork(int netID) {

	NETWORK::NetworkSetNetworkIdDynamic(netID, true);
	NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(netID, true);
	NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netID, false);

	// Sync the network ID to all players in the PlayerCache
	//for (const auto& src : PlayerCache)
	//	NETWORK::SetNetworkIdSyncToPlayer(netID, src, true);
	
}
inline void apply_velocity(Entity e, Cam cam)
{

	if (ENTITY::DOES_ENTITY_EXIST(e))
	{
		//RequestControlOnce(e);
		if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_WEAPON_WHEEL_NEXT))
			dist -= 5;
		if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_WEAPON_WHEEL_PREV))
			dist += 5;

		//if (!entity::take_control_of(e))
		//    return; // TODO: remove from vector


		other = ENTITY::GET_ENTITY_COORDS(e, true);

		PVector3 rot = CAM::GET_CAM_ROT(cam, 2);
		float pitch = g_Math->degToRad(rot.x); // vertical
		// float roll = rot.y;
		float yaw = g_Math->degToRad(rot.z + 90); // horizontal

		PVector3 velocity = PVector3(0, 0, 0);

		velocity.x = location.x + (dist * cos(pitch) * cos(yaw)) - other.x;
		velocity.y = location.y + (dist * sin(yaw) * cos(pitch)) - other.y;
		velocity.z = location.z + (dist * sin(pitch)) - other.z;

		ENTITY::SET_ENTITY_VELOCITY(e, velocity.x * (float)multiplier, velocity.y * (float)multiplier, velocity.z * (float)multiplier);

	}
}
inline void WorldEntityLoops()
{
	CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
	CPed* LocalPlayer = ped_factory->m_local_ped();
	if (!LocalPlayer) return;
	CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
	CPedInterface* pedinterface = ReplayInterface->m_ped_interface();
	CVehicleInterface* Vehinterface = ReplayInterface->m_vehicleinterface();
	static int NextProcessingPed = 0;
	static int NextProcessingCar = 0;

	for (int id = NextProcessingCar; id < Vehinterface->m_vehiclemax(); id++) {
		NextProcessingCar = id + 1;
		if (id > 254)NextProcessingCar = 0;

		auto cVeh = Vehinterface->m_vehiclelist()->m_getvehicle(id);
		if (!cVeh) continue;
		if (!p_variables->world.includePed && !cVeh->m_player_info())continue;
		if (cVeh == LocalPlayer->m_vehicle_manager()) continue;
		int handle = g_pattern->pointer_to_handle((CPed*)cVeh);
		if (!p_variables->world.DeleteCar && VEHICLE::GET_VEHICLE_ENGINE_HEALTH(handle) <= 0)continue;
		p_vehicle_func->DestroyVehicle(handle);
		break;
	}

	for (int id = NextProcessingPed; id < pedinterface->m_pedmax(); id++) {
		NextProcessingPed = id + 1;
		if (id > 254)NextProcessingPed = 0;

		CPed* cPed = (CPed*)pedinterface->m_pedlist()->m_ped(id);
		if (!cPed) continue;
		if (!p_variables->world.includePed && cPed->IsPedOrFalse())continue;
		if (cPed == LocalPlayer) continue;
		if (!cPed->health())continue;
		int handle = g_pattern->pointer_to_handle(cPed);
		//kick all from vehicle
		if (p_variables->world.TpAllToYou) 
		{
			auto LocalPos = LocalPlayer->m_visual_pos();
			AI::CLEAR_PED_TASKS_IMMEDIATELY(handle);
			int netScene = NETWORK::NETWORK_CREATE_SYNCHRONISED_SCENE(LocalPos.x, LocalPos.y, LocalPos.z, 0.f, 0.f, 0.f, 0, 0, 0, 0.f, 0.f, 0.f);
			NETWORK::NETWORK_ADD_PED_TO_SYNCHRONISED_SCENE(handle, netScene, "amb@code_human_cower@female@base", "base", 8.f, -8.f, 2, 0, 100.f, 0);
			NETWORK::NETWORK_START_SYNCHRONISED_SCENE(netScene);
		}
		if (p_variables->world.ShootBullets || p_variables->world.TazerPlayer || p_variables->world.BurnPlayer)
		{
			auto player = PLAYER::PLAYER_PED_ID();
			auto WeaponHash = p_variables->world.WeaponHash;
			if (p_variables->world.TazerPlayer)WeaponHash = WeaponHashes::WEAPON_STUNGUN;
			if (p_variables->world.BurnPlayer)WeaponHash = WeaponHashes::WEAPON_MOLOTOV;
			if(WeaponHash)
				shootPed(player, cPed, WeaponHash);
		}
		if (p_variables->world.silentKill)
		{
			auto Pos = cPed->m_visual_pos();
			FIRE::ADD_EXPLOSION(Pos.x, Pos.y, Pos.z, 26, FLT_MAX, false, true, 0.0);
		}
		if (p_variables->world.ParticleFx) {
			auto coords = cPed->m_visual_pos();
			FxD ChosenFxData = fxData[p_variables->ped_esp.SelectedFx];
			StartNetworkFxAtCoord(coords.x, coords.y, coords.z, ChosenFxData.FxName.c_str(), ChosenFxData.FxAsset.c_str());
		}
		if (p_variables->world.DeletePed)
			PED::DELETE_PED(&handle);
		break;
	}
	
}
void World_Func::Initialize(struct timer timer)
{
	bool ontick = timer.tick100;
	if (timer.tick200 && p_variables->world.AffectAll)WorldEntityLoops();

	//free cam
	static bool CamCreated = false;

	static Cam camera = -1;
	static int camBlip = -1;
	static Vehicle Veh = NULL;

	static float speedd = 0.5f;
	static float mult = 0.f;
	static PVector3 position;
	static PVector3 rotation;
	static const ControllerInputs controls[] = { ControllerInputs::INPUT_LOOK_LR, ControllerInputs::INPUT_LOOK_UD, ControllerInputs::INPUT_LOOK_UP_ONLY, ControllerInputs::INPUT_LOOK_DOWN_ONLY, ControllerInputs::INPUT_LOOK_LEFT_ONLY, ControllerInputs::INPUT_LOOK_RIGHT_ONLY, ControllerInputs::INPUT_LOOK_LEFT, ControllerInputs::INPUT_LOOK_RIGHT, ControllerInputs::INPUT_LOOK_UP, ControllerInputs::INPUT_LOOK_DOWN };

	if (ontick && !p_variables->p_menu.open && SAFE_CALL(GetAsyncKeyState)(p_variables->misc.p_flyKey) & 1)
	{
		p_variables->misc.freecam = !p_variables->misc.freecam;
	}
	auto playerPedID = PLAYER::PLAYER_PED_ID();
	if (p_variables->misc.freecam)
	{
		
		if (!CamCreated) {
			camera = CAM::CREATE_CAM((char*)"DEFAULT_SCRIPTED_CAMERA", 1);

			position = CAM::GET_GAMEPLAY_CAM_COORD();
			rotation = CAM::GET_GAMEPLAY_CAM_ROT(2);
			HUD::SET_RADAR_ZOOM_PRECISE(90.0);

			camBlip = HUD::ADD_BLIP_FOR_COORD(position.x, position.y, position.z);
			HUD::SET_BLIP_SPRITE(camBlip, 185);

			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Created cam %d   "),camera });
			if (PED::IS_PED_IN_ANY_VEHICLE(playerPedID, 0))
			{
				Veh = PED::GET_VEHICLE_PED_IS_USING(playerPedID);
				ENTITY::FREEZE_ENTITY_POSITION(Veh, true);
			}else
				ENTITY::FREEZE_ENTITY_POSITION(playerPedID, true);
			if (p_variables->misc.noclip)
			{
				if (PED::IS_PED_IN_ANY_VEHICLE(playerPedID, 0))
				{
					ENTITY::SET_ENTITY_ALPHA(playerPedID, 100, 0);
					ENTITY::SET_ENTITY_ALPHA(Veh, 100, 0);
					ENTITY::SET_ENTITY_COLLISION(Veh, false, 0);
				}
				else
				{
					ENTITY::SET_ENTITY_ALPHA(playerPedID, 100, 0);
					ENTITY::FREEZE_ENTITY_POSITION(playerPedID, true);
					ENTITY::SET_ENTITY_COLLISION(playerPedID, false, 0);
				}

			}

			CAM::SET_CAM_COORD(camera, position.x, position.y, position.z);
			CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 2);
			CAM::SET_CAM_ACTIVE(camera, true);
			CAM::RENDER_SCRIPT_CAMS(true, true, 500, true, true);
			CamCreated = true;
		}
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);

		for (const auto& control : controls)
			CONTROLS::ENABLE_CONTROL_ACTION(0, static_cast<int>(control), true);




		Vector3 vecChange = { 0.f, 0.f, 0.f };
		static bool onRcCar = false;
		static Entity RcCar = NULL;
		if (p_variables->world.RcCar && ontick)
		{

			if (RcCar)
			{
				//auto vehrot = ENTITY::GET_ENTITY_ROTATION(RcCar,5);

				AI::_CLEAR_VEHICLE_TASKS(RcCar);
				//CAM::POINT_CAM_AT_ENTITY(camera, RcCar, 0, 0, 1.5f, 1);
				CAM::ATTACH_CAM_TO_ENTITY(camera, RcCar, 0, 0, 2, 1);
				//CAM::SET_CAM_ROT(RcCar, vehrot.x, vehrot.y, vehrot.z, 5);
				// horn
				if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HORN))
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(RcCar, 200.0);

				// Forward
				if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_UP_ONLY))
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(RcCar, 10.0);
				//AI::TASK_VEHICLE_TEMP_ACTION(playerPedID, RcCar, 23, 100);
			// Backward
				if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_DOWN_ONLY))
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(RcCar, -10.0);
				//AI::TASK_VEHICLE_TEMP_ACTION(playerPedID, RcCar, 28, 100);
			// Left
				if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_LEFT_ONLY))
					AI::TASK_VEHICLE_TEMP_ACTION(playerPedID, RcCar, 7, 100);
				// Right
				if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_RIGHT_ONLY))
					AI::TASK_VEHICLE_TEMP_ACTION(playerPedID, RcCar, 8, 100);
			}
			else
			{
				//CAM::STOP_CAM_POINTING(camera);
				CAM::DETACH_CAM(camera);
			}

			//CAM::STOP_CAM_POINTING(camera);
		}
		if (!RcCar)
		{

			// Left Shift
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_JUMP))
				vecChange.z += speedd / 2;
			// Left Control
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_DUCK))
				vecChange.z -= speedd / 2;
			// Forward
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_UP_ONLY))
				vecChange.y += speedd;
			// Backward
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_DOWN_ONLY))
				vecChange.y -= speedd;
			// Left
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_LEFT_ONLY))
				vecChange.x -= speedd;
			// Right
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_RIGHT_ONLY))
				vecChange.x += speedd;
		}

		if (vecChange.x == 0.f && vecChange.y == 0.f && vecChange.z == 0.f)
			mult = 0.f;
		else if (mult < 10)
			mult += 0.15f;

		PVector3 rot = CAM::GET_CAM_ROT(camera, 2);
		short leftMouseState = CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK);
		static Object currentObject = 0;
		PVector3 dirrr = g_Math->Prot_to_direction(rot);
		PVector3 OBJCoords = PVector3(0, 0, 0);
		static int farMulti = 10;
		OBJCoords.x = position.x + dirrr.x * farMulti;
		OBJCoords.y = position.y + dirrr.y * farMulti;
		OBJCoords.z = position.z + dirrr.z * farMulti;

		if (p_variables->world.objectSpawner)
		{
			static bool IsModelRequested1 = false;
			static int idx = 0;
			static bool isSpawned = false;
			auto hhash = rage::joaat(object_list[idx]);
			static int maxObj = IM_ARRAYSIZE(object_list) - 1;


			if (ontick && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_FRONTEND_LB))
			{
				ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Previous < %s"), object_list[idx] });

				idx -= 1;
				if (idx < 0)idx = maxObj;
				OBJECT::DELETE_OBJECT(&currentObject);
				currentObject = NULL;
				IsModelRequested1 = false;
			}

			if (ontick && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_FRONTEND_RB))
			{
				ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Next > %s"), object_list[idx] });

				idx += 1;
				if (idx > maxObj) idx = 0;
				OBJECT::DELETE_OBJECT(&currentObject);
				currentObject = NULL;
				IsModelRequested1 = false;
			}

			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_WEAPON_WHEEL_PREV))
				farMulti += 5;
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_WEAPON_WHEEL_NEXT))
				farMulti -= 5;
			if (leftMouseState && currentObject)
			{
				if (ontick)
				{
					auto placeObject = OBJECT::CREATE_OBJECT(hhash, OBJCoords.x, OBJCoords.y, OBJCoords.z + rot.x / 3, true, true, true);
					ENTITY::SET_ENTITY_COORDS(placeObject, OBJCoords.x, OBJCoords.y, OBJCoords.z + rot.x / 3, 1, 1, 1, 1);
					ENTITY::SET_ENTITY_ROTATION(placeObject, 0, rot.y, rot.z + 90.0f, 2, 1);
					auto netID = NETWORK::OBJ_TO_NET(placeObject);
					if (netID) DoNetwork(netID);
					//ENTITY::SET_ENTITY_ALPHA(currentObject, 255, 0);
					//ENTITY::SET_ENTITY_COLLISION(currentObject, true, true);
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hhash);
				}
			}
			if (ontick && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
			{
				p_variables->world.objectSpawner = false;
				IsModelRequested1 = false;
				OBJECT::DELETE_OBJECT(&currentObject);
			}
			if (!currentObject)//spawn new object
			{
				if (!IsModelRequested1) {
					STREAMING::REQUEST_MODEL(hhash);
					//ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("REQUEST_MODEL !!") });
					IsModelRequested1 = true;
				}
				//ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("HAS_MODEL_LOADED !!") });
				else if (STREAMING::HAS_MODEL_LOADED(hhash) && ontick)
				{
					//ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("CREATE_OBJECT %s%d!!"),object_list[idx],idx });
					currentObject = OBJECT::CREATE_OBJECT(hhash, OBJCoords.x, OBJCoords.y, OBJCoords.z + rot.x / 3, false, true, true);
					
					ENTITY::SET_ENTITY_ALPHA(currentObject, 200, 0);
					ENTITY::SET_ENTITY_COLLISION(currentObject, false, false);
					
					farMulti = 10;
					//isSpawned = true;
				}
			}
			else
			{
				ENTITY::SET_ENTITY_COORDS(currentObject, OBJCoords.x, OBJCoords.y, OBJCoords.z + rot.x / 3, 1, 1, 1, 1);
				ENTITY::SET_ENTITY_ROTATION(currentObject, 0, rot.y, rot.z + 90.0f, 2, 1);
			}

		}
		if (leftMouseState)
		{


			PVector3 fffarCoords = PVector3(0, 0, 0);

			fffarCoords.x = position.x + dirrr.x * 1000;
			fffarCoords.y = position.y + dirrr.y * 1000;
			fffarCoords.z = position.z + dirrr.z * 1000;
			Entity entityCatch = 0;
			bool catched = g_Math->raycast(&entityCatch, camera);
			auto weaponHash = p_variables->world.WeaponHash;
			if (p_variables->world.ShootBullets && weaponHash && ontick)
			{
				WEAPON::REQUEST_WEAPON_ASSET(weaponHash, 31, 0);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS((position.x), (position.y), (position.z), (fffarCoords.x), (fffarCoords.y), (fffarCoords.z), (int)1000, false, weaponHash, playerPedID, true, false, 500.f);
			}

			if (ontick && p_variables->world.objectEraser)
			{
				if (ENTITY::IS_ENTITY_AN_OBJECT(entityCatch))
				{
					if (RequestControlOnce(entityCatch))
						ENTITY::DELETE_ENTITY(&entityCatch);
				}

			}
			
			if (p_variables->world.breakDoors || p_variables->world.breakWheels || p_variables->world.burstTires || p_variables->world.destroyEngine ||
				p_variables->world.washVehicle || p_variables->world.setAlarm || p_variables->world.smashVehicle || p_variables->world.ExplodeCar)
			{
				if (ontick)
					p_vehicle_func->DestroyVehicle(entityCatch);
			}
			if (p_variables->world.TazerPlayer && ontick)
			{
				weaponHash = WeaponHashes::WEAPON_STUNGUN;
				auto coords = ENTITY::GET_ENTITY_COORDS(entityCatch, true);

				WEAPON::REQUEST_WEAPON_ASSET(weaponHash, 31, 0);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(coords.x, coords.y, coords.z + 3.0f, coords.x, coords.y, coords.z, (int)10, false, weaponHash, playerPedID, false, true, 200.f);
			}
			if (p_variables->world.BurnPlayer && ontick)
			{
				weaponHash = WeaponHashes::WEAPON_MOLOTOV;
				auto coords = ENTITY::GET_ENTITY_COORDS(entityCatch, true);

				WEAPON::REQUEST_WEAPON_ASSET(weaponHash, 31, 0);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS((coords.x + 0.07f), (coords.y + 0.07f), (coords.z + 0.07f), (coords.x + 0.09f), (coords.y + 0.09f), (coords.z + 0.09f), (int)10, false, weaponHash, playerPedID, false, true, 200.f);
			}
			if (p_variables->world.DropHnArmor && ontick)
			{
				auto coords = ENTITY::GET_ENTITY_COORDS(entityCatch, true);
				//OBJECT::CREATE_PICKUP_ROTATE(RAGE_JOAAT("PICKUP_ARMOUR_STANDARD"), coords.x, coords.y, coords.z,0,0,0,512, 100,1,1, RAGE_JOAAT("PICKUP_PORTABLE_PACKAGE"));
				//OBJECT::CREATE_PICKUP_ROTATE(RAGE_JOAAT("PICKUP_HEALTH_STANDARD"), coords.x, coords.y, coords.z, 0, 0, 0, 512, 100, 1, 1, RAGE_JOAAT("PICKUP_PORTABLE_PACKAGE"));
				//auto pickup1 = OBJECT::CREATE_PORTABLE_PICKUP(RAGE_JOAAT("PICKUP_ARMOUR_STANDARD"), coords.x, coords.y, coords.z, 1, RAGE_JOAAT("PICKUP_PORTABLE_PACKAGE"));
				//auto pickup2 = OBJECT::CREATE_PORTABLE_PICKUP(RAGE_JOAAT("PICKUP_HEALTH_STANDARD"), coords.x, coords.y, coords.z, 1, RAGE_JOAAT("PICKUP_PORTABLE_PACKAGE"));

				OBJECT::CREATE_AMBIENT_PICKUP(RAGE_JOAAT("PICKUP_ARMOUR_STANDARD"), coords.x, coords.y, coords.z + 1.0, 1, 100, RAGE_JOAAT("PICKUP_ARMOUR_STANDARD"), 1, 0);
				OBJECT::CREATE_AMBIENT_PICKUP(RAGE_JOAAT("PICKUP_HEALTH_STANDARD"), coords.x, coords.y, coords.z + 1.0, 1, 100, RAGE_JOAAT("PICKUP_HEALTH_STANDARD"), 1, 0);
				//OBJECT::ATTACH_PORTABLE_PICKUP_TO_PED(entityCatch, OBJECT::GetPickupObject(pickup1));
				//OBJECT::ATTACH_PORTABLE_PICKUP_TO_PED(OBJECT::GetPickupObject(pickup1), entityCatch);
				//OBJECT::SET_PICKUP_REGENERATION_TIME(pickup1, 1000);
				//OBJECT::SET_PICKUP_REGENERATION_TIME(pickup2, 1000);
			}


			if (p_variables->world.DropCars && ontick && entityCatch)
			{
				auto coords = ENTITY::GET_ENTITY_COORDS(entityCatch, true);

				CReplayInterface* ReplayInterface = (CReplayInterface*)*(DWORD64*)(g_pattern->g_replayInterface);

				CVehicleInterface* VehicleInterface = ReplayInterface->m_vehicleinterface();


				for (int index = 0; index < VehicleInterface->m_vehiclemax(); index++)
				{
					CVehicleManager* Vehicle = VehicleInterface->m_vehiclelist()->m_getvehicle(index);
					if (!Vehicle) continue;
					auto vehHandle = g_pattern->pointer_to_handle((CPed*)Vehicle);
					ENTITY::SET_ENTITY_COORDS(vehHandle, coords.x, coords.y, coords.z + 3.0f, 1, 1, 1, 1);
				}
			}
			if (p_variables->world.ExplodeCar && catched && entityCatch && ontick)
			{
				static bool ondeleting = false;
				static bool requestmodelonce = true;
				static Vehicle playerVeh = -1;
				auto coords = ENTITY::GET_ENTITY_COORDS(entityCatch, true);
				if (requestmodelonce)
				{
					STREAMING::REQUEST_MODEL(0xB779A091);
					requestmodelonce = false;
				}

				if (STREAMING::HAS_MODEL_LOADED(0xB779A091) && !ondeleting)
				{
					//auto model = ENTITY::GET_ENTITY_MODEL(entityCatch);
					playerVeh = VEHICLE::CREATE_VEHICLE(0xB779A091, coords.x, coords.y, coords.z + 0.5f, 0, FALSE, TRUE);
					//ENTITY::SET_ENTITY_COLLISION(playerVeh, false, false);
					NETWORK::NETWORK_EXPLODE_VEHICLE(playerVeh, 1, 1, 0);
					ondeleting = true;
				}
				else
				{
					if (ENTITY::DOES_ENTITY_EXIST(playerVeh))
					{

						ENTITY::DELETE_ENTITY(&playerVeh);
					}
					ondeleting = false;
				}
			}
			if (p_variables->world.RcCar && ENTITY::IS_ENTITY_A_VEHICLE(entityCatch) && ontick)
			{
				RcCar = entityCatch;
				RequestControlOnce(entityCatch);
				VEHICLE::SET_VEHICLE_ENGINE_ON(RcCar, true, true);
			}
			else
				RcCar = NULL;

			if (p_variables->world.DragCars && entityCatch)
			{
				if (ENTITY::IS_ENTITY_A_VEHICLE(entityCatch))
				{
					RequestControlOnce(entityCatch);
					location = position;
					other = ENTITY::GET_ENTITY_COORDS(entityCatch, true);
					AI::SET_HIGH_FALL_TASK(entityCatch, 0, 0, 0);
					//ENTITY::SET_ENTITY_HAS_GRAVITY(entityCatch, FALSE);
					Vector3 diff = { position.x - other.x, position.y - other.y, position.z - other.z };
					float Distance = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
					dist = Distance - 0.1f;
					multiplier = 10.0;
					apply_velocity(entityCatch, camera);

				}
				//auto coords = ENTITY::GET_ENTITY_COORDS(entityCatch, true);

			}

		}
		if (p_variables->world.PhysicGun)
		{
			short rightMouseState = CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM);

			static Entity ent = NULL;
			Entity entityCatch = 0;


			if (leftMouseState)
			{
				if (ent == NULL)
				{
					bool catched = g_Math->raycast(&entityCatch, camera);
					if (catched && ENTITY::IS_ENTITY_A_VEHICLE(entityCatch))
						ent = entityCatch;
					RequestControlOnce(entityCatch);
				}
				else if (!rightMouseState)
				{
					//if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
					AI::SET_HIGH_FALL_TASK(ent, 0, 0, 0);
					ENTITY::SET_ENTITY_ALPHA(ent, 150, 0);
					ENTITY::SET_ENTITY_COLLISION(ent, false, false);
					multiplier = 5.0;
				}
				if (rightMouseState && ent)
				{
					dist = 100;
					multiplier = 100.0;
					ENTITY::SET_ENTITY_COLLISION(ent, true, true);
					ENTITY::RESET_ENTITY_ALPHA(ent);
					apply_velocity(ent, camera);
				}
			}
			else
			{
				ENTITY::SET_ENTITY_COLLISION(ent, true, true);
				ENTITY::RESET_ENTITY_ALPHA(ent);

				ent = NULL;
				dist = 10;
			}
			if (ent)
			{
				location = position;
				other = ENTITY::GET_ENTITY_COORDS(ent, true);
				apply_velocity(ent, camera);
			}

		}
		//float pitch = math::deg_to_rad(rot.x); // vertical
		//float roll = rot.y;
		float yaw = g_Math->degToRad(rot.z); // horizontal

		position.x += (vecChange.x * cos(yaw) - vecChange.y * sin(yaw)) * mult;
		position.y += (vecChange.x * sin(yaw) + vecChange.y * cos(yaw)) * mult;
		position.z += vecChange.z * mult;

		p_variables->world.camCoords = position;
		CAM::SET_CAM_COORD(camera, position.x, position.y, position.z);
		STREAMING::_SET_FOCUS_AREA(position.x, position.y, position.z, 0.f, 0.f, 0.f);

		HUD::LOCK_MINIMAP_POSITION(position.x, position.y);
		HUD::LOCK_MINIMAP_ANGLE(rot.z);
		HUD::SET_BLIP_COORDS(camBlip, position.x, position.y, position.z);
		HUD::SET_BLIP_ROTATION(camBlip, rot.z);

		rotation = CAM::GET_GAMEPLAY_CAM_ROT(2);
		CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 2);

		if (p_variables->misc.noclip)
		{
			bool invehicle = PED::IS_PED_IN_ANY_VEHICLE(playerPedID, false);
			if (Veh && invehicle)
				ENTITY::SET_ENTITY_COORDS(Veh, position.x, position.y, position.z - 2.0f, 1, 1, 1, 1);
			else
				ENTITY::SET_ENTITY_COORDS(playerPedID, position.x, position.y, position.z - 2.0f, 1, 1, 1, 1);
		}

	}
	else {
		if (CamCreated) {
			CAM::SET_CAM_ACTIVE(camera, false);
			CAM::RENDER_SCRIPT_CAMS(false, true, 500, true, true);
			CAM::DESTROY_CAM(camera, false);
			//CAM::_0xC819F3CBB62BF692(true, 0, 0);
			HUD::REMOVE_BLIP(camBlip);
			HUD::UNLOCK_MINIMAP_POSITION();
			HUD::UNLOCK_MINIMAP_ANGLE();

			STREAMING::CLEAR_FOCUS();
			p_variables->world.cam = 0;
			if (Veh)
				ENTITY::FREEZE_ENTITY_POSITION(Veh, false);
			ENTITY::FREEZE_ENTITY_POSITION(playerPedID, false);
			if (p_variables->misc.noclip)
			{
				ENTITY::SET_ENTITY_ALPHA(playerPedID, 255, 0);
				ENTITY::FREEZE_ENTITY_POSITION(playerPedID, false);
				ENTITY::SET_ENTITY_COLLISION(playerPedID, true, 0);
				if (Veh)
				{
					ENTITY::SET_ENTITY_COLLISION(Veh, true, 0);
					ENTITY::SET_ENTITY_ALPHA(Veh, 255, 0);
				}
			}


			CamCreated = false;
		}
	}
}//end init