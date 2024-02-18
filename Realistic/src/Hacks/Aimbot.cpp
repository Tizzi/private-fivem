#include <Aimbot.h>
#include <GameMath.h>
#include <invoker.hpp>
#include <hashes.hpp>
#include "../Fonts.h"

 int ped_array = 0;

inline CPed* Aimbot::BestTarget()
{
	CPed* ClosestPlayer = nullptr; 
	float closestdist = FLT_MAX;
	float dist;
	CPedFactory* g_pedFactory = (CPedFactory*)*(DWORD64*)(g_pattern->g_worldFactory);
	CPed* LocalPlayer = g_pedFactory->m_local_ped();
	CReplayInterface* ReplayInterface = (CReplayInterface*)*(DWORD64*)(g_pattern->g_replayInterface);
	CPedInterface* PedInterface = ReplayInterface->m_ped_interface();

	p_variables->p_friend.Staff.clear();
	for (int index = 0; index < PedInterface->m_pedmax(); index++)
	{
		CPed* Peds = PedInterface->m_pedlist()->m_ped(index);
		if (!Peds) continue;
		auto handle = g_pattern->pointer_to_handle(Peds);
		
		//PED::_0x2B5AA717A181FB4C(handle, FALSE);
		//ped_array = index; 
		if (Peds == g_pedFactory->m_local_ped()) continue; //localplayer
		if (Peds->health() <= 0) continue;
		if (p_variables->p_aimbot.ignore_peds && Peds->IsPedOrFalse()) continue;
		if (Peds->isGodMode() && !ENTITY::IS_ENTITY_VISIBLE(g_pattern->pointer_to_handle(Peds)))
			Peds->markAsStaff();
		if (Peds->IsFriend()) continue;
		/*
		auto componentID = p_variables->radar3d.teamCheckComponent;
		auto pedHandle = g_pattern->pointer_to_handle(Peds);
		auto playerHandle = g_pattern->pointer_to_handle(LocalPlayer);
		auto pedComp = PED::GET_PED_PROP_INDEX(pedHandle, componentID);
		if (pedComp && PED::GET_PED_PROP_INDEX(playerHandle, componentID))
		{
			auto model = PED::GET_PED_PROP_TEXTURE_INDEX(pedHandle, componentID);
			auto playerModel = PED::GET_PED_PROP_TEXTURE_INDEX(playerHandle, componentID);
			if(pedComp == 3)p_variables->p_friend.Staff.push_back(Peds->m_player_info()->m_player_id());
			if(model == playerModel)
				p_variables->p_friend.Friend.push_back(Peds->m_player_info()->m_player_id());
		}
		*/
		Vector3 DistanceCalculation = (g_pedFactory->m_local_ped()->m_visual_pos() - Peds->m_visual_pos());
		double Distance = sqrtf(DistanceCalculation.x * DistanceCalculation.x + DistanceCalculation.y * DistanceCalculation.y + DistanceCalculation.z * DistanceCalculation.z);


		if (Distance <= p_variables->p_aimbot.distance)
		{
			ImVec2 PedPos = g_Math->WorldToScreen(Peds->m_visual_pos());
			ImVec2 Head = g_Math->GetBone_W2S(Peds, ePedBoneType::HEAD); 
			
			static ImVec2 middle = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
			dist = g_Math->pythag(middle, Head);
			if (dist < closestdist)
			{
				{
					closestdist = dist;
					ClosestPlayer = Peds;
					
				}
			}
		}
	}
	return ClosestPlayer;
}

