#pragma once
#include <Vehicle.h>
#include <GameMath.h>

inline void max_vehicle(Vehicle veh)
{
	Hash model = ENTITY::GET_ENTITY_MODEL(veh);
	int neonColor[3] = { 0x32,0xCD,0x32 };
	VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

	VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TURBO, TRUE);
	VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TYRE_SMOKE, TRUE);
	VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_XENON_LIGHTS, TRUE);
	VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, 1);
	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, false);
	VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(veh, neonColor[0], neonColor[1], neonColor[2]);
	VEHICLE::SET_VEHICLE_LIGHTS(veh, true);
	VEHICLE::SET_VEHICLE_ALARM(veh, true);
	for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
	{
		if (slot == MOD_LIVERY)
		{
			continue;
		}

		int count = VEHICLE::GET_NUM_VEHICLE_MODS(veh, slot);
		if (count > 0)
		{
			int selected_mod = -1;

			for (int mod = count - 1; mod >= -1; mod--)
			{

				selected_mod = mod;
				break;
			}

			if (selected_mod != -1)
			{
				VEHICLE::SET_VEHICLE_MOD(veh, slot, selected_mod, true);
			}
		}
	}
}

inline void max_vehicle_performance(Vehicle veh)
{

	VehicleModType perfomance_mods[] = { MOD_ENGINE, MOD_BRAKES, MOD_TRANSMISSION, MOD_SUSPENSION, MOD_ARMOR, MOD_NITROUS, MOD_TURBO };
	VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

	for (auto mod_slot : perfomance_mods)
	{
		if (mod_slot != MOD_NITROUS && mod_slot != MOD_TURBO)
			VEHICLE::SET_VEHICLE_MOD(veh, mod_slot, VEHICLE::GET_NUM_VEHICLE_MODS(veh, mod_slot) - 1, true);
		else
			VEHICLE::TOGGLE_VEHICLE_MOD(veh, mod_slot, true);
	}

}

