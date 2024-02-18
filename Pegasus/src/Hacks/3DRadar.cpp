#include <3DRadar.h>
#include <GameMath.h>
#include <resources.h>

int healthsize = 11;
int healthsize1 = 5;

struct RadarCoordinates {
	float angle;
	ImVec2 coord;
};

void DrawTxt(char* text, float x, float y, float scale, float size, ImColor color)
{
	UI::SET_TEXT_COLOUR(0, 0, 255, 255);
	UI::SET_TEXT_FONT(4);
	UI::SET_TEXT_CENTRE(1);
	UI::SET_TEXT_PROPORTIONAL(1);//
	UI::SET_TEXT_SCALE(scale, size);
	UI::SET_TEXT_WRAP(0.0f, 1.0f);
	//HUD::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 255);
	UI::SET_TEXT_EDGE(2, 0, 0, 0, 255);
	//HUD::SET_TEXT_DROPSHADOW();//
	UI::SET_TEXT_OUTLINE();
	char* ss1 = const_cast<char*>("STRING");
	//char* ss2 = const_cast<char*>(text);
	UI::_SET_TEXT_ENTRY(ss1);
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_TEXT(x, y);

}
//DRAWING FUNCTIONS
void draw_rect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7)
{
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}
inline RadarCoordinates transformToRadarCoordinates(const Vector3 point, float Distance) {
	RadarCoordinates radarCoords;
	radarCoords.angle = std::atan2(point.y, point.x);
	radarCoords.coord = { Distance * std::cos(radarCoords.angle),Distance * std::sin(radarCoords.angle) };
	return radarCoords;
}
inline auto get2ddistance = [](double x1, double y1, double x2, double y2) {
	return sqrtf(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
};
inline float DistanceCalc(Vector3 dot1, Vector3 dot2)
{
	auto delta = dot1 - dot2;
	return delta.Length();
}
void Radar3D::Initialize()
{
	static auto DisplaySizex = ImGui::GetIO().DisplaySize.x;
	static auto DisplaySizey = ImGui::GetIO().DisplaySize.y;
	static auto middle_x = DisplaySizex / 2;
	static auto middle_y = DisplaySizey / 2;

	ImGui::PushFont(g_Renderer->TextFont);
	if (p_variables->p_aimbot.draw_fov)
	{
		ImGui::GetForegroundDrawList()->AddCircle({ middle_x, middle_y }, p_variables->p_aimbot.fov_size, ImColor(255, 255, 255, 150), 200);
	}
	if (p_variables->p_aimbot.crosshair == 2)
	{
		static float fontsize = 12;
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontsize, ImVec2(middle_x - fontsize / 2, middle_y - fontsize / 2), ImColor(255, 0, 0), ICON_FA_CROSSHAIRS);
	}
	else if (p_variables->p_aimbot.crosshair == 1)
		ImGui::GetForegroundDrawList()->AddCircle({ middle_x, middle_y - 1 }, 3, ImColor(255, 0, 0), 10);
	auto cam = 0;// g_pattern->GetCameraViewAngles();
	if (cam)
	{
		auto fps = *(Vector3*)(cam + 0x40);  //FPS
		auto tps = *(Vector3*)(cam + 0x3D0); //TPS
		std::string strfps = "Rot: X: " + std::to_string(fps.x) + " Y: " + std::to_string(fps.y) + " Z: " + std::to_string(fps.z);
		std::string strtps = "Rot2: X: " + std::to_string(tps.x) + " Y: " + std::to_string(tps.y) + " Z: " + std::to_string(tps.z);
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 20, ImVec2(DisplaySizex - 300, DisplaySizey - 100), ImColor(255, 0, 0), strfps.c_str());
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 20, ImVec2(DisplaySizex - 300, DisplaySizey - 120), ImColor(255, 0, 0), strtps.c_str());
	}

	if (p_variables->radar3d.p_enable3DRadar)
	{
		CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
		if (!ped_factory)
			return;
		CPed* LocalPlayer = ped_factory->m_local_ped();

		CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
		if (!ReplayInterface)
			return;

		CPedInterface* pedinterface = ReplayInterface->m_ped_interface();
		if (!pedinterface)
			return;
		
		
		
		if (HUD::IS_PAUSE_MENU_ACTIVE()) {
			p_variables->world.isPauseMennuActive = true;
			return;
		}
		else p_variables->world.isPauseMennuActive = false;
		//char text[] = "STRING";
		//DrawTxt(text, 600, 300, 1.0f, 0.25f, p_variables->p_color.Blue);
		//draw_rect(200, 200, 200, 200, 200, 200, 200, 255);
		//GRAPHICS::DRAW_DEBUG_TEXT_2D("Disabling First Person Cam", cPedPos.x, cPedPos.y, cPedPos.z, 0, 0, 255, 255);
		//GRAPHICS::DRAW_SPRITE("helicopterhud", "hud_corner", -0.01, -0.015, 0.013, 0.013, 0.0, 255, 0, 0, 200);

		int AdminNum = 0;
		static Attackers attackers;
		auto cAttackers = LocalPlayer->Attackers();
		if (g_offsets->isb2802 && cAttackers)
		{
			for (int i = 0; i < 3; i++)
			{
				CPed* attacker = nullptr;
				if (i == 0 && cAttackers->m_attacker0)
					attacker = (CPed*)cAttackers->m_attacker0;
				if (i == 1 && cAttackers->m_attacker1)
					attacker = (CPed*)cAttackers->m_attacker1;
				if (i == 2 && cAttackers->m_attacker2)
					attacker = (CPed*)cAttackers->m_attacker2;
				if (attacker && attacker != LocalPlayer)
				{
					auto attkinfo = attacker->m_player_info();
					if(attkinfo)
					attackers.add(attkinfo->m_player_id());
				}
			}
			
		}

		auto myVehicle = LocalPlayer->m_vehicle_manager();
		if (myVehicle && p_variables->p_vehicle.imOnVehicle)
		{
			int bodyHealth = myVehicle->body_health();
			int engineHealth = myVehicle->engine_health(); //VEHICLE::GET_VEHICLE_ENGINE_HEALTH(g_pattern->pointer_to_handle((CPed*)myVehicle));
			int Speed = myVehicle->get_speed()*3.6f;
			std::string name(myVehicle->modelInfo()->Name);
			
			char disstr[120];
			sprintf_s(disstr, "^5Model : ^2%s\n^5Speed: ^2%d KM/H\n^5Body HP: ^2%d\n^5Engine HP: ^2%d", name.c_str(), Speed, bodyHealth, engineHealth);
			
			g_Renderer->DrawText(ImGui::GetFont(), disstr, ImVec2(middle_x-50, DisplaySizey - 160), 20, p_variables->p_color.Blue,0,3);

		}
		if (p_variables->ped_esp.bestTarget)
		{
			ImVec2 w2s_aimpos = g_Math->WorldToScreen(p_variables->p_aimbot.aimloc);
			g_Renderer->FillRect(w2s_aimpos.x, w2s_aimpos.y, 10, 10, ImColor(255, 255, 0), 20);
		}
		
		
		bool isInBase = LocalPlayer->isGodMode();
		Vector3 localPos = LocalPlayer->m_visual_pos();
		if (p_variables->misc.freecam)
		{
			//PVector3 camPos = CAM::GET_CAM_COORD(p_variables->world.cam);
			//PVector3 camRot = CAM::GET_CAM_ROT(p_variables->world.cam,2);
			PVector3 camPos = p_variables->world.camCoords;
			localPos = { camPos.x,camPos.y,camPos.z };
			//float RadarZoom = (4 * 1.0f) * 0.2f;
			//HUD::SET_RADAR_ZOOM(10);

		}
		for (int i = 0; i < pedinterface->m_pedmax(); i++)
		{
			CPed* cPed = pedinterface->m_pedlist()->m_ped(i);
			if (!cPed) continue;

			
			//if (health <= 0) continue;
			//if (cPed->IsAnimals()) continue;

			if (cPed->IsPedOrFalse() && !p_variables->ped_esp.enable)
				continue;

			if (cPed == LocalPlayer && !p_variables->radar3d.include_self)
				continue;

			int PedID = 0;
			if (cPed->IsPedOrFalse())PedID = i;
			else PedID = cPed->m_player_info()->m_player_id();
			float health = cPed->health();
			float healthMax = cPed->m_healthmax();
			float armor = *(float*)(cPed + g_offsets->m_armor);
			bool isDead = (health <= 0);
			bool isFriend = cPed->IsFriend(PedID);
			bool isStaff = cPed->IsStaff();
			bool isAttacker = attackers.isAttacker(PedID);
			//auto pedHandle = g_pattern->pointer_to_handle(cPed);
			bool isInVisible = !cPed->IsVisible();//!ENTITY::IS_ENTITY_VISIBLE(pedHandle)
			UINT8 teamID = 0;
			Vector3 cPedPos = cPed->m_visual_pos();
			Vector3 cPedRot = cPed->m_navigation_manager()->m_get_rot();
			auto cPedName = cPed->GetName();
			if (strstr(cPedName, "Mr.x8") != NULL)continue;
			float Distance = (localPos - cPedPos).Length();
			//Vector3 localRot = LocalPlayer->m_navigation_manager()->m_get_rot();
			ImVec2 w2sPed = g_Math->WorldToScreen(cPedPos);
			bool isonscreen = g_Renderer->isonscreen(w2sPed);
			if (p_variables->misc.TESTER && isDead && isFriend)
			{
				TriggerEvent(true, "cv-koth:Revive", PedID);
			}
			if (isAttacker)
			{
				if ((p_variables->p_aimbot.aimloc - cPedPos).Length() < 10.f) attackers.remove(PedID);//p_variables->p_aimbot.currentClosestPedID == (uintptr_t)cPed

				if (!isFriend)
				{
					float y = 300 + AdminNum * 20;
					AdminNum++;
					std::string bufff(cPedName);
					std::string buff = "^3Attacker : ^1" + bufff + " ^2[" + std::to_string((int)Distance) + "]\n";
					g_Renderer->DrawText(ImGui::GetFont(), buff.c_str(), { 20,y }, 15, p_variables->p_color.Red, false, 3);
				}else
					attackers.remove(PedID);
			}
			//GRAPHICS::DRAW_LINE(cPedPos.x, cPedPos.y, cPedPos.z,localPos.x, localPos.y, localPos.z,0,0,0,255);
			//int playerid = cPed->m_player_info()->m_player_id();
					
			/*
			auto pedHandle = g_pattern->pointer_to_handle(cPed);
			auto pedComp = PED::GET_PED_PROP_INDEX(pedHandle, 1);
			if (pedComp)
			{
				auto model = PED::GET_PED_PROP_TEXTURE_INDEX(pedHandle, 1);
				if (pedComp == 3)
					isStaff = true;
				if (model == 6) teamID = 1;
				if (model == 8) teamID == 2;
				if (model == 3) teamID = 3;
			}//red =6 || blue = 8 || green = 3
			*/
			auto distanceIn2d = get2ddistance(localPos.x, localPos.y, cPedPos.x, cPedPos.y);
			if(p_variables->radar3d.SpectateWarning)
			if (distanceIn2d < 0.5f && localPos.z - cPedPos.z > 9.0f && cPed != LocalPlayer)
			{
				std::string buff;
				if (p_variables->radar3d.name)
				{
					std::string bufff(cPedName);
					if (isStaff)
						buff = "^1STAFF ^3" + bufff + "^1 ,IS SPECTATING ON YOU";
					else
						buff = "^1CHECKING^3";
				}
				else
					buff = std::to_string(PedID) + "^1 ,IS SPECTATING ON YOU";
				g_Renderer->DrawText(ImGui::GetFont(), buff.c_str(), { middle_x,middle_y }, 30, ImColor(255, 0, 0), true, 3);
			}

			if (p_variables->radar3d.detectAdmin)
			if (!cPed->IsPedOrFalse() && cPed->isGodMode())
			{
				std::string bufff(cPedName);
				std::string buff;
				
				
				if(isStaff)
					buff = "^0STAFF ^3" + bufff + "^0 [" + std::to_string((int)distanceIn2d) + "m]";
				else if(!isInBase)
					buff = "^0GODMODE ON ^3" + bufff + "^0 [" + std::to_string((int)distanceIn2d) + "m]";
				float y = 300 + AdminNum * 20;
				if(distanceIn2d > 100.0f)
					g_Renderer->DrawText(ImGui::GetFont(), buff.c_str(), { 20,y}, 15, p_variables->p_color.Green, false, 3);
				else
					g_Renderer->DrawText(ImGui::GetFont(), buff.c_str(), { 20,y }, 15, p_variables->p_color.Red, false, 3);
				AdminNum++;
			}
			if (distanceIn2d < 100 && !isonscreen && !isFriend || isStaff)//circle radar
			{
				ImVec2 Delta = { cPedPos.x - localPos.x ,cPedPos.y - localPos.y };

				//auto dir = ImVec2(cos(angle), sin(angle));
				DWORD64 addr = g_pattern->GetCameraViewAngles();
				if (!addr) { /*Log::Msg("addy null");*/ continue; }
				Vector3 TPS = *(Vector3*)(addr + 0x03D0);

				float rot = acosf(TPS.x) * 180.0f / 3.14f;
				if (asinf(TPS.y) * 180.0f / 3.14f < 0.0f) rot *= -1.0f;
				float ForwardDirection = DirectX::XMConvertToRadians(rot);
				FLOAT CosYaw = cos(ForwardDirection);
				FLOAT SinYaw = sin(ForwardDirection);
				FLOAT LocationX = (Delta.y * CosYaw - Delta.x * SinYaw);
				FLOAT LocationY = (Delta.x * CosYaw + Delta.y * SinYaw);

				float angle = std::atan2(LocationY, LocationX);
				float playerX = middle_x - std::cos(angle) * 100;
				float playerY = middle_y - std::sin(angle) * 100;
				if(isStaff)
					g_Renderer->DrawArrowHead(playerX, playerY, angle, 20, p_variables->p_color.BlueViolet);
				else
					g_Renderer->DrawArrowHead(playerX, playerY, angle, 20, p_variables->p_color.Red);
			}
			if (p_variables->radar2d.p_enable2DRadar)
			{
				static bool initRadarW = false;

				if (!initRadarW)
				{
					HUD::SET_RADAR_ZOOM_PRECISE(94.f);
					p_variables->p_settings.p_2DRadarZoom = 2.665;
					ImGui::SetNextWindowPos({ 12, 841 });
					ImGui::SetNextWindowSize({ 297, 325 });
					initRadarW = true;
				}
				
				ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(30, 32, 48, 50));
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
					{
						ImGui::Begin("Minimap", &p_variables->radar2d.p_enable2DRadar,ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
						{
							auto windowSize = ImGui::GetWindowSize();
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							auto drawColor = ImColor(255, 255, 255);
							static int dwsize = 10;
							if (cPed == LocalPlayer)
								drawColor = p_variables->p_color.Blue;
							else if (cPed->IsPedOrFalse())
								drawColor = ImColor(255, 255, 255);
							else if (isStaff)
								drawColor = p_variables->p_color.BlueViolet;
							else if (isFriend)
								drawColor = p_variables->p_color.Green;
							else
								drawColor = p_variables->p_color.Red;

							//draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y), ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y), ImColor(30, 32, 48), 1.f);
							//draw_list->AddLine(ImVec2(winpos.x, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y + winsize.y * 0.5f), ImColor(30, 32, 48), 1.f);

							float RadarZoom = (4 * p_variables->p_settings.p_2DRadarZoom) * 0.2f;//94 

							ImVec2 Radar_Enemy_Pos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
							Radar_Enemy_Pos = g_Math->WorldToRadar(cPed->m_visual_pos(), Radar_Enemy_Pos.x, Radar_Enemy_Pos.y, windowSize.x, RadarZoom);
							auto pedVeh = cPed->m_vehicle_manager();
							auto drawPos = ImVec2(Radar_Enemy_Pos.x + windowSize.x / 2, Radar_Enemy_Pos.y + windowSize.y / 2);
							if (isDead)
								ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), dwsize, ImVec2(drawPos.x - dwsize / 2, drawPos.y - dwsize / 2), drawColor, ICON_FA_TIMES);
							else if (!pedVeh)
							{
								g_Renderer->WindowFillRect(drawPos.x - 1, drawPos.y - 1, 8 + 2, 8 + 2, ImColor(0, 0, 0), 20);
								g_Renderer->WindowFillRect(drawPos.x, drawPos.y, 8, 8, drawColor, 20);
							}
							else
							{
								auto model = pedVeh->modelInfo();
								if (model)
								{
									uintptr_t veh = g_pattern->pointer_to_handle((CPed*)pedVeh);
									if (veh)
									{
										int vehClass = VEHICLE::GET_VEHICLE_CLASS(veh);
										if (vehClass == 15)
											ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), dwsize, ImVec2(drawPos.x - dwsize / 2, drawPos.y - dwsize / 2), drawColor, ICON_FA_ARROWS);
										else if (vehClass == 16)
											ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), dwsize, ImVec2(drawPos.x - dwsize / 2, drawPos.y - dwsize / 2), drawColor, ICON_FA_PLANE);
										else
											ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), dwsize - 2, ImVec2(drawPos.x - dwsize / 2, drawPos.y - dwsize / 2), drawColor, ICON_FA_CAR);
									}
									
								}
								
							}
							
							//g_Renderer->WindowDrawHealthBar(Radar_Enemy_Pos.x - 10.0f, Radar_Enemy_Pos.y + 5.0f, 4, 4, health, 100, 0, ImColor(0, 255, 0), 1);
							//draw_list->AddRectFilled(ImVec2(Radar_Ped_Pos.x + pos.x / 2, Radar_Ped_Pos.y + pos.y / 2), 3, ImColor(255, 0, 0), 100);

						}
						ImGui::End();
					}
					ImGui::PopStyleVar();
				}
				ImGui::PopStyleColor();
			}
			
			if (!isonscreen) continue;
			if (isDead) continue;

			Vector3 base = p_variables->p_friend.BasePOs;
			if(p_variables->misc.autoFriendList && isInBase && cPed->isGodMode() && !isFriend)
				p_variables->p_friend.Friend.push_back(PedID);
			
			if (p_variables->misc.autoFriendList && base.x != 0 && !isFriend)
			{
				Vector3 vec3Diff(cPedPos.x - base.x, cPedPos.y - base.y, cPedPos.z - base.z);
				float disToBase = vec3Diff.Length();
				if (disToBase < 100)
					p_variables->p_friend.Friend.push_back(PedID);
			}
			bool Visible = true;
			if (p_variables->radar3d.visiblecheck && !isFriend)
			{
				Visible = g_pattern->HasEntityClearLosToEntity(LocalPlayer, cPed, 235677380);
			}


			Vector3 HeadPos(cPedPos.x, cPedPos.y, cPedPos.z + 0.8f);
			Vector3 FootPos(cPedPos.x, cPedPos.y, cPedPos.z - 0.95f);
			Vector3 FootPos1(cPedPos.x, cPedPos.y, cPedPos.z - 0.96f);
			Vector3 FootPos2(cPedPos.x, cPedPos.y, cPedPos.z - 0.97f);
			//continue;
			//ImVec2 PedPos = g_Math->WorldToScreen(PedPosVec3);
			//ImVec2 LocalPos = g_Math->WorldToScreen(LocalPosVec3);
			ImVec2 Head = g_Math->WorldToScreen(HeadPos);
			ImVec2 Foot = g_Math->WorldToScreen(FootPos);
			ImVec2 Foot1 = g_Math->WorldToScreen(FootPos1);
			ImVec2 Foot2 = g_Math->WorldToScreen(FootPos2);
			float h = Head.y - Foot.y;
			float w = (Foot.y - Head.y) / 4;

			float cornerw = (Foot.y - Head.y) * 0.55f;
			float cornerh = Foot.y - Head.y;
			float drawpos = (((Foot.y - Head.y) * 0.574f) / 2);

			//g_Renderer->DrawText(ImGui::GetFont(), std::to_string(pedinterface->m_pedmax()), ImVec2(Head.x, Head.y - 14), 14, p_variables->color.Id, 1);


			

			//RadarCoordinates radarCoords = transformToRadarCoordinates(cPedPos, Distance);
			//g_Renderer->DrawCircle(middle_x + radarCoords.coord.x,middle_y + radarCoords.coord.y, 2, p_variables->color.Red);


			if (Distance < p_variables->radar3d.p_renderDistance || p_variables->world.FreeCam)
			{
				if (p_variables->radar3d.p_Box)
				{
					ImColor cBOx = p_variables->color.Box;
					if(isFriend)
						cBOx = p_variables->p_color.Lime;
					if (Visible || isFriend)
					{
						g_Renderer->DrawBox({ Foot.x - w, Head.y }, { Foot.x + w, Foot.y }, cBOx, 0);
					}
					else
					{
						g_Renderer->DrawBox({ Foot.x - w, Head.y }, { Foot.x + w, Foot.y }, p_variables->color.InVisible, 0);
					}
				}
				if (p_variables->radar3d.p_corner_box)
				{
					float w = (Foot.y - Head.y) * 0.495f;
					float h = Foot.y - Head.y;
					float drawpos = (((Foot.y - Head.y) * 0.495f) / 2);
					ImColor cBOx = p_variables->color.Corner;
					if (isFriend)
						cBOx = p_variables->p_color.Lime;
					if (Visible || isFriend)
					{
						g_Renderer->DrawCornerBox(Foot.x - drawpos, Head.y, w, h, 1, 0.25f, cBOx);
					}
					else
					{
						g_Renderer->DrawCornerBox(Foot.x - drawpos, Head.y, w, h, 1, 0.25f, p_variables->color.InVisible);
					}
				}
				if (p_variables->radar3d.p_box3d)
				{
					if (Visible)
					{
						Vector3	box_3d_head = g_Math->GetBone(cPed, 0);
						g_Renderer->Draw3DBox(cPedPos, box_3d_head, FootPos, cPed->m_navigation_manager()->m_get_rot(), p_variables->color.Box3d, 0, 0, 0);
					}
					else
					{
						Vector3	box_3d_head = g_Math->GetBone(cPed, 0);
						g_Renderer->Draw3DBox(cPedPos, box_3d_head, FootPos, cPed->m_navigation_manager()->m_get_rot(), p_variables->color.InVisible, 0, 0, 0);
					}
				}
				if (p_variables->radar3d.p_Skel && !isFriend)
				{
					//__m128 Head_Pos = g_pattern->GET_ENTITY_BONE_INDEX_BY_NAME(cPed, "SKEL_Head"); 
				//	Vector3 Head_Pos = g_pattern->GET_WORLD_POSITION_OF_ENTITY_BONE(cPed, 0x796E);
				//	ImVec2 Head_w2s = g_Math->WorldToScreen(Head_Pos);
				//	g_Renderer->FillRect(Head_w2s.x, Head_w2s.y, 5, 5, ImColor(255, 0, 0)); 
					float width = 3.0f;
					//	ImVec2 Rect = g_Math->GetBone_W2S(cPed, p_variables->radar3d.bones); 
					ImVec2 Head, Neck, Pelvis, RFoot, LFoot, LHand, RHand;

					Head = g_Math->GetBone_W2S(cPed, ePedBoneType::HEAD);
					Neck = g_Math->GetBone_W2S(cPed, ePedBoneType::NECK);
					Pelvis = g_Math->GetBone_W2S(cPed, ePedBoneType::ABDOMEN);
					RFoot = g_Math->GetBone_W2S(cPed, ePedBoneType::R_FOOT);
					LFoot = g_Math->GetBone_W2S(cPed, ePedBoneType::L_FOOT);
					LHand = g_Math->GetBone_W2S(cPed, ePedBoneType::L_HAND);
					RHand = g_Math->GetBone_W2S(cPed, ePedBoneType::R_HAND);



					//g_Renderer->FillRect(Rect.x, Rect.y, 5, 5, ImColor(255, 0, 0), 0); 
					if (Visible)
					{
						ImColor abgr = p_variables->p_color.Skeleton;
						int cicleR = (Neck.y - Head.y) * 0.90f;
						g_Renderer->Rect(Head.x - cicleR, Head.y - cicleR, cicleR * 2, cicleR * 2, abgr, 100);
						//ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), cicleR*2, ImVec2(Head.x - cicleR, Head.y - cicleR), abgr, ICON_FA_SMILE, (const char*)0,2.0f);
						//g_Renderer->DrawCircle(Head.x, Head.y, cicleR, abgr);

						//g_Renderer->DrawLineV(Head, Neck, abgr, width);
						g_Renderer->DrawLine(Neck, RHand, abgr, 1.0f);

						g_Renderer->DrawLine(Neck, LHand, abgr, 1.0f);

						g_Renderer->DrawLine(Neck, Pelvis, abgr, 1.0f);
						g_Renderer->DrawLine(Pelvis, LFoot, abgr, 1.0f);
						g_Renderer->DrawLine(Pelvis, RFoot, abgr, 1.0f);
					}
					else
					{
						ImColor abgr = p_variables->color.InVisible;
						int cicleR = (Neck.y - Head.y) * 0.90f;
						g_Renderer->Rect(Head.x - cicleR, Head.y - cicleR, cicleR * 2, cicleR * 2, abgr, 100);

						//g_Renderer->DrawLine(Head, Neck, abgr, width);
						g_Renderer->DrawLineV(Neck, RHand, abgr, width);

						g_Renderer->DrawLineV(Neck, LHand, abgr, width);

						g_Renderer->DrawLineV(Neck, Pelvis, abgr, width);
						g_Renderer->DrawLineV(Pelvis, LFoot, abgr, width);
						g_Renderer->DrawLineV(Pelvis, RFoot, abgr, width);
					}
				}
				if (p_variables->radar3d.p_Health)
				{
					if (health > 0)
					{
						ImColor HealthColor;
						if (health > 75) {
							HealthColor = ImColor(111, 203, 48);
						}
						else if (health < 75 && health > 50) {
							HealthColor = ImColor(172, 187, 47);
						}
						else if (health < 50 && health > 25) {
							HealthColor = ImColor(255, 140, 0);
						}
						else if (health < 25 && health > 15) {
							HealthColor = ImColor(255, 69, 0);
						}
						else if (health < 15 && health > 0)
						{
							HealthColor = ImColor(255, 0, 0);
						}
						if (p_variables->radar3d.p_Health_Bar_Type == 0)
						{
							if (health > 100.f)health -= 100;
							float Healthmath = (h / 100) * health;

							g_Renderer->DrawBorder(Foot.x - w - 7, Foot.y + 1, 3, h - 2, 1, ImColor(0, 0, 0));
							g_Renderer->FillRect(Foot.x - w - 6, Foot.y, 2, Healthmath, HealthColor);
						}
						else if (p_variables->radar3d.p_Health_Bar_Type == 1)
						{
							if (!p_variables->radar3d.p_box3d && !p_variables->radar3d.p_corner_box && !p_variables->radar3d.p_box3d)
							{
								float w = (Foot.y - Head.y) / 2.032f;
								float thiccerW = w * 2.0f;
							}
							else
							{
								float w = (Foot1.y - Head.y) / 2.032f;
								float thiccerW = w * 2.0f;
							}

						}

					}
				}
				if (p_variables->radar3d.p_Armorbar)
				{
					if (armor > 0)
					{
						if (p_variables->radar3d.p_Health_Bar_Type == 0)
						{
							if (p_variables->radar3d.p_Health_Bar_Type == 0)
							{
								float step = (h / 100);
								float draw = (step * armor);
								g_Renderer->DrawBorder(Foot.x - w - 12, Foot.y + 1, 3, h - 2, 1, ImColor(0, 0, 0));
								g_Renderer->FillRect(Foot.x - w - 11, Foot.y, 2, h, ImColor(0, 0, 0));
								g_Renderer->FillRect(Foot.x - w - 11, Foot.y, 2, draw, p_variables->color.Armor);
							}
							else
							{
								float step = (h / 100);
								float draw = (step * armor);
								g_Renderer->DrawBorder(Foot.x - w - 7, Foot.y + 1, 3, h - 2, 1, ImColor(0, 0, 0));
								g_Renderer->FillRect(Foot.x - w - 6, Foot.y, 2, h, ImColor(0, 0, 0));
								g_Renderer->FillRect(Foot.x - w - 6, Foot.y, 2, draw, p_variables->color.Armor);
							}
						}
						if (p_variables->radar3d.p_Health_Bar_Type == 1)
						{
							if (p_variables->radar3d.p_Health_Bar_Type == 1)
							{
								if (!p_variables->radar3d.p_Box && !p_variables->radar3d.p_corner_box && !p_variables->radar3d.p_box3d)
								{
									float w = (Foot1.y - Head.y) / 2.032f;
									float thiccerW = w * 2.0f;

								}
								else
								{
									float w = (Foot2.y - Head.y) / 2.032f;
									float thiccerW = w * 2.0f;

								}
							}
						}
						else
						{
							if (!p_variables->radar3d.p_box3d && !p_variables->radar3d.p_corner_box && !p_variables->radar3d.p_box3d)
							{
								if (p_variables->radar3d.p_Health_Bar_Type == 1)
								{
									float w = (Foot.y - Head.y) / 2.032f;
									float thiccerW = w * 2.0f;
								}
							}
							else
							{
								if (p_variables->radar3d.p_Health_Bar_Type == 1)
								{
									float w = (Foot1.y - Head.y) / 2.032f;
									float thiccerW = w * 2.0f;
								}
							}
						}
					}
				}
				
				std::string comboText;
				if (p_variables->radar3d.id)
				{
					comboText = "^7[^5ID:^3" + std::to_string(PedID) + "^7]";
				}///////////////^7[^5ID:^3123^2][^5Name:^0[LC]^3Mr.x8^2]
				
				if (p_variables->radar3d.name)
				{
					std::string strName(cPedName);
					//std::transform(strName.begin(), strName.end(), strName.begin(),
					//	[](unsigned char c) { return std::toupper(c); });
					//comboText.append(strName);
					comboText += strName;
				}
				char disstr[20];
				if (p_variables->radar3d.p_Distance)
				{
					sprintf_s(disstr, "^7[^0%dm^7]", (WORD)Distance);
					comboText.append(disstr);
				}
					
				if (isFriend)
					g_Renderer->DrawText(ImGui::GetFont(), comboText, ImVec2(Head.x, Head.y - 31), 15, p_variables->p_color.Green, 1, 3);
				//else if(Visible)
					//g_Renderer->DrawText(ImGui::GetFont(), comboText, ImVec2(Head.x, Head.y - 31), 15, p_variables->p_color.Red, 1, 3);
				else
					g_Renderer->DrawText(ImGui::GetFont(), comboText, ImVec2(Head.x, Head.y - 31), 15, p_variables->color.NameTags, 1, 3);
				{
					int drawY = 15;

					if (!p_variables->radar3d.id) drawY = 15;
					else drawY = 29;
					if(teamID == 1)
						g_Renderer->DrawText(ImGui::GetFont(), comboText.c_str(), ImVec2(Head.x, Head.y - 30), 15, p_variables->p_color.Red, 1);
					if(teamID == 2)
						g_Renderer->DrawText(ImGui::GetFont(), comboText.c_str(), ImVec2(Head.x, Head.y - 30), 15, p_variables->p_color.Blue, 1);
					if(teamID == 3)
						g_Renderer->DrawText(ImGui::GetFont(), comboText.c_str(), ImVec2(Head.x, Head.y - 30), 15, p_variables->p_color.Green, 1);

					
				}
				if (1)
				{
					std::string drawStr;
					std::string isStaff_ = isStaff ? "^2STAFF^7|" : "";
					std::string isGM = cPed->isGodMode() ? "^3GM^7|" : "";
					std::string isAttkerStr = isAttacker ? "^1Attacker|" : "";
					std::string isInvis_ = isInVisible ? "^5INVS" : "";
					
					if(isStaff || isInVisible || cPed->isGodMode() || isAttacker)
						drawStr += "^7[^1" + isStaff_ + isGM + isAttkerStr + isInvis_ + "^7]";
					
					//^7[^2STAFF^7|^3GM^7|^5INVS^7]^7[^8UNARMED^7]
					if (p_variables->radar3d.p_Weapon)
					{
						if (!cPed->m_weapon_manager())continue;
						if (!cPed->m_weapon_manager()->m_weapon_info())continue;
						std::string weaName = cPed->m_weapon_manager()->m_weapon_info()->getweaponname();
						if(weaName != "UNARMED")
							drawStr += "^7[^0" + weaName + "^7]";
					}
					if (!drawStr.empty())
					{
						if (Visible)
							g_Renderer->DrawText(ImGui::GetFont(), drawStr, ImVec2(Head.x, Head.y - 16), 15, p_variables->color.Weapon, 1, 3);
						else
							g_Renderer->DrawText(ImGui::GetFont(), drawStr, ImVec2(Head.x, Head.y - 16), 15, p_variables->color.Id, 1, 3);
					}
					
					
				}
				if (p_variables->ped_esp.p_Snapline && !isFriend)
				{
					if (Visible)
						g_Renderer->DrawLine(ImVec2(middle_x, DisplaySizey - 100), w2sPed, p_variables->color.Snaplines, 1);
					//else
						//g_Renderer->DrawLine(ImVec2(middle_x, DisplaySizey - 100), w2sPed, p_variables->color.InVisible, 1);



					if (!g_Renderer->isonscreen(w2sPed))
					{
						static float ScreenCX = ImGui::GetIO().DisplaySize.x;
						static float ScreenCY = ImGui::GetIO().DisplaySize.y;
						if (w2sPed.x > ScreenCX) w2sPed.x = ScreenCX - 1;
						if (w2sPed.x < 1) w2sPed.x = 1;
						if (w2sPed.y > ScreenCY) w2sPed.y = ScreenCY - 1;
						if (w2sPed.y < 1) w2sPed.y = 1;
						if (Visible)
							ImGui::GetBackgroundDrawList()->AddLine(ImVec2(middle_x, DisplaySizey - 200), w2sPed, p_variables->color.PedESP, 1);
						//g_Renderer->DrawLine(ImVec2(middle_x, DisplaySizey - 100), w2sPed, p_variables->color.PedESP, 1);
					//else
						//ImGui::GetBackgroundDrawList()->AddLine(ImVec2(middle_x, DisplaySizey-200), w2sPed, p_variables->color.InVisible, 1);
						//g_Renderer->DrawLine(ImVec2(middle_x, DisplaySizey - 100), w2sPed, p_variables->color.InVisible, 1);

					}
				}


			}
		}

	}
	if (p_variables->radar3d.saveLocation)
	{
		CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
		if (!ped_factory)
			return;
		CPed* LocalPlayer = ped_factory->m_local_ped();
		for (int i = 0; i < locationManager->locations.size(); ++i) {
			const Location& loc = locationManager->locations[i];
			Vector3 cObjPos = loc.coord;
			ImVec2 w2sObj = g_Math->WorldToScreen(cObjPos);
			//Vector3 DistanceCalculation = (LocalPlayer->m_visual_pos() - cObjPos);
			int Distance = (LocalPlayer->m_visual_pos() - cObjPos).Length();
			std::string DistanceString = "^7[^5 " + loc.name + "^7|^2" + std::to_string(Distance) + "m ^7]";
			g_Renderer->DrawText(ImGui::GetFont(), DistanceString.c_str(), w2sObj, 12, p_variables->color.Vehicle_Names, 1);
			ImGui::GetBackgroundDrawList()->AddCircleFilled(w2sObj, 4, p_variables->color.Vehicle_Box, 100);

		}
	}
	if (p_variables->object_esp.enable)//(p_variables->ped_esp.enable)
	{
		CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
		if (!ped_factory)
			return;
		CPed* LocalPlayer = ped_factory->m_local_ped();


		CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
		if (!ReplayInterface)
			return;

		CObjectInterface* objinterface = ReplayInterface->m_objectinterface();
		if (!objinterface)
			return;
		static std::vector<UINT32> listobj = { 1234788901,90805875,0};
		for (int i = 0; i < objinterface->max_objects; i++)
		{
			CObject* cObj = objinterface->list_obj->m_getobject(i);
			if (!cObj) continue;
			//auto it = std::find(listobj.begin(), listobj.end(), cObj->hash());
			//if (it == listobj.end())
			//	continue;

			Vector3 cObjPos = cObj->location();

			ImVec2 w2sObj = g_Math->WorldToScreen(cObjPos);
			//Vector3 DistanceCalculation = (LocalPlayer->m_visual_pos() - cObjPos);


			double Distance = (LocalPlayer->m_visual_pos() - cObjPos).Length();


			if (Distance < p_variables->object_esp.render_distance)
			{
				if (p_variables->object_esp.snaplines)
				{
					g_Renderer->DrawLine(ImVec2(middle_x, DisplaySizey), w2sObj, p_variables->color.PedESP, 1);
				}
				if (p_variables->object_esp.hashes)
				{
					g_Renderer->DrawText(ImGui::GetFont(), std::to_string(cObj->hash()).c_str(), w2sObj, 12, p_variables->p_color.BlueViolet, 1);
				}
				if (cObj->hash() == 1234788901)
				{
					g_Renderer->DrawText(ImGui::GetFont(), "LOOT BAG", { w2sObj.x,w2sObj.y + 12 }, 12, p_variables->p_color.BlueViolet, 1);
				}
				if (cObj->hash() == 90805875)
				{
					g_Renderer->DrawText(ImGui::GetFont(), "AIR DROP", { w2sObj.x,w2sObj.y + 12 }, 12, p_variables->p_color.BlueViolet, 1);
				}

				if (p_variables->object_esp.lootbag)
				{
					ImGui::GetBackgroundDrawList()->AddCircleFilled(w2sObj, 5, p_variables->p_color.BlueViolet, 100);
				}
			}

		}
	}
	if (p_variables->vehicle_esp.enable)
	{
		if (p_variables->vehicle_esp.enable)
		{
			CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
			if (!ped_factory)
				return;
			CPed* LocalPlayer = ped_factory->m_local_ped();


			CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);
			if (!ReplayInterface)
				return;

			CVehicleInterface* vehicle_interface = ReplayInterface->m_vehicleinterface();
			if (!vehicle_interface)
				return;
			static const char* filters[]{ "WARTHOG", "RHINO", "AMTRAK", "M3A3", "BTR82A", "bell360", "T72B3","type75","f16c"};
			static UINT32 filterHashes[]{ rage::joaat("WARTHOG"), rage::joaat("RHINO") ,rage::joaat("AMTRAK") ,rage::joaat("M3A3") 
				,rage::joaat("BTR82A") ,rage::joaat("bell360") ,rage::joaat("T72B3") ,rage::joaat("type75") ,rage::joaat("f16c") };
			for (int i = 0; i < vehicle_interface->m_vehiclemax(); i++)
			{
				CVehicleManager* cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(i);
				if (!cVehicle) continue;
				Vector3 cPedPos = cVehicle->m_visual_pos();
				cPedPos.z -= 2.0f;
				ImVec2 w2sPed = g_Math->WorldToScreen(cPedPos);
				
				//Vector3 DistanceCalculation = (LocalPlayer->m_visual_pos() - cPedPos);

				double Distance = (LocalPlayer->m_visual_pos() - cPedPos).Length();

				if (Distance < p_variables->vehicle_esp.render_distance)
				{
					auto base_info = cVehicle->modelInfo();
					if (!base_info)continue;
					bool inFilterList = false;
					for (int i = 0; i < IM_ARRAYSIZE(filters); i++) {
						auto vehName = filters[i];
						if (filterHashes[i] == base_info->Hash)
						{
							inFilterList = true;
							break;
						}
					}
					if (p_variables->vehicle_esp.filter && !inFilterList)
						continue;
					else//alway keep on sight
					{
						if (w2sPed.x < 0) w2sPed.x = 20;
						if (w2sPed.y < 0) w2sPed.y = 20;
						if (w2sPed.x > DisplaySizex) w2sPed.x = DisplaySizex-20;
						if (w2sPed.y > DisplaySizey) w2sPed.y = DisplaySizey - 20;
					}
					w2sPed.y -= 10;
					if (p_variables->vehicle_esp.box)
					{
						ImGui::GetBackgroundDrawList()->AddCircleFilled(w2sPed, 4, p_variables->color.Vehicle_Box, 100);
					}
					std::string buffer;
					if (p_variables->radar3d.id)
					{
						buffer = "^7[^5ID:^3" + std::to_string(i) + "^7]^0";
					}
					if (p_variables->vehicle_esp.names)
					{
						auto vehhealth = (int)cVehicle->body_health();
						buffer += base_info->Name;
						
						buffer = buffer + "^7[^0" + std::to_string(vehhealth) + "^7]";
						g_Renderer->DrawText(ImGui::GetFont(), buffer, w2sPed, 12.5f, p_variables->color.Vehicle_Names, 1, 3);
					}
					if (p_variables->vehicle_esp.snaplines)
					{
						g_Renderer->DrawLine(ImVec2(middle_x, DisplaySizey), w2sPed, p_variables->color.Vehicle_Snaplines, 1);
					}
				}

			}
		}
	}
	
	ImGui::PopFont();
}