inline auto get_distance = [](double x1, double y1, double x2, double y2) {
	return sqrtf(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
};

inline Vector3 findClosestDot(const ImVec2& center, const std::vector<Vector3>& bones) {
	double minDistance = 8000;
	Vector3 closestDot;

	for (const Vector3& pos : bones) {
		auto dot = g_Math->WorldToScreen(pos);
		double distance = std::sqrt(std::pow(dot.x - center.x, 2) + std::pow(dot.y - center.y, 2));
		if (distance < minDistance) {
			minDistance = distance;
			closestDot = pos;
		}
	}

	return closestDot;
}

//CPed* Peds_array[256]; 
void Aimbot::Initialize(struct timer timer)
{
	if (!p_variables->misc.d3DInited)return;
	if (p_variables->world.isPauseMennuActive) return;
	auto* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
	if (!g_pedFactory)return;
	CPed* LocalPlayer = g_pedFactory->m_local_ped();
	if (!LocalPlayer)return;
	if (p_variables->p_menu.open) return;
	

	bool ontick = timer.tick100;
	if (0)//LocalPlayer->health()>0.0)
	{
		auto currentH = LocalPlayer->health();
		auto maxH = LocalPlayer->m_healthmax();
		static float lastH = LocalPlayer->health();
		
		if (currentH < maxH && currentH < lastH)
		{
			
			auto totalLost = lastH - currentH;
			auto regenH = totalLost * 3/4;
			LocalPlayer->m_health(currentH + regenH);
			lastH = currentH + regenH;
			PED::SET_PED_SUFFERS_CRITICAL_HITS(PLAYER::PLAYER_PED_ID(), FALSE);
		}
	}
	if (p_variables->p_aimbot.triggerbot)
	{
		static bool state = false;
		static bool onTarget = false;
		if (state && ontick)
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // Release
			state = false;
		}
		Entity AimedAtEntity;
		if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, 25))
		{
			//if (raycast(&AimedAtEntity))
			if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(PLAYER::PLAYER_ID(), &AimedAtEntity))
			//if (PLAYER::IS_PLAYER_FREE_AIMING(PLAYER::PLAYER_PED_ID()))
			{
				if (!state && ENTITY::IS_ENTITY_A_PED(AimedAtEntity) && !ENTITY::IS_ENTITY_DEAD(AimedAtEntity))
				{
					CPed* pPed = (CPed*)g_pattern->handle_to_pointer(AimedAtEntity);
					if(!pPed->IsFriend())
					if (onTarget)
					{
						onTarget = false;
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); // Press
						state = true;
					}
					else
						onTarget = true;
					
					//Vector3 coords = ENTITY::GET_ENTITY_COORDS(AimedAtEntity, 1);
					//PED::SET_PED_SHOOTS_AT_COORD(PLAYER::PLAYER_PED_ID(), coords.x, coords.y, coords.z, true);
					
				}
			}
		}

	}
	if (p_variables->p_aimbot.enable)
	{
		static CPed* best_target = 0;
		if (timer.tick500) {
			best_target = this->BestTarget();
			
		}
		static Vector3 best_target_pos = Vector3(0, 0, 0); 

		bool is_Visible = true;
		static auto middle_x = ImGui::GetIO().DisplaySize.x / 2;
		static auto middle_y = ImGui::GetIO().DisplaySize.y / 2;

		if (best_target)
		{
			if (p_variables->p_aimbot.visibility_check)
			{
				is_Visible = g_pattern->HasEntityClearLosToEntity(LocalPlayer, best_target, 235677380);//4
				if (!is_Visible)return;
			}
			
			if (!p_variables->p_aimbot.nearest_bone)
			{
			
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
					best_target_pos = g_Math->GetBone(best_target, ePedBoneType::L_ANKLE);
					break;
				}
				
			}
			else
			{
			
				Vector3 Bone1 = g_Math->GetBone(best_target, ePedBoneType::HEAD);
				Vector3 Bone2 = g_Math->GetBone(best_target, ePedBoneType::NECK);
				Vector3 Bone3 = g_Math->GetBone(best_target, ePedBoneType::ABDOMEN);
				Vector3 Bone4 = g_Math->GetBone(best_target, ePedBoneType::L_FOOT);
				Vector3 Bone5 = g_Math->GetBone(best_target, ePedBoneType::R_FOOT);
				Vector3 Bone6 = g_Math->GetBone(best_target, ePedBoneType::R_HAND);
				Vector3 Bone7 = g_Math->GetBone(best_target, ePedBoneType::L_HAND);

				std::vector<Vector3> bones = { Bone1, Bone2, Bone3, Bone4, Bone5, Bone6, Bone7 };
				best_target_pos = findClosestDot(ImVec2(middle_x, middle_y), bones);
			}


			if (ontick && 0)
			{
				Hash WeaponHash = 0;
				auto player = PLAYER::PLAYER_PED_ID();
				WEAPON::GET_CURRENT_PED_VEHICLE_WEAPON(player, &WeaponHash);
				locationManager->locations.clear();
				locationManager->addLocation(std::to_string(WeaponHash), { 0,0,0 });
			}
			p_variables->p_aimbot.aimloc = best_target_pos;///
			p_variables->p_aimbot.currentClosestPedID = (uintptr_t)best_target;
			ImVec2 Bone_Pos_W2s = g_Math->WorldToScreen(best_target_pos);
			

			auto lmbState = SAFE_CALL(GetAsyncKeyState)(p_variables->p_aimbot.Key2);
			auto rmbState = SAFE_CALL(GetAsyncKeyState)(p_variables->p_aimbot.Key);
			auto AltState = SAFE_CALL(GetAsyncKeyState)(VK_LMENU);

			if (rmbState && p_variables->p_aimbot.normal_aim ||
				lmbState && p_variables->p_aimbot.normal_aim ||
				lmbState && p_variables->p_aimbot.silent_aim ||
				lmbState && p_variables->p_aimbot.magic_aim ||
				AltState
				)
			{
				
				auto fov_distance = get_distance(middle_x, middle_y, Bone_Pos_W2s.x, Bone_Pos_W2s.y);
				bool inAimFov = fov_distance < p_variables->p_aimbot.fov_size;
				bool inSilentFov = fov_distance < p_variables->p_aimbot.silent_fov;

				if (p_variables->p_aimbot.focusCamOnVehRot)
				{
					auto myVeh = LocalPlayer->m_vehicle_manager();
					Vector3 rot, pos;
					if (myVeh)
					{
						float weaponRange = 10.f;
						rot = myVeh->m_navigation_manager()->m_get_rot();
						pos = myVeh->m_visual_pos();
						DWORD64 addr = g_pattern->GetCameraViewAngles();
						if (!addr) { /*Log::Msg("addnull");*/ return; }
						*(Vector3*)(addr + 0x40) = rot;  //FPS
						*(Vector3*)(addr + 0x3D0) = rot;  //TPS

					}
				}

				if (inAimFov)
				{
					if (is_Visible)//found best target ..Start Aimming
					{
						Vector3 velocity = { 0,0,0 };
						auto vehicle = best_target->m_vehicle_manager();
						auto handle = g_pattern->pointer_to_handle(best_target);
						auto inveh = PED::IS_PED_IN_ANY_VEHICLE(handle, false);
						auto cPedName = best_target->GetName();
						if (strstr(cPedName, "Mr.x8") != NULL)best_target_pos = { 0,0,0 };

						if (inveh)
						{
							best_target_pos = best_target_pos = g_Math->GetBone(best_target, ePedBoneType::HEAD);
							velocity = vehicle->GetVelocity();
						}
						else
							velocity = best_target->velocity();

						auto deltaTime = ImGui::GetIO().Framerate / 2;// *p_variables->misc.float1 / 2;
						if(p_variables->p_aimbot.prediction)
							best_target_pos += (velocity / deltaTime);//m/s /s0.002

						auto player = PLAYER::PLAYER_PED_ID();
						bool isShooting = PED::IS_PED_SHOOTING(player);
						bool isReloading = PED::IS_PED_RELOADING(player);
	//do aim bot
						if (p_variables->p_aimbot.normal_aim) {
							static float rrandx, rrandy, rrandz;
							Vector3 preAimSpot = best_target_pos;
							if (timer.tick200 && p_variables->p_aimbot.randomSpot)
							{
								rrandx = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-0.25f, 0.25f);
								rrandy = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-0.25f, 0.25f);
								rrandz = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-0.3f, 0.0f);
								
							}
							preAimSpot = { preAimSpot.x + rrandx, preAimSpot.y + rrandy,preAimSpot.z + rrandz };
							if(p_variables->p_aimbot.mouseAim && !isReloading)//&& !WeaponHash)
								g_Math->MouseAim(preAimSpot);
							else if (!isReloading)
								g_Math->set_aim_angels(preAimSpot, deltaTime);
						}

						Hash VehWeapon = 0;
						WEAPON::GET_CURRENT_PED_VEHICLE_WEAPON(player, &VehWeapon);

						if (VehWeapon && AltState) {							//vehicle weapon aimbot

							auto myVeh = LocalPlayer->m_vehicle_manager();

							auto pos = LocalPlayer->m_visual_pos() + LocalPlayer->heading();
							auto WeaponDamage = WEAPON::GET_WEAPON_DAMAGE(VehWeapon, NULL);
							//GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS_IGNORE_ENTITY_NEW(pos.x, pos.y, pos.z,
							//	best_target_pos.x, best_target_pos.y, best_target_pos.z, WeaponDamage, true, WeaponHash, player,
							//	true, false, 200.f, player, 1, 1, 0, 0);

							if (VehWeapon != VehicleWeapons::VEHICLE_WEAPON_STRIKEFORCE_CANNON
								&& VehWeapon != VehicleWeapons::VEHICLE_WEAPON_PLAYER_BUZZARD
								&& VehWeapon != 0x25453A1B //625293851 warhog 25453A1B
								)//1673139446 m3  63BA14F6
							{
								if (timer.tick200)
								{
									GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos.x, pos.y, pos.z,
										best_target_pos.x, best_target_pos.y, best_target_pos.z, WeaponDamage, true, VehWeapon, player,
										true, false, 200.f);
								}

							}

							else
							{
								float randx = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-2.5f, 2.5f);
								float randy = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-2.5f, 2.5f);//1.5f
								//float randz = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-0.5f, 1.5f);
								float randv = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(0.f, 20.f);

								best_target_pos += (velocity * randv) / deltaTime;
								best_target_pos.x += randx;
								best_target_pos.y += randy;
								//best_target_pos.z += randz;

								auto eyesPos = pos;
								Vector3 vec3Diff(pos.x - best_target_pos.x, pos.y - best_target_pos.y, pos.z - best_target_pos.z);
								float distance = vec3Diff.Length();
								Vector3 Output = Vector3((best_target_pos.x - pos.x) / distance, (best_target_pos.y - pos.y) / distance, (best_target_pos.z - pos.z) / distance);
								Output = Output.Normalize();
								//auto Origin = vehicle->m_navigation_manager()->m_get_rot();

								//Vector3 Delta = Origin - Output;
											//float qdot = Origin.x * Output.x + Origin.y * Output.y + Origin.z * Output.z;
											//auto angle = 2 * acos(qdot);

								//Delta = Delta.Normalize();
								//auto angle = Delta.Length() * 180.0 / DirectX::XM_PI;
								if (1)// (Delta.Length() < 0.5f)
								{
									//if((Delta.Length() < 0.3f))
									if (VehWeapon == VehicleWeapons::VEHICLE_WEAPON_PLAYER_BUZZARD)
									{
										//auto r1 = ENTITY::GET_ENTITY_ROTATION(emtity,5);
										auto dOutput = CAM::GET_GAMEPLAY_CAM_ROT(5);
										Vector3 Output2 = { dOutput.x,dOutput.y,dOutput.z };
										auto emtity = PED::GET_VEHICLE_PED_IS_USING(player);
										ENTITY::SET_ENTITY_ROTATION(emtity, Output2.x, Output2.y, Output2.z, 5, 0);
										//auto r2 = myveh->m_navigation_manager()->m_get_rot();
										//Log::Msg("x: %f | x: %f | y : %f | y : %f | z : %f | z : %f", r1.x, r2.x, r1.y, r2.y, r1.z, r2.z);
									}
									else
										myVeh->m_navigation_manager()->m_rot_vec3(Output);

									VEHICLE::SET_VEHICLE_SHOOT_AT_TARGET(player, g_pattern->pointer_to_handle(best_target), best_target_pos.x, best_target_pos.y, best_target_pos.z);
									//g_Math->MouseAim(best_target_pos);
								}
							}
						}
						
						if (inSilentFov && p_variables->p_aimbot.silent_aim)
						{
							static int bCount = 0;
							static float hitChancePerc = (float)p_variables->p_aimbot.hitChance / 100;
							
							if (isShooting) bCount++;
							if (lmbState && ontick)
							{
								static UINT8 sCount = 0;
								if (sCount < 2 && !isReloading)
								{
									sCount++;
									PED::SET_PED_SHOOTS_AT_COORD(player, best_target_pos.x, best_target_pos.y, best_target_pos.z, false);
									//ImGui::InsertNotification({ ImGuiToastType_Success, 1000, Encrypt("Silentaim!!!") });
								}
								else
								{
									sCount = 0;
									bCount = 0;//reset
								}
							}

						}
						auto WeaponHash = WEAPON::GET_SELECTED_PED_WEAPON(player);
						if (AltState && timer.tick200)
						{
							if (WeaponHash == WeaponHashes::WEAPON_STICKYBOMB || WeaponHash == WeaponHashes::WEAPON_GRENADE || WeaponHash == WeaponHashes::WEAPON_PIPEBOMB)
							{
								auto WeaponDamage = WEAPON::GET_WEAPON_DAMAGE(WeaponHash, NULL);
								GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(best_target_pos.x, best_target_pos.y, best_target_pos.z,
									best_target_pos.x, best_target_pos.y, best_target_pos.z-0.2f, WeaponDamage, false, WeaponHash, player,
									true, false, 200.f);
								ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Nade Thrown!") });
							}
						}
						if (lmbState && p_variables->p_aimbot.magic_aim)
						{
							static int bCount = 0;
							float hitChancePerc = (float)p_variables->p_aimbot.hitChance / 100;

							if (isShooting) bCount++;
							
							if (WeaponHash && hitChancePerc * bCount >= 1 && ontick)//|| WeaponHash && AltState)
							{
								int currentAmmo = 0;// WEAPON::GET_AMMO_IN_PED_WEAPON(player, WeaponHash);
								WEAPON::GET_AMMO_IN_CLIP(player, WeaponHash, &currentAmmo);
								auto WeaponDamage = WEAPON::GET_WEAPON_DAMAGE(WeaponHash, NULL);
								p_variables->p_aimbot.currentClosestPedID = (uintptr_t)best_target;
								auto heading = best_target->heading();
								Vector3 startpos = best_target_pos + heading;//Vector3{0.07, 0.07f, 0.07f}
								Vector3 endpos = best_target_pos - Vector3{0.09f, 0.09f, 0.09f};

								if (currentAmmo && !isReloading)
								{
									if(p_variables->p_aimbot.magic_aim)
										GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startpos.x, startpos.y, startpos.z,
											endpos.x, endpos.y, endpos.z, WeaponDamage, false, WeaponHash, player,
											true, false, 200.f);
								}


								bCount = 0;
							}
						}
					}
				}
			}
		}
	}
}