void Vehicle_Func::DestroyVehicle(Vehicle veh)
{
	if (!veh) return;
	if (!ENTITY::DOES_ENTITY_EXIST(veh))return;
	if (!ENTITY::IS_ENTITY_A_VEHICLE(veh)) return;
	
	RequestControlOnce(veh);
	if (p_variables->world.smashVehicle)
	{
		VEHICLE::SMASH_VEHICLE_WINDOW(veh, 0);
		VEHICLE::SMASH_VEHICLE_WINDOW(veh, 1);
		VEHICLE::SMASH_VEHICLE_WINDOW(veh, 2);
		VEHICLE::SMASH_VEHICLE_WINDOW(veh, 3);

	}
	if (p_variables->world.burstTires)
	{
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 0, true, 1000.0f);
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 1, true, 1000.0f);
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 2, true, 1000.0f);
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 3, true, 1000.0f);
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 4, true, 1000.0f);
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 5, true, 1000.0f);
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 6, true, 1000.0f);
		VEHICLE::SET_VEHICLE_TYRE_BURST(veh, 7, true, 1000.0f);

	}
	if (p_variables->world.breakDoors)
	{
		VEHICLE::SET_VEHICLE_DOOR_BROKEN(veh, 0, 0);
		VEHICLE::SET_VEHICLE_DOOR_BROKEN(veh, 1, 0);
		VEHICLE::SET_VEHICLE_DOOR_BROKEN(veh, 2, 0);
		VEHICLE::SET_VEHICLE_DOOR_BROKEN(veh, 3, 0);

	}
	if (p_variables->world.destroyEngine)
	{
		VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh, -4000.f);

	}
	if (p_variables->world.breakWheels)
	{

		CFX::BreakOffVehicleWheel(veh, 0, 1, 0, 0, 0);
		CFX::BreakOffVehicleWheel(veh, 1, 1, 0, 0, 0);
		CFX::BreakOffVehicleWheel(veh, 2, 1, 0, 0, 0);
		CFX::BreakOffVehicleWheel(veh, 3, 1, 0, 0, 0);
		CFX::BreakOffVehicleWheel(veh, 4, 1, 0, 0, 0);
		CFX::BreakOffVehicleWheel(veh, 5, 1, 0, 0, 0);
		CFX::BreakOffVehicleWheel(veh, 6, 1, 0, 0, 0);
		CFX::BreakOffVehicleWheel(veh, 7, 1, 0, 0, 0);

	}
	if (p_variables->world.setAlarm)
	{
		VEHICLE::SET_VEHICLE_ALARM(veh, true);
		VEHICLE::START_VEHICLE_ALARM(veh);

	}
	if (p_variables->world.washVehicle)
	{

		if (VEHICLE::GET_VEHICLE_DIRT_LEVEL(veh) > 0.0f)
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0.0f);
		else
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 15.0f);

	}
	if (p_variables->world.SinkCar)
	{
		ENTITY::SET_ENTITY_COLLISION(veh, false, false);
	}

	if (p_variables->world.ToSky)
	{
		ENTITY::APPLY_FORCE_TO_ENTITY(veh, 3, 0, 0, 150.f, 0, 0, 0, 0, false, true, true, false, true);
	}

	if (p_variables->world.ExplodeCar && !p_variables->world.FreeCam)
	{
		auto coords = ENTITY::GET_ENTITY_COORDS(veh, true);
		NETWORK::NETWORK_EXPLODE_VEHICLE(veh, 1, 1, 0);
		VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh, -4000.f);
		//FIRE::ADD_EXPLOSION(coords.x, coords.y, coords.z, 2, 1000.0, true, false, 1.0);
	}

	if (p_variables->world.DeleteCar)
	{
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);
		VEHICLE::DELETE_VEHICLE(&veh);
	}
}
void Vehicle_Func::Initialize(struct timer timer)
{
	CPedFactory* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
	CPed* LocalPlayer = g_pedFactory->m_local_ped();
	CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
	CVehicleInterface* vehicle_interface = ReplayInterface->m_vehicleinterface();

	if (!LocalPlayer)
		return;
	bool ontick = timer.tick100;
	bool invehicle = p_variables->p_vehicle.imOnVehicle;// PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false);
	if (invehicle)
	{
		auto veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
		if (p_variables->p_vehicle.GodMode && ontick)
		{
			ENTITY::SET_ENTITY_INVINCIBLE(veh, true);
		}
		if (p_variables->p_vehicle.p_nocol)
		{
			ENTITY::SET_ENTITY_COLLISION(veh, false, true);
		}
		if (p_variables->p_vehicle.repair) {
			g_pattern->repair_vehicle(veh);
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
			p_variables->p_vehicle.repair = false;
		}
		if (p_variables->p_vehicle.maxPerformance) {
			max_vehicle_performance(veh);
			p_variables->p_vehicle.maxPerformance = false;
		}
		if (p_variables->p_vehicle.maxVehMods) {
			max_vehicle(veh);
			p_variables->p_vehicle.maxVehMods = false;
		}
		if (p_variables->p_vehicle.p_rocket_boost && ontick)
		{
			VEHICLE::_SET_VEHICLE_ROCKET_BOOST_ACTIVE(veh, 1);
			VEHICLE::_SET_VEHICLE_ROCKET_BOOST_REFILL_TIME(veh, 15);
		}
		if (p_variables->p_vehicle.hornBoost && ontick)
		{
			if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HORN))
			{
				float multiplier = 1.1f;
				auto velo = ENTITY::GET_ENTITY_VELOCITY(veh);
				ENTITY::SET_ENTITY_VELOCITY(veh, multiplier * velo.x, multiplier * velo.y, multiplier * velo.z);
			}
		}
		if (p_variables->p_vehicle.instantBreak && ontick)
		{
			if (invehicle && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HANDBRAKE))
			{
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, 0.0f);
			}
		}

		if (p_variables->p_vehicle.driveWander || p_variables->p_vehicle.driveWaypoint || p_variables->p_vehicle.driveCoord)
		{
			g_pattern->WayPointPos = *reinterpret_cast<Vector2*>(g_pattern->WayPointRead);
			auto playerid = PLAYER::PLAYER_PED_ID();
			if (p_variables->p_vehicle.driveWander)
			{
				AI::TASK_VEHICLE_DRIVE_WANDER(playerid, veh, 100.00, 786468);
				p_variables->p_vehicle.driveWander = false;
			}
			if (p_variables->p_vehicle.driveWaypoint)
			{
				AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerid, veh, g_pattern->WayPointPos.x, g_pattern->WayPointPos.y, 50, 100.00, 786468, 100);
				p_variables->p_vehicle.driveWaypoint = false;
			}
			if (p_variables->p_vehicle.driveCoord)
			{
				AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerid, veh, locationManager->selected.x, locationManager->selected.y, locationManager->selected.z, 100.00, 786468, 100);
				p_variables->p_vehicle.driveCoord = false;
			}
			PED::SET_DRIVER_ABILITY(playerid, 10.0);
			PED::SET_DRIVER_AGGRESSIVENESS(playerid, 10.0);
			p_variables->p_vehicle.driveWaypoint = false;
		}
		if (p_variables->p_vehicle.stopDrivingTask)
		{
			AI::CLEAR_PED_TASKS(PLAYER::PLAYER_PED_ID());
			p_variables->p_vehicle.stopDrivingTask = false;
		}

	}
	if (p_variables->p_vehicle.spawnvehicule) {
		static bool NeedToSpawn = true;
		static bool IsModelRequested = false;
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		Vehicle playerVeh = NULL;

		auto hash = p_variables->p_vehicle.VehicleToSpawn;
		//constexpr auto hash = RAGE_JOAAT("adder");
		if (!IsModelRequested) {
			if (STREAMING::IS_MODEL_VALID(hash))
			{
				STREAMING::REQUEST_MODEL(hash);
				IsModelRequested = true;
			}
			else
			{
				p_variables->p_vehicle.spawnvehicule = false;
				ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("Vehicle Name is not valid") });
			}

		}
		else
		{
			if (STREAMING::HAS_MODEL_LOADED(hash))
			{
				if (invehicle)
				{
					auto veh = PED::GET_VEHICLE_PED_IS_IN(playerPed, FALSE);
					ENTITY::SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);
					VEHICLE::DELETE_VEHICLE(&veh);
				}
				PVector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.0, 8.0, 0.5);
				if (p_variables->p_vehicle.BoostPlaneOnSpawn)
					coords.z += 400.0f;
				
				Vector3 spawnCoords = { coords.x ,coords.y,coords.z };
				if (p_variables->p_vehicle.spawnNetworked)
				{
					//*(unsigned short*)g_pattern->g_isScriptNetworked = 0xC085;
					playerVeh = VEHICLE::CREATE_VEHICLE(hash, coords.x, coords.y, coords.z, ENTITY::GET_ENTITY_HEADING(playerPed), false, TRUE);
					//*(unsigned short*)g_pattern->g_isScriptNetworked = 0xC084;
					//playerVeh = g_pattern->create_vehicle(hash, &spawnCoords, ENTITY::GET_ENTITY_HEADING(playerPed), true, true);
					NETWORK::NetworkRegisterEntityAsNetworked(playerVeh);
					auto netID = NETWORK::VEH_TO_NET(playerVeh);
					ENTITY::_SET_ENTITY_REGISTER(playerVeh, true);
					
					if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(playerVeh))
					{
						NETWORK::NetworkSetNetworkIdDynamic(netID, true);
						NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(netID, true);
						NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netID, false);
					}
						
				}
				else
				{
					playerVeh = VEHICLE::CREATE_VEHICLE(hash, coords.x, coords.y, coords.z, ENTITY::GET_ENTITY_HEADING(playerPed), FALSE, TRUE);
				}
					//NETWORK::NetworkRegisterEntityAsNetworked(playerVeh);
				if(p_variables->p_vehicle.spawnInVehicle)
					PED::SET_PED_INTO_VEHICLE(playerPed, playerVeh, SEAT_DRIVER);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				if (p_variables->p_vehicle.spawn_maxed) {
					max_vehicle(playerVeh);
					max_vehicle_performance(playerVeh);
				}
				VEHICLE::SET_VEHICLE_ENGINE_ON(playerVeh, true, true);

				if (p_variables->p_vehicle.BoostPlaneOnSpawn)
				{
					
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(playerVeh, 200.0);
					
				}
				if (p_variables->p_vehicle.nocollision)
					ENTITY::SET_ENTITY_COLLISION(playerVeh, false, false);
				IsModelRequested = false;
				
				p_variables->p_vehicle.spawnvehicule = false;
			}
		}

	}

	if (p_variables->p_vehicle.p_acc)
	{
		g_pedFactory->m_local_ped()->m_vehicle_manager()->m_vehicle_handling()->m_acceleration(p_variables->p_vehicle.p_acc_int);
		//VEHICLE::_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), p_variables->p_vehicle.p_acc_int);
		//VEHICLE::_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), p_variables->p_vehicle.p_acc_int);
	}
	if (p_variables->p_vehicle.driftMode && ontick)
	{
		//g_pedFactory->m_local_ped()->m_vehicle_manager()->m_gravity(p_variables->p_vehicle.p_grav_int); 
		if(invehicle)
			VEHICLE::SET_VEHICLE_GRAVITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), 5.0);
	}
	if (p_variables->p_vehicle.easyHandling && ontick)
	{
		//g_pedFactory->m_local_ped()->m_vehicle_manager()->m_gravity(p_variables->p_vehicle.p_grav_int); 
		if (invehicle)
			VEHICLE::SET_VEHICLE_GRAVITY(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), 20.0);
	}

	bool fallOffRestore = false;
	if (p_variables->p_vehicle.NoFallOff)
	{
		if(ontick)
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::PLAYER_PED_ID(), true);
		fallOffRestore = true;
	}
	else if(fallOffRestore)
	{
		PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::PLAYER_PED_ID(), false);
		fallOffRestore = false;
	}

	if (p_variables->p_vehicle.warp)
	{
		auto player = PLAYER::PLAYER_PED_ID();
		auto trying_to_open_veicle = PED::GET_VEHICLE_PED_IS_TRYING_TO_ENTER(player);
		if(trying_to_open_veicle)
		PED::SET_PED_INTO_VEHICLE(player, trying_to_open_veicle, -1);
	}
	if (p_variables->p_vehicle.deleteVehicle)
	{
		CVehicleManager* cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(p_variables->p_vehicle.SelectedVehicle);
		//not yet have the offset
		if (cVehicle)
		{
			auto player = PLAYER::PLAYER_PED_ID();
			uintptr_t cVehicleHandle = g_pattern->pointer_to_handle((CPed*)cVehicle);
			auto asdas = (Vehicle)cVehicleHandle;
			if (RequestControlOnce(cVehicleHandle))
			{
				ENTITY::SET_ENTITY_AS_MISSION_ENTITY(asdas, true, true);
				VEHICLE::DELETE_VEHICLE(&asdas);
			}

			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });

		}
		p_variables->p_vehicle.deleteVehicle = false;
	}
	if (p_variables->p_vehicle.repairVehicle)
	{
		CVehicleManager* cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(p_variables->p_vehicle.SelectedVehicle);
		if (cVehicle)
		{
			uintptr_t veh = g_pattern->pointer_to_handle((CPed*)(cVehicle));
			if (RequestControlOnce(veh))
			{
				VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh,999.f);
				VEHICLE::SET_VEHICLE_BODY_HEALTH(veh, 999.f);
				VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(veh);
			}
			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Reparing Car !!") });

		}
		p_variables->p_vehicle.repairVehicle = false;
	}
	if (p_variables->p_vehicle.FreeCamOpts)
	{
		CVehicleManager* cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(p_variables->p_vehicle.SelectedVehicle);
		if (cVehicle)
		{
			uintptr_t veh = g_pattern->pointer_to_handle((CPed*)(cVehicle));
			DestroyVehicle(veh);
			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("All Options Applied !!") });

		}
		p_variables->p_vehicle.FreeCamOpts = false;
	}
	if (p_variables->p_vehicle.bringnSteal)
	{
		CVehicleManager* cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(p_variables->p_vehicle.SelectedVehicle);
		if (cVehicle)
		{
			static INT8 tries = 0;
			static PVector3 SpawnCoord;
			static bool Need2GoBack = false;
			auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(cVehicle));
			auto Driver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(cVehicleHandle, -1);
			Vector3 OrgVehCoord = cVehicle->m_visual_pos();
			if (Need2GoBack)
			{
				if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(SpawnCoord.x, SpawnCoord.y, SpawnCoord.z, OrgVehCoord.x, OrgVehCoord.y, OrgVehCoord.z, 1) < 10.0f)
				{
					p_variables->p_vehicle.bringnSteal = false;//success
					p_variables->p_vehicle.repair = true;
					Need2GoBack = false;
					tries = 0;
					ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
				}
				else
				{
					tries++;
					if (tries > 300) 
					{ Need2GoBack = false; tries = 0; p_variables->p_vehicle.bringnSteal = false;
						ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("Tried 300x Still fail!!") });
					}//give up
					ENTITY::SET_ENTITY_COORDS(cVehicleHandle, SpawnCoord.x, SpawnCoord.y, SpawnCoord.z, 1, 1, 1, 1);
					
				}
					
				
			}
			else if ((!Driver || !PED::IS_PED_A_PLAYER(Driver)))
			{
				
				auto player = PLAYER::PLAYER_PED_ID();
				if (1)
				{
					RequestControlOnce(cVehicleHandle);
					Vector3 spawnloc = locationManager->findLocation("spawn");
					if (Driver)
						AI::CLEAR_PED_TASKS_IMMEDIATELY(Driver);

					if(!Need2GoBack)
					if (spawnloc.Length() > 0)
						SpawnCoord = { spawnloc.x, spawnloc.y, spawnloc.z };
					else
						SpawnCoord = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0.0, 8.0, 0.5);
					PED::SET_PED_INTO_VEHICLE(player, (uintptr_t)cVehicleHandle, -1);
					ENTITY::SET_ENTITY_COORDS(cVehicleHandle, SpawnCoord.x, SpawnCoord.y, SpawnCoord.z, 1, 1, 1, 1);
					Need2GoBack = true;
					
				}
				else
				{
					p_variables->p_vehicle.bringnSteal = false;
					ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("False To Take Control Of This Car!!") });
				}
					
			}
			else{ p_variables->p_vehicle.bringnSteal = false; 
				ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("This car is using by other player !!") });
			}
		}
		
	}
	



}//end init