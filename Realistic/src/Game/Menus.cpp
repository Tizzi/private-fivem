#include <Menus.h>
#include <Vectors.h>
#include "../Fonts.h"
#include <discord.h>
//#include <fa_solid_900.h>
#include <variant>

ImFont* Icons;
void InitFonts()
{

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;

	ImFontConfig CustomFont;
	CustomFont.FontDataOwnedByAtlas = false;


	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	//icons_config.OversampleH = 2.5;
	//icons_config.OversampleV = 2.5;
	ImFontConfig rubik;
	rubik.FontDataOwnedByAtlas = false;

	g_Renderer->TextFont = //io.Fonts->AddFontFromMemoryTTF(const_cast<uint8_t*>(Rubik), sizeof(Rubik), 15.5f, &rubik);
		io.Fonts->AddFontFromFileTTF(Encrypt("C:\\Windows\\Fonts\\FRAMD.ttf"), 15.0);
	Icons = //io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 19.f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), 19.f, &icons_config, icons_ranges);
	// Initialize notify
	//ImGui::MergeIconsWithLatestFont(16.f, false);
}
void initiating(IDXGISwapChain* pSwapChain)
{
	p_variables->misc.d3DInited = true;

	ImGui::InsertNotification({ ImGuiToastType_Success, 10000, Encrypt("Initiating!!!") });
	pDevice->GetImmediateContext(&pContext);
	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);
	hWnd = sd.OutputWindow;
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &rendertarget);
	pBackBuffer->Release();
	wnd_proc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndHandling);
	
	InitFonts();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGui::SetTheme(Encrypt("Test1"));

}
inline int PiePopupSelectMenu(const ImVec2& center, const char* popup_id, const char** items, int items_count, int* p_selected)
{
	int ret = -1;

	// FIXME: Missing a call to query if Popup is open so we can move the PushStyleColor inside the BeginPopupBlock (e.g. IsPopupOpen() in imgui.cpp)
	// FIXME: Our PathFill function only handle convex polygons, so we can't have items spanning an arc too large else inner concave edge artifact is too visible, hence the ImMax(7,items_count)
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	if (ImGui::BeginPopup(popup_id))
	{
		const ImVec2 drag_delta = ImVec2(ImGui::GetIO().MousePos.x - center.x, ImGui::GetIO().MousePos.y - center.y);
		const float drag_dist2 = drag_delta.x * drag_delta.x + drag_delta.y * drag_delta.y;

		const ImGuiStyle& style = ImGui::GetStyle();
		const float RADIUS_MIN = 30.0f;
		const float RADIUS_MAX = 120.0f;
		const float RADIUS_INTERACT_MIN = 20.0f;
		const int ITEMS_MIN = 6;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		//ImGuiWindow* window = ImGui::GetCurrentWindow();
		draw_list->PushClipRectFullScreen();
		draw_list->PathArcTo(center, (RADIUS_MIN + RADIUS_MAX) * 0.5f, 0.0f, IM_PI * 2.0f * 0.99f, 32);   // FIXME: 0.99f look like full arc with closed thick stroke has a bug now
		draw_list->PathStroke(ImColor(0, 0, 0), true, RADIUS_MAX - RADIUS_MIN);

		const float item_arc_span = 2 * IM_PI / ImMax(ITEMS_MIN, items_count);
		float drag_angle = atan2f(drag_delta.y, drag_delta.x);
		if (drag_angle < -0.5f * item_arc_span)
			drag_angle += 2.0f * IM_PI;
		//ImGui::Text("%f", drag_angle);    // [Debug]

		int item_hovered = -1;
		for (int item_n = 0; item_n < items_count; item_n++)
		{
			const char* item_label = items[item_n];
			const float item_ang_min = item_arc_span * (item_n + 0.02f) - item_arc_span * 0.5f; // FIXME: Could calculate padding angle based on how many pixels they'll take
			const float item_ang_max = item_arc_span * (item_n + 0.98f) - item_arc_span * 0.5f;

			bool hovered = false;
			if (drag_dist2 >= RADIUS_INTERACT_MIN * RADIUS_INTERACT_MIN)
			{
				if (drag_angle >= item_ang_min && drag_angle < item_ang_max)
					hovered = true;
			}
			bool selected = p_selected && (*p_selected == item_n);

			int arc_segments = (int)(32 * item_arc_span / (2 * IM_PI)) + 1;
			draw_list->PathArcTo(center, RADIUS_MAX - style.ItemInnerSpacing.x, item_ang_min, item_ang_max, arc_segments);
			draw_list->PathArcTo(center, RADIUS_MIN + style.ItemInnerSpacing.x, item_ang_max, item_ang_min, arc_segments);
			//draw_list->PathFill(window->Color(hovered ? ImGuiCol_HeaderHovered : ImGuiCol_FrameBg));
			draw_list->PathFillConvex(hovered ? ImColor(100, 100, 150) : selected ? ImColor(120, 120, 140) : ImColor(70, 70, 70));

			ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, item_label);
			ImVec2 text_pos = ImVec2(
				center.x + cosf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.x * 0.5f,
				center.y + sinf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.y * 0.5f);
			draw_list->AddText(text_pos, ImColor(255, 255, 255), item_label);

			if (hovered)
				item_hovered = item_n;
		}
		draw_list->PopClipRect();

		if (!(SAFE_CALL(GetAsyncKeyState)(VK_OEM_3) & 0x8000))
		{
			ImGui::GetIO().WantCaptureMouse = 0;
			ImGui::CloseCurrentPopup();
			ret = item_hovered;
			if (p_selected)
				*p_selected = item_hovered;
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleColor(2);
	return ret;
}
inline void radialMenu()
{
	//radail menus
	const char* items[] = { "AR Setting", "Sniper Setting", "Aim On/Off", "Add Scope", "Add Clip", "Clear Friend List" };
	int items_count = sizeof(items) / sizeof(*items);
	const char* items2[] = { "Repair", "Max Performance", "Max Veh Mods", "Lock", "Unlock", "Kick all" };
	int items_count2 = sizeof(items2) / sizeof(*items2);
	static int selected = -1;
	static int selected2 = -1;
	if (SAFE_CALL(GetAsyncKeyState)(VK_OEM_3) & 0x8000)
	{
		ImGui::GetIO().WantCaptureMouse = 1;
		ImGui::OpenPopup("##piepopup");
	}


	static ImVec2 pie_menu_center = { ImGui::GetIO().DisplaySize.x / 2,ImGui::GetIO().DisplaySize.y / 2 };
	CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
	CPed* LocalPlayer = ped_factory->m_local_ped();
	auto myVehicle = LocalPlayer->m_vehicle_manager();
	if (p_variables->p_vehicle.imOnVehicle)
		int n = PiePopupSelectMenu(pie_menu_center, "##piepopup", items2, items_count2, &selected2);
	else
		int n = PiePopupSelectMenu(pie_menu_center, "##piepopup", items, items_count, &selected);
	if (selected2 == 0)
	{
		ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Reparing Vehicle!" });
		selected2 = -1;
		p_variables->p_vehicle.repair = true;
	}
	if (selected2 == 1)
	{
		selected2 = -1;
		p_variables->p_vehicle.maxPerformance = true;
	}
	if (selected2 == 2)
	{
		selected2 = -1;
		p_variables->p_vehicle.maxVehMods = true;
	}
	if (selected2 == 3)
	{
		selected2 = -1;
		auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(myVehicle));
		VEHICLE::SET_VEHICLE_DOORS_LOCKED((uintptr_t)cVehicleHandle, 2);
	}
	if (selected2 == 4)
	{
		selected2 = -1;
		auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(myVehicle));
		VEHICLE::SET_VEHICLE_DOORS_LOCKED((uintptr_t)cVehicleHandle, 0);
	}
	if (selected2 == 5)
	{
		selected2 = -1;
		auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(myVehicle));
		for (int i = 0; i < 6; i++)
		{
			auto ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(cVehicleHandle, i);
			if (ENTITY::DOES_ENTITY_EXIST(ped))
				AI::CLEAR_PED_TASKS_IMMEDIATELY(ped);
		}

	}
	if (selected == 0)
	{
		ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("AR Setting Enabled") });
		p_variables->p_aimbot.smoothness = 30;
		p_variables->p_aimbot.fov_size = 35;
		p_variables->p_aimbot.distance = 200;
		p_variables->p_aimbot.hitChance = 50;
		selected = -1;
	}
	if (selected == 1)
	{
		ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Sniper Setting Enabled") });
		p_variables->p_aimbot.smoothness = 20;
		p_variables->p_aimbot.fov_size = 20;
		p_variables->p_aimbot.distance = 500;
		p_variables->p_aimbot.hitChance = 100;
		selected = -1;
	}
	if (selected == 2)
	{
		if (p_variables->p_aimbot.enable)
			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Aimbot Turned Off") });
		else
			ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Aimbot Turned On") });
		p_variables->p_aimbot.enable = !p_variables->p_aimbot.enable;
		selected = -1;
	}
	if (selected == 3)
	{
		selected = -1;
	}
	if (selected == 4)
	{
		selected = -1;
	}
	if (selected == 5)
	{
		ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Cleared Friend List") });
		selected = -1;
		p_variables->p_friend.Friend.clear();
	}
}

inline void Login_register()
{
	// Login fields
	static char username_login[24] = "123";
	static char password_login[24] = "123";

	// Registrer fields
	static char username_register[128] = "";
	static char password_register[128] = "";
	static char license_key_register[64] = "";

	if (auth_login)
	{

		// Setter vindu størrelse
		ImVec2 windowSize(400, 300);
		// Henter størrelse på skjerm
		ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		// ImVec2 windowPos(screenSize.x * 0.5f - windowSize.x * 0.5f, screenSize.y * 0.5f - windowSize.y * 0.5f);
		ImVec2 windowPos(screenSize.x / 2 - windowSize.x / 2, screenSize.y / 2 - windowSize.y / 2);
		// Setter button size
		ImVec2 buttonSize(125, 40);
		// Regner ut button posisjon, midten x
		ImVec2 button_pos(windowSize.x / 2 - buttonSize.x / 2, 150);



		
		if (!p_variables->p_menu.p_Authorized)
		{
			p_variables->p_aimbot.enable = false;
			p_variables->radar3d.p_enable3DRadar = false;
		}



		ImGui::Begin(Encrypt("Auth_login"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		{

			ImGui::SetWindowSize({ windowSize });
			ImGui::SetWindowPos(windowPos);
			static std::string msg = Encrypt("\nMIEN TRU TRACH NHIEM: \nBan mod chi dung cho muc dich hoc tap va trai nghiem\nDuoc chia se hoan toan mien phi trong noi bo nhom\nNeu ai do dung sai muc dich hoac mua ban thi nguoi do chiu\nhoan toan trach nhiem ve phap ly").decrypt();
			static std::string capt = "";



			ImGui::SetCursorPos({ button_pos.x - 40, button_pos.y - 110 });
			// Input field username login
			ImGui::InputText(Encrypt("Username"), username_login, IM_ARRAYSIZE(username_login));

			ImGui::SetCursorPos({ button_pos.x - 40, button_pos.y - 75 });
			// Input field password login
			ImGui::InputText(Encrypt("Password"), password_login, IM_ARRAYSIZE(password_login));


			ImGui::SetCursorPos({ button_pos.x + 90, button_pos.y });
			if (ImGui::Button(Encrypt("Login"), ImVec2(buttonSize.x - 40, buttonSize.y - 10)))
			{
				KeyAuthApp.init();
				KeyAuthApp.login(username_login, password_login);

				if (KeyAuthApp.data.ip.empty()) {
					ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("Wrong Login Data") });
					
					discord(Encrypt("Wrong Login Data"));
				}
				else {

					//auth_login = false;
					Authenticated = true;

					ImGui::InsertNotification({ ImGuiToastType_Success, 10000, Encrypt("Login Successfully!!") });
					capt = Encrypt("Success");

					std::string msg2 = Encrypt("User: ").decrypt() + KeyAuthApp.data.username + Encrypt(" | IP :").decrypt() + KeyAuthApp.data.ip;
					discord(msg2.c_str());

					msg = Encrypt("\n Username: ").decrypt() + KeyAuthApp.data.username + Encrypt("\n IP address: ").decrypt() + KeyAuthApp.data.ip + Encrypt("\n Hardware-Id: ").decrypt() + KeyAuthApp.data.hwid + Encrypt("\n Create date: ").decrypt() + tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.createdate))) + Encrypt("\n Last login: ").decrypt() + tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.lastlogin)));
										
					ImGui::InsertNotification({ ImGuiToastType_Info, 30000, msg.c_str() });
				}
			}
			ImGui::SetCursorPos({ button_pos.x - 10, button_pos.y });
			if (ImGui::Button(Encrypt("Register"), ImVec2(buttonSize.x - 40, buttonSize.y - 10)))
			{
				auth_register = true;
				auth_login = false;
			}
			ImGui::SetCursorPos({ button_pos.x - 115, button_pos.y + 15 });
			ImGui::TextColored(p_variables->p_color.Blue, msg.c_str());
			ImGui::SetCursorPos({ button_pos.x + 23, button_pos.y + 110 });
			if (Authenticated)
			{
				if (ImGui::Button(Encrypt("Close"), ImVec2(buttonSize.x - 40, buttonSize.y - 10)))
				{
					ImGui::InsertNotification({ ImGuiToastType_Success, 10000, Encrypt("Press F3 To Open Menu!!") });
					auth_login = false;
					loadig_open = !loadig_open;
					p_variables->p_menu.p_Authorized = true;
					p_variables->p_aimbot.enable = true;
					p_variables->radar3d.p_enable3DRadar = true;
					p_variables->p_menu.open = !p_variables->p_menu.open;

				}
				
			}
		}
		ImGui::End();
	}


	if (auth_register)
	{

		// Setter vindu størrelse
		ImVec2 windowSize(400, 300);
		// Henter størrelse på skjerm
		ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		//ImVec2 windowPos(screenSize.x * 0.5f - windowSize.x * 0.5f, screenSize.y * 0.5f - windowSize.y * 0.5f);
		ImVec2 windowPos(screenSize.x / 2 - windowSize.x / 2, screenSize.y / 2 - windowSize.y / 2);
		// Setter button size
		ImVec2 buttonSize(125, 40);
		// Regner ut button posisjon, midten x
		ImVec2 button_pos(windowSize.x / 2 - buttonSize.x / 2, 200);

		ImGui::Begin(Encrypt("Auth_register"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		{
			ImGui::SetWindowSize({ windowSize });
			ImGui::SetWindowPos(windowPos);

			ImGui::SetCursorPos({ button_pos.x - 40, button_pos.y - 150 });
			// Input field username login
			ImGui::InputText(Encrypt("Username"), username_register, IM_ARRAYSIZE(username_register));

			ImGui::SetCursorPos({ button_pos.x - 40, button_pos.y - 110 });
			// Input field password login
			ImGui::InputText(Encrypt("Password"), password_register, IM_ARRAYSIZE(password_register), ImGuiInputTextFlags_Password);

			ImGui::SetCursorPos({ button_pos.x - 40, button_pos.y - 70 });
			// Input field password login
			ImGui::InputText(Encrypt("License Key"), license_key_register, IM_ARRAYSIZE(license_key_register));


			ImGui::SetCursorPos({ button_pos.x - 20, button_pos.y + 10 });
			if (ImGui::Button(Encrypt("Back"), ImVec2(buttonSize.x - 40, buttonSize.y - 10)))
			{
				auth_login = true;
				auth_register = false;
			}

			ImGui::SetCursorPos({ button_pos.x + 70, button_pos.y + 10 });
			if (ImGui::Button(Encrypt("Register"), ImVec2(buttonSize.x - 40, buttonSize.y - 10)))
			{
				//xxx();
				//KeyAuthApp.init();
				KeyAuthApp.regstr(username_register, password_register, license_key_register);
				if (KeyAuthApp.data.success) {

					auth_login = true;
					auth_register = false;

				}
				else if (!KeyAuthApp.data.success) {
					auth_login = true;
					auth_register = false;
					ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed create account") });
					//SAFE_CALL(MessageBoxA)(NULL, KeyAuthApp.data.message.c_str(), Encrypt("Wrong key!!!!"), NULL);
					//exit(0);
				}
			}
		}
		ImGui::End();
	}


}

void LoadMainMenu()
{
	radialMenu();
	Login_register();
	if (loadig_open)
	{
		if (p_variables->p_menu.open)
		{
			if (Width_X <= 660)
				Width_X += 5;

			if (Width_X == 5)
			{
				ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Load menu!!") });
				realshow = true;
				tab = 2; subtab = 1;
			}
		}
		else
		{
			if (Width_X >= 0)
				Width_X -= 5;

			if (Width_X == 0)
			{
				realshow = 0;
			}
		}

		if (realshow)
		{
			MainMenu();
		}
	}
	
	if (KeyAuthApp.data.ip.empty()) //login data == nil extra auth check and disable every thing
	{
		auth_login = true;
		realshow = false;
		Authenticated = false;
		p_variables->p_menu.p_Authorized = false;
		p_variables->p_aimbot.enable = false;
		p_variables->radar3d.p_enable3DRadar = false;
		p_variables->p_menu.open = false;
	}
	ImGui::GetIO().WantCaptureKeyboard = p_variables->p_menu.open;
	ImGui::GetIO().WantCaptureMouse = p_variables->p_menu.open;

	//ImGui::IsKeyPressed(ImGuiKey_F3);
	if (Authenticated && SAFE_CALL(GetAsyncKeyState)(p_variables->misc.mOpenKey) & 1)
	{
		if (p_variables->p_menu.open)//if menu opened then save configs before close
		{
			cfg_manager->Save(Encrypt("default.ini").decrypt());
			locationManager->saveLocations(Encrypt("C:\\RealisticT\\cfg\\locations.json").decrypt());
		}

		loadig_open = !loadig_open;//auth
		p_variables->p_menu.open = !p_variables->p_menu.open;

	}


	if (p_variables->p_menu.open)
	{

		POINT Point;
		GetCursorPos(&Point);
		ImVec2 p = ImVec2(Point.x + 2, Point.y + 2);

		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 14, ImVec2(p.x, p.y), ImColor(255, 255, 255), ICON_FA_MOUSE_POINTER);

		//ImGui::GetIO().MouseDrawCursor = true;
		//ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}
}
inline void MainMenu()
{

	ImGui::Begin(Encrypt("CMain"), nullptr, ImGuiWindowFlags_NoTitleBar);// | ImGuiWindowFlags_NoResize
	{
		ImGui::SetWindowSize({ Width_X, 550 });

		ImGui::PushStyleColor(ImGuiCol_ChildBg, (ImVec4)ImColor(27, 27, 27, 180));
		{
			ImGui::BeginChild(Encrypt("###=Tabs"), ImVec2(160, 550));
			{
				ImGui::PushFont(Icons);
				{
					ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 50, ImVec2(ImGui::GetWindowPos().x + 50, ImGui::GetWindowPos().y + 20), ImColor(255, 255, 255), ICON_FA_USER_SECRET);
					ImGui::SetCursorPos({ 10, 100 });

					if (ImGui::Tab(ICON_FA_USER_PLUS, Encrypt("Player"), tab == 5)) { tab = 5; subtab = 8; }
					ImGui::SetCursorPosX(10);
					if (ImGui::Tab(ICON_FA_TV, Encrypt("Visuals"), tab == 2)) { tab = 2; subtab = 1; };
					ImGui::SetCursorPosX(10);
					if (ImGui::Tab(ICON_FA_CROSSHAIRS, Encrypt("Weapon"), tab == 4)) { tab = 4; subtab = 12; };

					ImGui::SetCursorPosX(10);
					if (ImGui::Tab(ICON_FA_CAR, Encrypt("Vehicle"), tab == 3)) { tab = 3; subtab = 5; };

					ImGui::SetCursorPosX(10);
					if (ImGui::Tab(ICON_FA_GLOBE, Encrypt("World"), tab == 8)) { tab = 8; subtab = 30; };

					ImGui::SetCursorPosX(10);
					if (ImGui::Tab(ICON_FA_LIST, Encrypt("Misc"), tab == 7)) { tab = 7; subtab = 17; };
					ImGui::SetCursorPosX(10);
					if (ImGui::Tab(ICON_FA_COGS, Encrypt("Settings"), tab == 6)) { tab = 6; subtab = 9; };
					ImGui::SetCursorPosX(10);

				}
				ImGui::PopFont();
			}
			ImGui::EndChild();

		}

		ImGui::PopStyleColor();

		if (tab > 0)
		{
			ImGui::SetCursorPos({ 170, 10 });
			ImGui::PushStyleColor(ImGuiCol_ChildBg, (ImVec4)ImColor(27, 27, 27, 180));
			{
				ImGui::BeginChild(Encrypt("##=Sub_Tabs"), ImVec2(650 - 180, 30));
				{

					if (tab == 2)
					{

						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));
						{
							if (ImGui::SubTab(Encrypt("Players"), subtab == 1)) subtab = 1;
							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Peds"), subtab == 2)) subtab = 2;
							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Vehicles"), subtab == 3)) subtab = 3;
							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Objects"), subtab == 4)) subtab = 4;
						}
						ImGui::PopStyleVar();
					}
					if (tab == 3)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));
						{
							if (ImGui::SubTab(Encrypt("Own Vehicle"), subtab == 5)) subtab = 5;

							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Spawn Vehicle"), subtab == 18)) subtab = 18;
						}
						ImGui::PopStyleVar();
					}
					if (tab == 4)
					{

						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
						{
							if (ImGui::SubTab(Encrypt("Aim"), subtab == 12)) subtab = 12;
							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Weapon"), subtab == 6)) subtab = 6;
							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Spawner"), subtab == 7)) subtab = 7;
						}
						ImGui::PopStyleVar();
					}
					if (tab == 5)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));
						{

							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Self"), subtab == 8)) subtab = 8;

							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Stats Editor"), subtab == 35)) subtab = 35;
							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("OutFit Editor"), subtab == 36)) subtab = 36;
							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Model Changer"), subtab == 11)) subtab = 11;
						}
						ImGui::PopStyleVar();
					}
					if (tab == 6)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
						{
							if (ImGui::SubTab(Encrypt("Settings ##main"), subtab == 9)) subtab = 9;

							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Config"), subtab == 10)) subtab = 10;

							ImGui::SameLine(0);
							if (ImGui::SubTab(Encrypt("Debugs"), subtab == 16)) subtab = 16;
						}
						ImGui::PopStyleVar();
					}
					if (tab == 7)
					{

						if (ImGui::SubTab(Encrypt("Weather"), subtab == 17)) subtab = 17;
						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("Ignored List"), subtab == 15)) subtab = 15;
						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("Cam Fov"), subtab == 31)) subtab = 31;

						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("Exec"), subtab == 19)) subtab = 19;

						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("Trigger"), subtab == 38)) subtab = 38;
					}
					if (tab == 8)
					{
						if (ImGui::SubTab(Encrypt("Free Cam"), subtab == 30)) subtab = 30;

						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("World Options"), subtab == 37)) subtab = 37;

						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("All Players"), subtab == 14)) subtab = 14;

						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("All Vehicles"), subtab == 13)) subtab = 13;

						ImGui::SameLine(0);
						if (ImGui::SubTab(Encrypt("Saved Locations"), subtab == 20)) subtab = 20;


					}

				}
				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			if (tab == 1)
			{
				//unused

			}
			if (tab == 2)//Visual
			{
				//Visual>player esp
				if (subtab == 1)
				{
					static int slider = 0;
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Enable ESP"), &p_variables->radar3d.p_enable3DRadar);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Box"), &p_variables->radar3d.p_Box, (float*)&p_variables->color.Box);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Corner Box"), &p_variables->radar3d.p_corner_box, (float*)&p_variables->color.Corner);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Enable Player Blips"), &p_variables->radar2d.playerBlips);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("2D radar"), &p_variables->radar2d.p_enable2DRadar);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					/*ImGui::Checkbox_Item_Color(Encrypt("3D Box"), &p_variables->radar3d.p_box3d, (float*)&p_variables->color.Box3d);
					ImGui::Spacing();*/
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Skeleton"), &p_variables->radar3d.p_Skel, (float*)&p_variables->p_color.Skeleton);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Healthbar"), &p_variables->radar3d.p_Health);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Armorbar"), &p_variables->radar3d.p_Armorbar, (float*)&p_variables->color.Armor);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Include Self"), &p_variables->radar3d.include_self);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Detect Admin"), &p_variables->radar3d.detectAdmin);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Warning When Admin Spectate"), &p_variables->radar3d.SpectateWarning);
					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70, 50 });
					ImGui::Checkbox_Item_Color(Encrypt("Name"), &p_variables->radar3d.name, (float*)&p_variables->color.NameTags);
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item_Color(Encrypt("Weapon"), &p_variables->radar3d.p_Weapon, (float*)&p_variables->color.Weapon);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item_Color(Encrypt("Distance"), &p_variables->radar3d.p_Distance, (float*)&p_variables->color.Distance);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item_Color(Encrypt("Id"), &p_variables->radar3d.id, (float*)&p_variables->color.Id);
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item_Color(Encrypt("Visible Check"), &p_variables->radar3d.visiblecheck, (float*)&p_variables->color.InVisible);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					const char* Health[2] = { "Side", "Bottom" };

					ImGui::Combo_Item(Encrypt("Bar Style"), &p_variables->radar3d.p_Health_Bar_Type, Health, 2);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Slider_Item(Encrypt("Render Distance"), &p_variables->radar3d.p_renderDistance, 0, 1000);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					const char* components[10] = { "Face", "Mask", "Hair", "Torso", "Leg","Parachute", "bag","Shoes","Accessory","Undershirt" };
					ImGui::Combo_Item(Encrypt("TeamCheck Component"), &p_variables->radar3d.teamCheckComponent, components, 10);
				}
				//Visual>Ped esp
				if (subtab == 2)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Enable Ped ESP"), &p_variables->ped_esp.enable);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Snapline"), &p_variables->ped_esp.p_Snapline, (float*)&p_variables->color.PedESP);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Enable Ped Blips"), &p_variables->ped_esp.pedBlips);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Only Show car Blips"), &p_variables->ped_esp.OnlyCarBlips);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Best Target Spot"), &p_variables->ped_esp.bestTarget);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Render Distance"), &p_variables->ped_esp.render_distance, 0, 1000);
				}
				//Visual>Vehicle esp
				if (subtab == 3)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Enable Vehicle ESP"), &p_variables->vehicle_esp.enable);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Vehicle Spot"), &p_variables->vehicle_esp.box, (float*)&p_variables->color.Vehicle_Box);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Name Tags"), &p_variables->vehicle_esp.names, (float*)&p_variables->color.Vehicle_Names);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Snapline"), &p_variables->vehicle_esp.snaplines, (float*)&p_variables->color.Vehicle_Snaplines);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Render Distance"), &p_variables->vehicle_esp.render_distance, 0, 1000);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Enable Filter"), &p_variables->vehicle_esp.filter);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Focus Cam On Vehicle"), &p_variables->p_aimbot.focusCamOnVehRot);
				}
				//Visual>Object esp
				if (subtab == 4)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Enable Object ESP"), &p_variables->object_esp.enable);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("All Object Hash"), &p_variables->object_esp.hashes, (float*)&p_variables->color.Vehicle_Box);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Lootbag Circle"), &p_variables->object_esp.lootbag, (float*)&p_variables->color.Vehicle_Names);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item_Color(Encrypt("Snapline"), &p_variables->object_esp.snaplines, (float*)&p_variables->color.Vehicle_Snaplines);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Render Distance"), &p_variables->object_esp.render_distance, 0, 1000);
				}
			}
			//Vehicle
			if (tab == 3)
			{
				//Vehicle>Own Vehicle
				if (subtab == 5)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("GodMode"), &p_variables->p_vehicle.GodMode);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("No Collision"), &p_variables->p_vehicle.p_nocol);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("No Fall OFF"), &p_variables->p_vehicle.NoFallOff);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Repair Vehicle"), ImVec2((650 - 190) / 2, 32))) p_variables->p_vehicle.repair = true;
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Max Performance"), ImVec2((650 - 190) / 2, 32))) p_variables->p_vehicle.maxPerformance = true;
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Max Vehicle Mods"), ImVec2((650 - 190) / 2, 32))) p_variables->p_vehicle.maxVehMods = true;
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Easy Handling"), &p_variables->p_vehicle.easyHandling);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Drift Mode"), &p_variables->p_vehicle.driftMode);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Acceleration Changer"), &p_variables->p_vehicle.p_acc);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("TorqueModOps"), &p_variables->p_vehicle.p_acc_int, 1, 150);

					//ImGui::Spacing();
					//ImGui::SetCursorPosX(170);
					//ImGui::Slider_Item(Encrypt("Gravity Scale"), &p_variables->p_vehicle.p_grav_int, 0, 1000);

					ImGui::SetCursorPos({ (170 * 2) + 70, 50 });
					ImGui::Checkbox_Item(Encrypt("Horn Boost"), &p_variables->p_vehicle.hornBoost);
					HELPMARKER("Press E to increase vehicle speed");
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Rocket Boost"), &p_variables->p_vehicle.p_rocket_boost);
					HELPMARKER("Instant Recharge Vehicle rocket");
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Instant Break"), &p_variables->p_vehicle.instantBreak);
					//ImGui::SetCursorPosX((170 * 2) + 70);
					//ImGui::Checkbox_Item(Encrypt("Vehicle Warp"), &p_variables->p_vehicle.vehiclewarp);
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Force Enter Car"), &p_variables->p_vehicle.warp);
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Car Color"), ImVec2((650 - 190) / 2, 32))) {
						if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false)) {
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Car Color Changed") });
							auto veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
							auto i1 = GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 155);
							auto i2 = GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 155);
							VEHICLE::SET_VEHICLE_COLOURS(veh, i1, i2);
						}
					}
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Active Parachute"), ImVec2((650 - 190) / 2, 32))) {
						if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false)) {
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Added Car Parachute to this Car") });
							auto veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE);
							VEHICLE::_SET_VEHICLE_PARACHUTE_MODEL(veh, 230075693);//ENTITY::GET_ENTITY_MODEL(veh)
							VEHICLE::_SET_VEHICLE_PARACHUTE_ACTIVE(veh, TRUE);
						}

					}

					static char plate_input[8] = "Gucci";
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::InputText_Item(Encrypt("Plate Text"), plate_input, sizeof(plate_input));
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Set Plate"), ImVec2((650 - 190) / 2, 32))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Plate Changed") });
						auto* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
						uintptr_t veh = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(g_pedFactory->m_local_ped()->m_vehicle_manager()));
						if (RequestControlOnce(veh))
						{
							g_pattern->oSet_Plate(veh, plate_input);
						}
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("SkyFall"), ImVec2((650 - 190) / 2, 32))) {
						auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
						ENTITY::SET_ENTITY_COORDS(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0), pos.x, pos.y, pos.z + 500.f, 1, 1, 1, 1);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Sky Falling !!") });
					}
				}
				//Vehicle>SpawnVehicle
				if (subtab == 18)
				{

					static int selected = -1;
					static int vehCatelog = -1;
					const char* types[]{ "SuperSports", "Boats", "Commercial", "Compacts", "Coupes", "Cycles", "Emergency", "Helicopters", "Industrial", "Military", "Motorcycles", "Muscle", "OffRoad", "Planes", "Sedans", "Service", "AfterHours", "dcasino", "arenawar", "Doomsday", "Sports", "SportsClassics", "Super", "SUVs", "Trailer", "Trains", "Utility", "Vans", "IMPORTEXPORT" };

					static char filterVehText[100] = "";

					ImGui::Spacing();
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Text("Filter Vehicle");
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::InputText("##ffff", filterVehText, sizeof(filterVehText));
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);

					if (ImGui::ListBoxHeader(Encrypt("##VehSpawnList"), ImVec2(230, 340)))
					{
						//for (int i = 0; filterVehText[i]; i++)
						//	filterVehText[i] = std::toupper(static_cast<unsigned char>(filterVehText[i]));

						int vehCount = IM_ARRAYSIZE(allVehicleNames);
						for (int i = 0; i < vehCount; i++) {
							bool is_selected = (selected == i);
							//std::string name = weapon_list[i] + std::to_string(i);
							if (strstr(allVehicleNames[i], filterVehText) != nullptr) {
								if (ImGui::Selectable(allVehicleNames[i], is_selected))
								{

									selected = i;

								}
							}
						}
						ImGui::ListBoxFooter();
					}
					//ImGui::Spacing();
					//ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					//ImGui::Combo_Item(Encrypt("Catelog"), &vehCatelog, types, 29);

					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					ImGui::Checkbox_Item(Encrypt("Spawn Inside"), &p_variables->p_vehicle.spawnInVehicle);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Networked"), &p_variables->p_vehicle.spawnNetworked);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Spawn maxed"), &p_variables->p_vehicle.spawn_maxed);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Boost Plane On Spawn"), &p_variables->p_vehicle.BoostPlaneOnSpawn);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("No Collistion"), &p_variables->p_vehicle.nocollision);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Spawn Vehicle"), ImVec2((650 - 190) / 2, 32)))
					{

						p_variables->p_vehicle.spawnvehicule = true;

						auto VehicleName = allVehicleNames[selected];
						p_variables->p_vehicle.VehicleToSpawn = rage::joaat(VehicleName);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Spawned %s", VehicleName });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Spawn Custom Vehicle"), ImVec2((650 - 190) / 2, 32)))
					{
						p_variables->p_vehicle.spawnvehicule = true;

						p_variables->p_vehicle.VehicleToSpawn = rage::joaat(filterVehText);

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Vehicle to.."), ImVec2((650 - 190) / 2, 32)))
					{
						CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
						CPed* LocalPlayer = ped_factory->m_local_ped();
						auto myveh = LocalPlayer->m_vehicle_manager();
						if (myveh)
						{
							auto VehicleName = allVehicleNames[selected];
							myveh->modelInfo()->Hash = rage::joaat(VehicleName);
						}

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("taskVehicleChase"), &p_variables->Ped_Options.Chasing);


				}

			}
			//Weapon
			if (tab == 4)
			{

				//Weapon>Aimbot
				if (subtab == 12)
				{


					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Enable Aimbot"), &p_variables->p_aimbot.enable);
					ImGui::Spacing();


					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Normal Aimbot"), &p_variables->p_aimbot.normal_aim);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Silent aim"), &p_variables->p_aimbot.silent_aim);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Magic Bullet"), &p_variables->p_aimbot.magic_aim);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Prediction"), &p_variables->p_aimbot.prediction);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Random Hit"), &p_variables->p_aimbot.randomSpot);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("FoV Size"), &p_variables->p_aimbot.fov_size, 10, 500);


					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Draw FoV"), &p_variables->p_aimbot.draw_fov);

					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Hit chance"), &p_variables->p_aimbot.hitChance, 1, 100);


					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Smoothness"), &p_variables->p_aimbot.smoothness, 0, 100);

					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Aimbot distance"), &p_variables->p_aimbot.distance, 0, 500);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Silent FOV"), &p_variables->p_aimbot.silent_fov, 20, 500);




					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					const char* Aimbot_Bones[5] = { "Head", "Neck", "Body", "Hands", "Foot" };
					const char* Aimbot_CrossHair[5] = { "Disable", "Small_Dot", "Advanced", "Cross" };
					ImGui::Combo_Item(Encrypt("Aimbot Bones"), &p_variables->p_aimbot.aimbot_bone, Aimbot_Bones, 5);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Find Visible Spot"), &p_variables->p_aimbot.nearest_bone);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Ignore Peds"), &p_variables->p_aimbot.ignore_peds);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Triggerbot"), &p_variables->p_aimbot.triggerbot);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Mouse Event Aim"), &p_variables->p_aimbot.mouseAim);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Combo_Item(Encrypt("Draw CrossHair"), &p_variables->p_aimbot.crosshair, Aimbot_CrossHair, 4);

					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Visible Check"), &p_variables->p_aimbot.visibility_check);
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Hotkey_Item(Encrypt("Aimbot Key"), &p_variables->p_aimbot.Key);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Hotkey_Item(Encrypt("Aimbot Key 2"), &p_variables->p_aimbot.Key2);
					ImGui::SetCursorPosX((170 * 2) + 70);
					const char* Aimbot_Type[2] = { "rrraw", "vectoor" };

				}

				//Weapon>Modify
				if (subtab == 6)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("No Recoil"), &p_variables->p_Weapons.NoRecoil);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("No Spread"), &p_variables->p_Weapons.NoSpread);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Fast Reload"), &p_variables->p_Weapons.NoReload);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Infinite Ammo"), &p_variables->p_Weapons.InfiniteAmmo);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Max Weapon Range"), &p_variables->p_Weapons.range);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Ammo"), ImVec2((650 - 190) / 2, 32))) {
						auto weahash = WEAPON::GET_SELECTED_PED_WEAPON(PLAYER::PLAYER_PED_ID());
						WEAPON::SET_PED_AMMO(PLAYER::PLAYER_PED_ID(), weahash, p_variables->p_Weapons.ammoCount);
					}
					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("Ammo Count"), &p_variables->p_Weapons.ammoCount, 2, 1000);
					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					ImGui::Checkbox_Item(Encrypt("Rapid Fire"), &p_variables->p_Weapons.Rapid_Fire);
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Slider_Item(Encrypt("Fire Rate"), &p_variables->p_Weapons.fireRate, 1, 1000);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Explosive Ammo"), &p_variables->p_Weapons.explo_ammo);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Fire Ammo"), &p_variables->p_Weapons.fire_ammo);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Damage Multiply"), &p_variables->p_Weapons.damageMultiply);
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::SliderFloat(Encrypt("Multiply"), &p_variables->p_Weapons.dmgModifier, 0.1f, 100.0f);
				}
				//Weapon>SpawnWeapons
				if (subtab == 7)
				{
					static int selected = 0;

					ImGui::Spacing();
					ImGui::SetCursorPos({ 170, 50 });
					//ImGui::ListBox(Encrypt("##WeaponList"), &selected, weapon_list, IM_ARRAYSIZE(weapon_list), 10);

					if (ImGui::ListBoxHeader(Encrypt("##WeaponList"), ImVec2(230, 340)))
					{
						//
						int weaponCount = IM_ARRAYSIZE(AllWeaponNames);
						for (int i = 0; i < weaponCount; i++) {
							bool is_selected = (selected == i);
							//std::string name = weapon_list[i] + std::to_string(i);

							if (ImGui::Selectable(AllReadableWeaponNames[i], is_selected))
							{

								selected = i;

							}
						}
						ImGui::ListBoxFooter();
					}
					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });

					if (ImGui::Button(Encrypt("Give Weapon"), ImVec2((650 - 190) / 2, 22)))
					{
						auto WName = AllWeaponNames[selected];
						auto whash = rage::joaat(WName);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Weapon %s Gived"),WName });
						WEAPON::GIVE_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), whash, (int)250, (bool)false, (bool)true);

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);

					if (ImGui::Button(Encrypt("Give All Weapons"), ImVec2((650 - 190) / 2, 22)))
					{
						for (int i = 0; i < std::size(AllWeaponNames) - 1; i++)
						{
							auto wWName = AllWeaponNames[i];
							auto wwhash = rage::joaat(wWName);
							WEAPON::GIVE_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), wwhash, (int)250, (bool)false, (bool)false);
						}
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("All Weapons Spawned") });

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);

					if (ImGui::Button(Encrypt("Drop Current Weapon"), ImVec2((650 - 190) / 2, 22)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Weapon Droped!!") });
						WEAPON::SET_PED_DROPS_INVENTORY_WEAPON(PLAYER::PLAYER_PED_ID(), WEAPON::GET_SELECTED_PED_WEAPON(PLAYER::PLAYER_PED_ID()), 0, 2.0, 0, -1);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);

					if (ImGui::Button(Encrypt("Add Special Finish")))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Weapon Modified") });
						auto pedid = PLAYER::PLAYER_PED_ID();
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x27872C90);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xD7391086);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x9B76C72C);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x487AAE09);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x85A64DF9);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x377CD377);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xD89B9658);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x4EAD7533);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x4032B5E7);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x77B8AB2F);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x7A6A7B7B);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x161E9241);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);

					if (ImGui::Button(Encrypt("Add Magazine")))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Weapon Modified") });
						auto pedid = PLAYER::PLAYER_PED_ID();
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xDE1FA12C);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xE6CFD1AA);

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);

					if (ImGui::Button(Encrypt("Add Suppressor")))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Weapon Modified") });
						auto pedid = PLAYER::PLAYER_PED_ID();
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x65EA7EBB);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x837445AA);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xA73D4664);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xC304849A);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xE608B35E);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);

					if (ImGui::Button(Encrypt("Add Scope")))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Weapon Modified") });
						auto pedid = PLAYER::PLAYER_PED_ID();
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x420FD713);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x420FD71);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x9D2FBF29);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xA0D89C42);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xAA2C45B4);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xD2443DDC);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x3CC6BA57);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0x3C00AFED);
						WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(pedid, WEAPON::GET_SELECTED_PED_WEAPON(pedid), 0xBC54DA77);
					}
				}

			}
			if (tab == 5)//Player
			{
				//Player>Self
				if (subtab == 8)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Godmode"), &p_variables->misc.p_godmode);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Semi Godmode"), &p_variables->misc.p_autoheal);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Regen HP When Hurt"), &p_variables->misc.regen);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Tiny Ped"), &p_variables->misc.tityPed);
					ImGui::Spacing();

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("No Rag-Doll"), &p_variables->misc.ragDoll);

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Invisible"), &p_variables->misc.invisible);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Infinite Armor"), &p_variables->misc.infArmor);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Anti HeadShot"), &p_variables->misc.antiHS);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Infinite Stamina"), &p_variables->misc.infStamina);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("No Collision"), &p_variables->misc.p_nocol);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Super Jump"), &p_variables->misc.superjump);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Anti Stun"), &p_variables->misc.AntiStun);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					//ImGui::Checkbox_Item(Encrypt("Enable Clown Blood Vfx"), &p_variables->misc.clownVFX);
					ImGui::Spacing();

					ImGui::SetCursorPosX(170);
					static int fxxxx = 0;
					if (ImGui::DragInt("##3fxxxx", &fxxxx, 1, 1, 924,"%d", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit)) {
						if (fxxxx >= 924)fxxxx = 0;
						p_variables->misc.int1 = fxxxx;
					}

					ImGui::SetCursorPos({ (170 * 2) + 70, 50 });
					ImGui::Checkbox_Item(Encrypt("Fast Run"), &p_variables->misc.p_fastrun);

					ImGui::SetCursorPosX((170 * 2) + 70);
					if (p_variables->misc.p_fastrun)
						ImGui::SliderFloat(Encrypt("Speed"), &p_variables->misc.runspeed, 1.f, 10.f);
					ImGui::Spacing();


					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Explosive Meele"), &p_variables->misc.explo_meele);
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("No Camera Shake"), &p_variables->misc.NoCamShake);
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Infinite OxyGen"), ImVec2((650 - 190) / 2, 32))) {
						PED::SET_PED_DIES_IN_WATER(PLAYER::PLAYER_PED_ID(), FALSE);
					}
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Infinite Combat Rolls"), ImVec2((650 - 190) / 2, 32))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_SHOOTING_ABILITY"), 250, true);
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_FLYING_ABILITY"), 9999, true);
					}
					//ImGui::Checkbox_Item(Encrypt("Infinite Combat Rolls"), &p_variables->misc.infRolls);
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Suicide"), ImVec2((650 - 190) / 2, 32))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("You are Dead !!") });
						ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 0);
					}
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Health"), ImVec2((650 - 190) / 2, 32))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						auto* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
						g_pedFactory->m_local_ped()->m_health(200);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Armor"), ImVec2((650 - 190) / 2, 32))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						auto* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
						g_pedFactory->m_local_ped()->m_armor(100);
					}
					
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Revive"), ImVec2((650 - 190) / 2, 32))) {
						auto coords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), TRUE);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						
						ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 100);
						ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);
						AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID());
						GRAPHICS::_STOP_ALL_SCREEN_EFFECTS();
						NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(coords.x, coords.y, coords.z, 0, false, false);

						TriggerEvent(false, "ems:revive");
						TriggerEvent(false, "esx_ambulancejob:revive");
						TriggerEvent(true, "esx_ambulancejob:setDeathStatus", false);
						TriggerEvent(false, "hospital:client:Revive");
						//TriggerEvent(true, "chames_ambulance_medic:removeMoney", -999999);
						TriggerEvent(false, "paramedic:revive");
						TriggerEvent(false, "playerSpawned");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Restore Hunger/Thirst"), ImVec2((650 - 190) / 2, 32))) {
						TriggerEvent(false, "esx_status:add", "hunger", 1000000);
						TriggerEvent(false, "esx_status:add", "thirst", 1000000);
						TriggerEvent(false, "hud:client:UpdateNeeds", 100, 100);
					
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Un-Cuff"), ImVec2((650 - 190) / 2, 32))) {
						static bool cuffstate = false;
						PED::SET_ENABLE_HANDCUFFS(PLAYER::PLAYER_PED_ID(), cuffstate);
						cuffstate = !cuffstate;
						CONTROLS::ENABLE_ALL_CONTROL_ACTIONS(0);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("SkyFall"), ImVec2((650 - 190) / 2, 32))) {
						auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(),false);
						ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), pos.x, pos.y, pos.z + 500.f, 1, 1, 1, 1);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Sky Falling !!") });
					}
					//ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();



						//ImGui::SetCursorPosX((170 * 2) + 70);
						//if (ImGui::Button(Encrypt("Stop Animation"), ImVec2((650 - 190) / 2, 32))) {

						//}

						//ImGui::Spacing();

				}

				//Player>StatsEditor
				if (subtab == 35)
				{
					static int stamina = 99;
					static int strength = 99;
					static int stealth = 99;
					static int shooting = 99;
					static int flying = 99;
					static int driving = 99;
					static int lung_capacity = 99;

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderInt("Stamina", &stamina, 0, 100);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderInt("Strength", &strength, 0, 100);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderInt("Stealth", &stealth, 0, 100);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderInt("Shooting", &shooting, 0, 100);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderInt("Flying", &flying, 0, 100);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderInt("Driving", &driving, 0, 100);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderInt("Lung Capacity", &lung_capacity, 0, 100);
					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					if (ImGui::Button(Encrypt("Change Stamina"), ImVec2(132, 25)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_STAMINA"), stamina, true);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Strength"), ImVec2(132, 25)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_STRENGTH"), strength, true);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Stealth"), ImVec2(132, 25)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_STEALTH_ABILITY"), stealth, true);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Shooting"), ImVec2(132, 25)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_SHOOTING_ABILITY"), shooting, true);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Flying"), ImVec2(132, 25)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_FLYING_ABILITY"), flying, true);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Driving"), ImVec2(132, 25)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_DRIVING_ABILITY"), driving, true);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change Lung Capacity"), ImVec2(132, 25)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_LUNG_CAPACITY"), lung_capacity, true);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Change All"), ImVec2(150, 50)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed Change Player Stats!!") });
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_STAMINA"), stamina, true);
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_STRENGTH"), strength, true);
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_STEALTH_ABILITY"), stealth, true);
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_SHOOTING_ABILITY"), shooting, true);
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_FLYING_ABILITY"), flying, true);
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_DRIVING_ABILITY"), driving, true);
						STATS::STAT_SET_INT(RAGE_JOAAT("MP0_LUNG_CAPACITY"), lung_capacity, true);

					}
				}
				//Player>OutfitEditor
				if (subtab == 36)
				{
					ImGui::Spacing();
					static int i1 = 0, i2 = 0, i3 = 0;
					static int num1 = 0, num2 = 0, num3 = 0;
					static int selected = -1;
					static int selected2 = -1;
					auto player = PLAYER::PLAYER_PED_ID();

					static int item_current25 = 0;
					const char* items25[] = { "Face", "Mask", "Hair", "Torso", "Leg","Parachute", "bag", "Shoes", "Accessory", "Undershirt", "Kevlar", "AcBadgecessory", "Torso 2" };


					ImGui::SetCursorPos({ 170 , 50 });

					if (ImGui::ListBoxHeader(Encrypt("##OutfitComponents"), ImVec2(120, 260)))
					{
						for (int id = 0; id < IM_ARRAYSIZE(items25); id++)
						{

							bool is_selected = (selected == id);
							if (ImGui::Selectable(items25[id], is_selected))
							{
								selected = id;
								num1 = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(player, selected);
								i1 = PED::GET_PED_DRAWABLE_VARIATION(player, selected);
								i2 = 0; i3 = 0;
							}
						}
						ImGui::ListBoxFooter();
					}
					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) ,50 });
					static bool cosmicV = false;

					if (cosmicV) selected = -1;
					ImGui::Spacing();
					if (selected != -1)
					{

						auto text1 = "ComponentId: " + std::to_string(i1);
						auto text2 = "DrawableId: " + std::to_string(i2);
						auto text3 = "TextureId: " + std::to_string(i3);

						ImGui::SetCursorPosX(170 * 2);

						ImGui::Text(text1.c_str());
						ImGui::SetCursorPosX(170 * 2);
						ImGui::Text(text2.c_str());
						ImGui::SetCursorPosX(170 * 2);
						ImGui::Text(text3.c_str());
						ImGui::PushItemWidth(200);

						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::InputInt("##1", &i1, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CharsNoBlank)) {
							if (i1 < 0)i1 = num1;
							if (i1 > num1) i1 = 0;
							i2 = 0; i3 = 0;
							num2 = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(player, i1);

							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), selected, i1, i2, i3);
						}
						//ImGui::InputInt("##1", &i1);
						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::InputInt("##2", &i2, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit)) {
							if (i2 < 0)i2 = num2;
							if (i2 > num2) i2 = 0;
							num3 = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(player, i1, i2);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), selected, i1, i2, i3);
						}
						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::InputInt("##3", &i3, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit)) {
							if (i3 < 0)i3 = num3;
							if (i3 > num3) i3 = 0;
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), selected, i1, i2, i3);
						}

						ImGui::PopItemWidth();

					}
					ImGui::SetCursorPosX(170 * 2);
					ImGui::Checkbox_Item(Encrypt("CosmicV Server"), &cosmicV);
					if (!cosmicV)
					{

						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::Button(Encrypt("Random Outfit"), ImVec2((650 - 190) / 2, 32)))
						{
							//ResetAppearance();
							PED::SET_PED_RANDOM_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 0);
						}
						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::Button(Encrypt("Santa Claus"), ImVec2((650 - 190) / 2, 32)))
						{
							//ResetAppearance();
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, 8, 0, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 3, 12, 0, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 4, 19, 0, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 6, 4, 4, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 7, 10, 0, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 8, 21, 2, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 11, 19, 0, 0);
						}

						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::Button(Encrypt("Green Alien"), ImVec2((650 - 190) / 2, 32)))
						{
							//ResetAppearance();

							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, 134, 8, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 3, 3, 8, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 4, 106, 8, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 6, 6, 1, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 11, 274, 8, 0);
						}

						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::Button(Encrypt("Purple Alien"), ImVec2((650 - 190) / 2, 32)))
						{
							//ResetAppearance();

							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, 134, 9, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 3, 3, 9, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 4, 106, 9, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 6, 6, 1, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 11, 274, 9, 0);
						}
						ImGui::SetCursorPosX(170 * 2);
						if (ImGui::Button(Encrypt("Elf"), ImVec2((650 - 190) / 2, 32)))
						{
							//ResetAppearance();

							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, 34, 0, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 3, 4, 0, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 4, 19, 1, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 6, 22, 1, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 7, 18, 0, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 8, 28, 8, 0);
							PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 11, 19, 1, 0);
						}
					}

					if (cosmicV)
					{
						ImGui::SetCursorPosX(170 * 2);

						static const char* items26[] = { "Freddy", "Paul", "Jack", "Neco", "Avo","Bushen", "Jason", "Uglycus", "Tim", "Kenzo", "Franz", "Buu", "Avirsa", "BMO", "Fuzza" };
						static vector<int> compvar = { 348,216,349,237,335,236,4,287,212,318,282,369,241,337,290 };
						static vector<int> textr = { 0,1,0,1,0,0,0,2,1,9,1,0,20,0,0 };
						if (ImGui::ListBoxHeader(Encrypt("##CosmicV"), ImVec2(120, 260)))
						{
							for (int id = 0; id < IM_ARRAYSIZE(items26); id++)
							{

								bool is_selected = (selected2 == id);
								if (ImGui::Selectable(items26[id], is_selected))
								{
									selected2 = id;
									PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, compvar[id], textr[id], 0);
								}
							}
							ImGui::ListBoxFooter();
						}
					}
				}
				//Player>ModelSpawner
				if (subtab == 11)
				{

					static int selected = -1;

					static char filterText[100] = "";

					//ImGui::Spacing();
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::SetNextItemWidth(200);
					ImGui::Text("Filter :");
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::InputText("##ffff", filterText, sizeof(filterText));
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::ListBoxHeader(Encrypt("##ModelSpawnList"), ImVec2(230, 340)))
					{
						//for (int i = 0; filterText[i]; i++) 
						//	filterText[i] = std::toupper(static_cast<unsigned char>(filterText[i]));

						int vehCount = IM_ARRAYSIZE(player_models);
						for (int i = 0; i < vehCount; i++) {
							bool is_selected = (selected == i);
							//std::string name = weapon_list[i] + std::to_string(i);
							if (strstr(player_models[i], filterText) != nullptr) {
								if (ImGui::Selectable(player_models[i], is_selected))
								{

									selected = i;

									//auto ModelName = player_models[selected];
									//p_variables->misc.ModelToChange = rage::joaat(ModelName);
									//p_variables->misc.ModelChanger = true;
								}
							}
						}
						ImGui::ListBoxFooter();
					}

					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					if (ImGui::Button(Encrypt("Change Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = player_models[selected];
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Default Fivem Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "mp_m_freemode_01";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Jesus Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "u_m_m_jesus_01";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Clown Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "s_m_y_clown_01";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Cop Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "s_m_y_cop_01";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Alien Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "s_m_m_movalien_01";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Niko Belic Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "mp_m_niko_01";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Chimp Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "a_c_chimp";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Husky Model"), ImVec2((650 - 190) / 2, 32)))
					{
						auto ModelName = "a_c_husky";
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Successed Changed to %s", ModelName });

						p_variables->misc.ModelToChange = rage::joaat(ModelName);
						p_variables->misc.ModelChanger = true;

					}
				}

			}
			//Misc
			if (tab == 7)
			{
				//Misc>Resources
				if (subtab == 19)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Enable"), &p_variables->p_resource.enable);
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Cache"), &p_variables->p_resource.cache);
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Inject"), &p_variables->p_resource.excution);
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Replace"), &p_variables->p_resource.replace);
					ImGui::SetCursorPosX(170);
					ImGui::PushItemWidth(150);
					ImGui::InputInt(Encrypt("Target Index"), &p_variables->p_resource.targetIndex);
					static char buf[100] = "hardcap";
					static char buf2[100] = "temp";
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Target Name"), buf, ImGuiInputTextFlags_AutoSelectAll);
					p_variables->p_resource.targetName = buf;
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Folder Name"), buf2, ImGuiInputTextFlags_AutoSelectAll);
					p_variables->p_resource.svrName = buf2;
					ImGui::PopItemWidth();

					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Dump All Resource"), ImVec2((650 - 190) / 2, 32))) {
						CacheAllResource2();
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					static std::string resname;
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Dump Selected Resource"), ImVec2((650 - 190) / 2, 32))) {
						CacheResourceData(resname);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					if (ImGui::ListBoxHeader(Encrypt("##ResourceList"), ImVec2(230, 450)))
					{
						std::vector<fx::ResourceImpl*>* g_allResources = (std::vector<fx::ResourceImpl*>*)g_pattern->g_allResources;
						int i = 0; static int selected = -1;
						for (fx::ResourceImpl* resource : *g_allResources)
						{
							bool is_selected = (selected == i);
							if (ImGui::Selectable(resource->m_name.c_str(), is_selected))
							{

								selected = i;
								resname = resource->m_name;
								strcpy_s(buf, resource->m_name.c_str());
							}
							i++;
						}
						ImGui::ListBoxFooter();
					}
				}
				//Misc>Trigger
				if (subtab == 38)
				{
					static char inputBuffer[256]; // Buffer to store user input
					static char inputBuffer1[256];
					static char inputBuffer2[256];
					static char inputBuffer3[256];
					static char inputBuffer4[256];
					static char inputBuffer5[256];
					static char inputBuffer6[256];
					static bool triggerServer = false;

					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("TriggerServerEvent"), &triggerServer);
					ImGui::SetCursorPosX(170);
					ImGui::InputText(Encrypt("EventName"), inputBuffer, sizeof(inputBuffer));
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Arg1"), inputBuffer1, sizeof(inputBuffer));
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Arg2"), inputBuffer2, sizeof(inputBuffer));
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Arg3"), inputBuffer3, sizeof(inputBuffer));
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Arg4"), inputBuffer4, sizeof(inputBuffer));
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Arg5"), inputBuffer5, sizeof(inputBuffer));
					ImGui::SetCursorPosX(170);
					ImGui::InputText_Item(Encrypt("Arg6"), inputBuffer6, sizeof(inputBuffer));
					ImGui::SetCursorPosX(170);
					if (ImGui::Button("Submit", ImVec2(200, 32))) {
						
						if (triggerServer)
							TriggerEvent(true, inputBuffer, inputBuffer1, inputBuffer2, inputBuffer3, inputBuffer4, inputBuffer5, inputBuffer6);
						else
							TriggerEvent(false, inputBuffer, inputBuffer1, inputBuffer2, inputBuffer3, inputBuffer4, inputBuffer5, inputBuffer6);

						//memset(inputBuffer, 0, sizeof(inputBuffer));

					}
				}
				//Misc>Ignorelist
				if (subtab == 15)
				{
					CPed* cPedSelected = 0;
					static int selected = 0;
					ImGui::SetCursorPos({ 170 , 50 });
					if (ImGui::ListBoxHeader(Encrypt("##Ignorelist"), ImVec2(230, 340)))
					{
						for (int id : p_variables->p_friend.Friend)
						{

							bool is_selected = (selected == id);
							if (ImGui::Selectable(std::to_string(id).c_str(), is_selected))
							{
								selected = id;

							}
						}
						ImGui::ListBoxFooter();
					}

					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					ImGui::Checkbox_Item(Encrypt("Auto Add To List"), &p_variables->misc.autoFriendList);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Remove Ignore List"), ImVec2((650 - 190) / 2, 22))) {
						auto it = std::find(p_variables->p_friend.Friend.begin(), p_variables->p_friend.Friend.end(), (DWORD64)selected);
						if (it != p_variables->p_friend.Friend.end())
							p_variables->p_friend.Friend.erase(it);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Remove All"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->p_friend.Friend.clear();
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Set Base Coord"), ImVec2((650 - 190) / 2, 32))) {
						CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);

						CPed* LocalPlayer = ped_factory->m_local_ped();
						p_variables->p_friend.BasePOs = LocalPlayer->m_visual_pos();
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}

				}
				//Misc>Cam-FOV
				if (subtab == 31)
				{
					static int fov = 0;
					static int savedFov = 0;
					//static int cam = CAM::GET_RENDERING_CAM();
					DWORD64 camParams = 0;
					DWORD64 addr = g_pattern->GetCameraViewAngles();
					if (addr)
						camParams = *(DWORD64*)(addr + 0x10);
					ImGui::Spacing();
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Slider_Item(Encrypt("Camera Fov TPS"), &fov, 5, 250);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Text(std::to_string(fov).c_str());
					if (camParams && fov == 0)
						fov = *(float*)(camParams + 0x30);
					if (camParams && fov != savedFov)
					{
						savedFov = fov;
						CAM::SET_CAM_FOV(4, fov);

						*(float*)(camParams + 0x30) = fov;
					}

					static int zoomPrecise = 90;
					static int savedzoomPrecise = 0;
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Slider_Item(Encrypt("MiniMap Zoom Precise"), &zoomPrecise, 1, 100);
					if (zoomPrecise != savedzoomPrecise)
					{
						savedzoomPrecise = zoomPrecise;
						HUD::SET_RADAR_ZOOM_PRECISE(zoomPrecise);
					}

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::SliderFloat(Encrypt("2DRadar Zoom"), &p_variables->p_settings.p_2DRadarZoom, 1, 20);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::DragFloat(Encrypt("Aim Random X"), &p_variables->misc.float1, 0.01f);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::DragFloat(Encrypt("Aim Random Y"), &p_variables->misc.float2, 0.01f);

				}
				//Misc>Weather
				if (subtab == 17)
				{

					ImGui::SetCursorPos({ 170, 50 });
					if (0)
						if (ImGui::Button(Encrypt("Menu Style"), ImVec2((650 - 190) / 2, 32))) {
							ImGui::ShowStyleEditor();
						}

					ImGui::SetCursorPosX(170);
					ImGui::Hotkey_Item(Encrypt("Open Menu Key"), &p_variables->misc.mOpenKey);
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("FPS BOOSTER"), &p_variables->misc.FPSBOOSTER);
					ImGui::SetCursorPosX(170);
					/*
					if (0)
						if (ImGui::Button(Encrypt("Print all registed natives"), ImVec2((650 - 190) / 2, 32))) {
							for (const auto& element : (*orin_fastPathMap)) {
								const NativeHash& nativeHash = element.first;
								const NativeHandler& nativeHandler = element.second;

								// Print the native hash and handler
								Log::Msg("Native Hash: %llx, Native Handler: %llx", nativeHash, nativeHandler);
							}
						}*/
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Unfreezing"), ImVec2((650 - 190) / 2, 32))) {
						ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);
						ENTITY::FREEZE_ENTITY_POSITION(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), FALSE), FALSE);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Weather Clear"), ImVec2((650 - 190) / 2, 32)))
					{
						char* weather = (char*)"CLEAR";
						GAMEPLAY::SET_WEATHER_TYPE_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW(weather);
						GAMEPLAY::SET_OVERRIDE_WEATHER(weather);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Set Weather Clear !!") });
					}
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Weather EXTRASUNNY"), ImVec2((650 - 190) / 2, 32)))
					{
						char* weather = (char*)"EXTRASUNNY";
						GAMEPLAY::SET_WEATHER_TYPE_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW(weather);
						GAMEPLAY::SET_OVERRIDE_WEATHER(weather);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Set Weather EXTRASUNNY !!") });
					}
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Weather BLIZZARD"), ImVec2((650 - 190) / 2, 32)))
					{
						char* weather = (char*)"BLIZZARD";
						GAMEPLAY::SET_WEATHER_TYPE_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW(weather);
						GAMEPLAY::SET_OVERRIDE_WEATHER(weather);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Set Weather BLIZZARD !!") });
					}
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Weather RAIN"), ImVec2((650 - 190) / 2, 32)))
					{
						char* weather = (char*)"RAIN";
						GAMEPLAY::SET_WEATHER_TYPE_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW(weather);
						GAMEPLAY::SET_OVERRIDE_WEATHER(weather);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Set Weather RAIN !!") });
					}
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Weather XMAS"), ImVec2((650 - 190) / 2, 32)))
					{
						char* weather = (char*)"XMAS";
						GAMEPLAY::SET_WEATHER_TYPE_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(weather);
						GAMEPLAY::SET_WEATHER_TYPE_NOW(weather);
						GAMEPLAY::SET_OVERRIDE_WEATHER(weather);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Set Weather XMAS !!") });
					}
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Time Day"), ImVec2((650 - 190) / 2, 32)))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Set Time Day !!") });
						NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(12, 12, 0);
					}
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Set Time Night"), ImVec2((650 - 190) / 2, 32)))
					{
						NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(23, 50, 0);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Set Time Night !!") });
					}

					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Stop Rain and Winds"), ImVec2((650 - 190) / 2, 32))) {

						GAMEPLAY::_SET_RAIN_FX_INTENSITY(0.001);
						GAMEPLAY::SET_WIND_SPEED(0.001);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Rain and Winds stoped !!") });
					}
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					if (ImGui::Button(Encrypt("Brightness"), ImVec2((650 - 190) / 2, 32))) {
						char modifierName[] = "rply_brightness";
						//GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();

						GRAPHICS::SET_TRANSITION_TIMECYCLE_MODIFIER(modifierName, 5.0f);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Brightness !!") });
					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Brightness 2"), ImVec2((650 - 190) / 2, 32))) {
						char modifierName[] = "int_lesters";
						GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();

						GRAPHICS::SET_TRANSITION_TIMECYCLE_MODIFIER(modifierName, 5.0f);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Brightness !!") });
					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Glow PED White"), ImVec2((650 - 190) / 2, 32))) {
						char modifierName[] = "mp_lad_day";
						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER(modifierName);
						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER_STRENGTH(5.0f);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Glow PED White !!") });
					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Glow PED Red"), ImVec2((650 - 190) / 2, 32))) {
						char modifierName[] = "mp_lad_night";

						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER(modifierName);
						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER_STRENGTH(5.0f);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Glow PED Red !!") });
					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Glow PED Orange"), ImVec2((650 - 190) / 2, 32))) {
						char modifierName[] = "mp_lad_judgment";

						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER(modifierName);
						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER_STRENGTH(5.0f);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Glow PED Orange !!") });
					}
					static char name[30] = "";
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					ImGui::InputText("name", name, sizeof(name));
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Custom"), ImVec2((650 - 190) / 2, 32))) {
						//char modifierName[] = "int_lesters";

						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER(name);
						GRAPHICS::_SET_EXTRA_TIMECYCLE_MODIFIER_STRENGTH(5.0f);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Custom effect !!") });
					}
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Clear Effects"), ImVec2((650 - 190) / 2, 32))) {

						GRAPHICS::CLEAR_TIMECYCLE_MODIFIER();

						GRAPHICS::_CLEAR_EXTRA_TIMECYCLE_MODIFIER();
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Done !!") });
					}
					if (0)
						if (ImGui::Button(Encrypt("Hook Natives"), ImVec2((650 - 190) / 2, 32))) {

							//HookNative();
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
							//auto player = PLAYER::PLAYER_PED_ID();
							//Log::Msg("g_fastPathMap: %llx", g_fastPathMap);
							//Log::Msg("g_mappingTable: %llx", g_mappingTable);
							//Log::Msg("Health Max : %d", PED::GET_PED_MAX_HEALTH(player));
							//Log::Msg("Health Max : %d", PED::GET_PED_MAX_HEALTH(player));
							//Log::Msg("Health Max : %d", PED::GET_PED_MAX_HEALTH(player));
						}
					ImGui::SetCursorPosX(170);
					//ImGui::SliderFloat(Encrypt("Float 1"), &p_variables->misc.float1, 0.5f, 3.0f);
					ImGui::SetCursorPosX(170);
					//ImGui::SliderInt("Int 1", &p_variables->misc.int1, 0, 100);
					/*
					ImGui::SetCursorPos({ 170 , 85 });
					static bool selected = false;
					if (ImGui::ListBoxHeader("##Logwindow", ImVec2(470, 310)))
					{
						for (int i = 0; i < 30; i++)
						{
							bool isSelected = selected == i;
							if (ImGui::Selectable(std::to_string(i).c_str(), isSelected, ImGuiSelectableFlags_SelectOnClick, ImVec2{230,15}))
							{

								selected = i;

							}
						}
						ImGui::ListBoxFooter();
					}
					*/
				}
				
			}
			//Setting
			if (tab == 6)
			{
				//Setting>Themes
				if (subtab == 9)
				{

					ImGui::SetCursorPos({ 170, 50 });
					static const char* InterfaceTheme = "";
					//ImGui::BeginChild("theme", ImVec2((650 - 190) / 2, 32));
					{
						if (ImGui::BeginMenuEx("menu.tools.theme", ICON_FA_PALETTE)) {
							auto themes = ImGui::Themes();
							for (auto& title : themes) {
								auto theme = title;
								if (ImGui::MenuItem(title, nullptr, InterfaceTheme == theme)) {
									InterfaceTheme = theme;

								}
							}
							ImGui::EndMenu();
						}
					}
					//ImGui::EndChild();

					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("dark"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("dark");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("light"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("light");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("classic"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("classic");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("spectrum"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("spectrum");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("dracula"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("dracula");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("deepdark"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("deepdark");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Test1"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("Test1");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Test2"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("Test2");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					if (ImGui::Button(Encrypt("Test3"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("Test3");
					}
					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70, 50 });
					if (ImGui::Button(Encrypt("Round Items"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::SetTheme("Test3");
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("A10 FigterJet Setting"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->p_aimbot.fov_size = 77;
						p_variables->p_aimbot.distance = 230;
						p_variables->p_aimbot.focusCamOnVehRot = true;
						p_variables->ped_esp.OnlyCarBlips = true;
						p_variables->radar2d.playerBlips = true;
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Bypass Weapon Check"), &p_variables->p_settings.bypassWeaponCheck);
					
				}
				//Setting>ConfigFiles
				if (subtab == 10)
				{
					static int selected = 0;
					std::string cfg = _xor_("default.ini");
					static ImGuiTextFilter name;

					ImGui::SetCursorPos({ 170, 50 });
					ImGui::BeginChild(Encrypt("##ConfigList"), ImVec2((650 - 190) / 2, 290));
					{
						for (int i = 0; i < cfgs.size(); i++) {
							ImGui::SetCursorPosX(10);
							if (ImGui::Selectable(cfgs.at(i).c_str(), selected == i, 0, ImVec2((650 - 190) / 2 - 20, 12))) {
								selected = i;
								cfg = cfgs.at(i).c_str();
							}
						}
					}
					ImGui::EndChild();
					ImGui::SetCursorPos({ (170 * 2) + 70, 50 });
					ImGui::InputText_Item(Encrypt("Enter Config Name"), name.InputBuf, sizeof(name.InputBuf));
					ImGui::Spacing();
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Load"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						cfg_manager->Load(cfg);
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Save"), ImVec2((650 - 190) / 2, 22))) {
						std::string cfgdata = _xor_(".ini");
						std::string cfgname = name.InputBuf + cfgdata;
						if (strlen(name.InputBuf) == 0)
							cfgname = _xor_("default.ini");
						cfg_manager->Save(cfgname);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX({ (170 * 2) + 70 });
					if (ImGui::Button(Encrypt("Reload"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						cfgs = cfg_manager->GetConfigs();
					}
				}

				//Setting>Debugs
				if (subtab == 16)
				{
					CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);

					CPed* LocalPlayer = ped_factory->m_local_ped();
					CWeaponManager* weaponManager = 0;
					CVehicleManager* vehManager = 0;
					CWeaponInfo* weaponInfos = 0;
					CPlayerInfo* playerInfos = 0;
					void* addr = 0;
					DWORD64 cam = 0;
					if (LocalPlayer)
					{
						playerInfos = LocalPlayer->m_player_info();
						addr = (void*)(LocalPlayer->m_player_info() + 0x20);
						weaponManager = LocalPlayer->m_weapon_manager();
						vehManager = LocalPlayer->m_vehicle_manager();
						if (weaponManager)
							weaponInfos = weaponManager->m_weapon_info();
						cam = g_pattern->GetCameraViewAngles();

					}


					char Buff[256] = "";
					ImGui::SetCursorPos({ 170 , 50 });
					ImGui::InputScalar(Encrypt("ped_factory"), ImGuiDataType_U64, ped_factory, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					ImGui::SetCursorPosX(170);
					if (LocalPlayer)
						ImGui::InputScalar(Encrypt("LocalPlayer"), ImGuiDataType_U64, LocalPlayer, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					ImGui::SetCursorPosX(170);
					if (LocalPlayer)
						ImGui::InputScalar(Encrypt("PlayerInfo"), ImGuiDataType_U64, playerInfos, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					ImGui::SetCursorPosX(170);
					if (LocalPlayer)
						ImGui::InputScalar(Encrypt("addr"), ImGuiDataType_U64, addr, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					ImGui::SetCursorPosX(170);
					if (weaponManager)
						ImGui::InputScalar(Encrypt("weaponManager"), ImGuiDataType_U64, weaponManager, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					ImGui::SetCursorPosX(170);
					if (vehManager)
						ImGui::InputScalar(Encrypt("m_vehicle_manager"), ImGuiDataType_U64, vehManager, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					ImGui::SetCursorPosX(170);
					if (weaponInfos)
						ImGui::InputScalar(Encrypt("weaponInfos"), ImGuiDataType_U64, weaponInfos, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					ImGui::SetCursorPosX(170);
					if (cam)
						ImGui::InputScalar(Encrypt("Camera"), ImGuiDataType_U64, &cam, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
					if (LocalPlayer)
					{
						std::string strHealth = "Health: " + std::to_string(LocalPlayer->health());
						ImGui::SetCursorPosX(170);
						ImGui::Text(strHealth.c_str());
						strHealth = "HealthMax: " + std::to_string(LocalPlayer->m_healthmax());
						ImGui::SetCursorPosX(170);
						ImGui::Text(strHealth.c_str());
					}

				}
			}
			//World
			if (tab == 8)
			{
				//World>FreeCam
				if (subtab == 30)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Enable"), &p_variables->misc.freecam);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Hotkey_Item(Encrypt("No Clip Key"), &p_variables->misc.p_flyKey);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("No Clip"), &p_variables->misc.noclip);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Drag Vehicle"), &p_variables->world.DragCars);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Physic Gun"), &p_variables->world.PhysicGun);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Remote Control Car"), &p_variables->world.RcCar);
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Burn Player"), &p_variables->world.BurnPlayer);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Drop All Cars On Him"), &p_variables->world.DropCars);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Tazer Player"), &p_variables->world.TazerPlayer);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Give Health and Armor"), &p_variables->world.DropHnArmor);
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Shoot Bullet"), &p_variables->world.ShootBullets);
					static const char* Weapons[8] = { "Ray Pistol", "RPG", "AR", "HEAVY SNIPER", "GRENADE", "FLARE","MOLOTOV", "Stun gun" };
					Hash weaHash[8] = { 0xAF3696A1 ,0xB1CA77B1, 0xAF113F99,0x0C472FE2, 0xA284510B ,0x47757124, 0x24B17070, 0x3656C8C1 };
					static int i = -1;
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Combo_Item(Encrypt("Weapon To Shoot"), &i, Weapons, 7);
					if (i > 0)
						p_variables->world.WeaponHash = weaHash[i];
					

					//vehicles
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					ImGui::Checkbox_Item(Encrypt("Wash Vehicle"), &p_variables->world.washVehicle);
					
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Destroy Car Engine"), &p_variables->world.destroyEngine);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Burst Car Tires"), &p_variables->world.burstTires);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Set Car Alarm"), &p_variables->world.setAlarm);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Smash Car Windows"), &p_variables->world.smashVehicle);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Break Car Doors"), &p_variables->world.breakDoors);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Break Car Wheels"), &p_variables->world.breakWheels);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Explode Car"), &p_variables->world.ExplodeCar);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Send To Sky"), &p_variables->world.ToSky);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Make It Sink"), &p_variables->world.SinkCar);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Delete Car"), &p_variables->world.DeleteCar);
					
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Object Spawner"), &p_variables->world.objectSpawner);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Object Eraser"), &p_variables->world.objectEraser);

				}
				//World>world Options
				if (subtab == 37)
				{
					ImGui::SetCursorPos({ 170, 50 });
					ImGui::Checkbox_Item(Encrypt("Enable"), &p_variables->world.AffectAll);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Include Peds"), &p_variables->world.includePed);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Particle Fx Effect"), &p_variables->world.ParticleFx);
					ImGui::SetCursorPosX(170);
					
					if (p_variables->world.ParticleFx)
						if (ImGui::ListBoxHeader(Encrypt("##ParticleFx"), ImVec2(230, 140)))
						{
							static int selected2 = -1;
							for (int i = 0; i < fxData.size(); i++) {
								auto it = std::next(fxData.begin(), i);
								auto data = it->second;
								bool is_selected = (selected2 == i);
								if (ImGui::Selectable(data.name.c_str(), is_selected))
								{
									p_variables->ped_esp.SelectedFx = it->first.c_str();
									selected2 = i;

								}
							}
							ImGui::ListBoxFooter();
						}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Teleport All To you"), &p_variables->world.TpAllToYou);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Burn Players"), &p_variables->world.BurnPlayer);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Tazer Players"), &p_variables->world.TazerPlayer);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Silent Kill Players"), &p_variables->world.silentKill);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Delete Peds"), &p_variables->world.DeletePed);

					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Shoot Bullet"), &p_variables->world.ShootBullets);
					static const char* Weapons[8] = { "Ray Pistol", "RPG", "AR", "HEAVY SNIPER", "GRENADE", "FLARE","MOLOTOV", "Stun gun" };
					Hash weaHash[8] = { 0xAF3696A1 ,0xB1CA77B1, 0xAF113F99,0x0C472FE2, 0xA284510B ,0x47757124, 0x24B17070, 0x3656C8C1 };
					static int i = -1;
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Combo_Item(Encrypt("Weapon To Shoot"), &i, Weapons, 7);
					if (i > 0)
						p_variables->world.WeaponHash = weaHash[i];
					//vehicles
					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					ImGui::Checkbox_Item(Encrypt("Destroy Car Engine"), &p_variables->world.destroyEngine);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Burst Car Tires"), &p_variables->world.burstTires);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Set Car Alarm"), &p_variables->world.setAlarm);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Smash Car Windows"), &p_variables->world.smashVehicle);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Break Car Doors"), &p_variables->world.breakDoors);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Break Car Wheels"), &p_variables->world.breakWheels);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Explode Car"), &p_variables->world.ExplodeCar);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Send To Sky"), &p_variables->world.ToSky);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Make It Sink"), &p_variables->world.SinkCar);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Delete Car"), &p_variables->world.DeleteCar);
				}
				//World>Playerlist
				if (subtab == 14)
				{
					static bool spec = false;
					static bool includePed = true;
					static bool inSpecting = false;
					static bool ignoreAll = false;
					static int selected = 0;
					CPed* cPedSelected = 0;
					static int selected_veh = 0;
					Vector3 PedPos(0, 0, 0);
					Vector3 veh_manager(0, 0, 0);

					ImGui::SetCursorPos({ 10, 10 });
					CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);

					CPed* LocalPlayer = ped_factory->m_local_ped();


					CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);


					CPedInterface* pedinterface = ReplayInterface->m_ped_interface();


					ImGui::SetCursorPos({ 170 , 50 });
					if (ImGui::ListBoxHeader(Encrypt("##Playerlist"), ImVec2(230, 330)))
					{
						//static auto pedCount = pedinterface->m_pedcount();
						//if (pedCount != pedinterface->m_pedcount()) {

							//pedCount = pedinterface->m_pedcount();
						for (int i = 0; i < pedinterface->m_pedmax(); i++)
						{
							CPed* cPed = pedinterface->m_pedlist()->m_ped(i);
							if (!cPed) continue;

							if (cPed->IsPedOrFalse() && !includePed) continue; //is ped -> skip
							if (cPed == LocalPlayer) continue;

							bool isFriend = cPed->IsFriend();


							const char* name = cPed->GetName();

							std::string strName(name);
							if (strName == "NPC")
							{
								strName = "NPC #" + std::to_string(i);
								//name = strName.c_str();
							}

							if (isFriend)
								ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
							bool is_selected = (selected == i);
							if (ImGui::Selectable(strName.c_str(), is_selected))
							{
								PedPos = cPed->m_visual_pos();
								selected = i;

							}
							if (isFriend)
								ImGui::PopStyleColor();
							if (ignoreAll)
							{
								//std::to_string(cPed->m_player_info()->m_player_id());
								p_variables->p_friend.Friend.push_back(cPed->m_player_info()->m_player_id());
							}

						}
						if (ignoreAll) ignoreAll = false;
						ImGui::ListBoxFooter();
						//}
					}
					static int selected2 = -1;
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox(Encrypt("Particle Fx :"), &p_variables->Ped_Options.ParticleFx);
					ImGui::SetCursorPosX(170);
					if(p_variables->Ped_Options.ParticleFx)
					if (ImGui::ListBoxHeader(Encrypt("##ParticleFx"), ImVec2(230, 140)))
					{
						for (int i = 0; i < fxData.size(); i++) {
							auto it = std::next(fxData.begin(), i);
							auto data = it->second;
							bool is_selected = (selected2 == i);
							if (ImGui::Selectable(data.name.c_str(), is_selected))
							{
								p_variables->ped_esp.SelectedFx = it->first.c_str();
								selected2 = i;

							}
						}
						ImGui::ListBoxFooter();
					}
					if (selected)
					{
						cPedSelected = pedinterface->m_pedlist()->m_ped(selected);
						p_variables->ped_esp.SelectedPedID = selected;
					}
					if (cPedSelected)
					{

						const char* name = cPedSelected->GetName();
						Vector3 ploc = cPedSelected->m_visual_pos();
						ImVec2 w2s_pos = g_Math->WorldToScreen(ploc);
						g_Renderer->FillRect(w2s_pos.x, w2s_pos.y, 10, 10, ImColor(255, 255, 0), 20);

						std::string strName(name);
						if (strName == "NPC")
							strName = "NPC_#" + std::to_string(selected);
						//ImGui::Spacing();
						ImGui::SetCursorPos({ (170 * 2) + 70, 50 });
						//std::string strIndex = "Index: %d" + std::to_string(selected);
						//ImGui::Text(strIndex.c_str());
						//ImGui::SetCursorPosX((170 * 2) + 70);

						strName = "Name : " + strName;
						ImGui::Text(strName.c_str());
						ImGui::SetCursorPosX((170 * 2) + 70);
						std::string strLocation = "Location: X: " + std::to_string((int)ploc.x) + " Y: " + std::to_string((int)ploc.y) + " Z: " + std::to_string((int)ploc.z);
						ImGui::Text(strLocation.c_str());
						ImGui::SetCursorPosX((170 * 2) + 70);
						std::string strHealth = "Health: " + std::to_string(*(float*)(cPedSelected + 0x280));
						ImGui::Text(strHealth.c_str());
						//g_Math->set_aim_angels(ploc, 1, 40);

					}
					ImGui::SetCursorPos({ (170 * 2) + 70, 120 });
					ImGui::Checkbox(Encrypt("Include Ped"), &includePed);
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox(Encrypt("Spectate"), &spec);
					ImGui::Spacing();

					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Copy Player Outfit"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->Ped_Options.copyOutFit = true;
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Open Inventory"), ImVec2((650 - 190) / 2, 22))) {
						auto pid = cPedSelected->m_player_info()->m_player_id();//CFX::GetPlayerServerId(cPedSelected->m_player_info()->m_player_id());
						TriggerEvent(true, "esx_inventoryhud:openPlayerInventory", pid, cPedSelected->GetName());
						//TriggerEvent(true, "ox_inventory:forceOpenInventory", pid, "stash",1);
						TriggerEvent(true, "inventory:server:OpenInventory", "otherplayer", pid);
					}
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Create Ped"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->Ped_Options.spawnPed = true;
						
					}
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Clone Ped"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->Ped_Options.clonePed = true;
						
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Give Health"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						p_variables->Ped_Options.giveHealth = true;
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Give Armor"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						p_variables->Ped_Options.giveArmor = true;
					}

					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Shoot Bullet"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->Ped_Options.shootBullet = true;
						selected++;
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox(Encrypt("Spam 1M Vehicles on him"), &p_variables->Ped_Options.spam1MVehicle);
					
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("TP To Him"), ImVec2((650 - 190) / 2, 22))) {
						if (cPedSelected)
						{
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
							uintptr_t pedHandle = g_pattern->pointer_to_handle(cPedSelected);
							ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), TRUE);
							LocalPlayer->m_set_visual_pos(cPedSelected->m_visual_pos());
							LocalPlayer->m_navigation_manager()->m_set_visual_pos(cPedSelected->m_visual_pos());
							ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);
						}

					}
					//selected += ImGui::GetIO().MouseWheel;
					//if (selected < 0)selected == 0;



					if (spec && cPedSelected)
					{
						inSpecting = true;
						uintptr_t pedHandle = g_pattern->pointer_to_handle(cPedSelected);
						//NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(1, pedHandle);
						g_pattern->network_set_in_spectator_mode(1, pedHandle);
						ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), TRUE);
					}
					else
					{
						if (inSpecting)
						{
							inSpecting = false;
							uintptr_t pedHandle = g_pattern->pointer_to_handle(cPedSelected);
							//NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(0, pedHandle);
							g_pattern->network_set_in_spectator_mode(0, pedHandle);
							ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);
						}

					}
					if (cPedSelected && cPedSelected->m_vehicle_manager())
					{
						ImGui::Spacing();
						ImGui::SetCursorPosX((170 * 2) + 70);
						if (ImGui::Button(Encrypt("TP to vehicle"), ImVec2((650 - 190) / 2, 22)))
						{
							auto player = PLAYER::PLAYER_PED_ID();
							uintptr_t pedHandle = g_pattern->pointer_to_handle(cPedSelected);
							auto veh = PED::GET_VEHICLE_PED_IS_IN(pedHandle, 0);

							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
							PED::SET_PED_INTO_VEHICLE(player, veh, 0);
							PED::SET_PED_INTO_VEHICLE(player, veh, 1);
						}
						ImGui::Spacing();
						ImGui::SetCursorPosX((170 * 2) + 70);
						if (ImGui::Button(Encrypt("Lock Vehicle"), ImVec2((650 - 190) / 2, 22)))
						{
							uintptr_t pedHandle = g_pattern->pointer_to_handle(cPedSelected);
							auto veh = PED::GET_VEHICLE_PED_IS_IN(pedHandle, 0);
							VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh, 2);
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						}
						ImGui::Spacing();
						ImGui::SetCursorPosX((170 * 2) + 70);
						if (ImGui::Button(Encrypt("Destroy car"), ImVec2((650 - 190) / 2, 22)))
						{
							p_variables->Ped_Options.destroyCar = true;
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						}
					}

					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Add Ignore List"), ImVec2((650 - 190) / 2, 22))) {
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						p_variables->p_friend.Friend.push_back(cPedSelected->m_player_info()->m_player_id());
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Ignore All"), ImVec2((650 - 190) / 2, 22))) {
						ignoreAll = true;
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Remote Control NPC"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->Ped_Options.remoteNPC = true;
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						//uintptr_t pedHandle = g_pattern->pointer_to_handle(cPedSelected);

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("All NPC Attack Him With RPG"), ImVec2((650 - 190) / 2, 22))) {

						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						p_variables->Ped_Options.rpgAndAttack = true;

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox(Encrypt("Chasing him"), &p_variables->Ped_Options.Chasing);

				}
				//World>VehicleList
				if (subtab == 13)
				{
					static int count = 0;
					static int selected = false;
					static bool onlyBroken = false;
					static CVehicleManager* cVehicle = nullptr;
					static Vector3 Vehicle_Pos = { 0,0,0 };
					CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);

					CPed* LocalPlayer = ped_factory->m_local_ped();

					CReplayInterface* ReplayInterface = (CReplayInterface*)*(uint64_t*)(g_pattern->g_replayInterface);

					CVehicleInterface* vehicle_interface = ReplayInterface->m_vehicleinterface();

					ImGui::SetCursorPos({ 170 , 50 });

					if (ImGui::ListBoxHeader("##Vehiclelist", ImVec2(230, 340)))
					{
						//if(vehicle_interface->m_vehiclecount()>count)
						for (int i = 0; i < vehicle_interface->m_vehiclemax(); i++)
						{
							CVehicleManager* cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(i);
							if (!cVehicle) continue;
							if (onlyBroken)
								if (cVehicle->engine_health() > 0)continue;
							auto base_info = *reinterpret_cast<CModelInfo**>((char*)cVehicle + 0x20);
							if (!base_info) continue;
							std::string strVehName(base_info->Name);
							strVehName += "_#" + std::to_string(i);
							
							if (ImGui::Selectable(strVehName.c_str(), selected == i, ImGuiSelectableFlags_SelectOnClick, ImVec2{ 230,15 }))
							{
								selected = i;
								p_variables->p_vehicle.SelectedVehicle = i;
							}
						}
						//count = vehicle_interface->m_vehiclecount();
						ImGui::ListBoxFooter();
					}
					//selected += ImGui::GetIO().MouseWheel;
					//if (selected < 0)selected == 0;

					cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(selected);

					if (cVehicle)
					{
						//cVehicle = vehicle_interface->m_vehiclelist()->m_getvehicle(selected);
						auto base_info = *reinterpret_cast<CModelInfo**>((char*)cVehicle + 0x20);
						Vehicle_Pos = cVehicle->m_visual_pos();
						ImVec2 Vehicle_w2s_pos = g_Math->WorldToScreen(Vehicle_Pos);
						g_Renderer->FillRect(Vehicle_w2s_pos.x, Vehicle_w2s_pos.y, 10, 10, ImColor(255, 255, 0), 20);


						ImGui::Spacing();
						ImGui::SetCursorPos({ (170 * 2) + 70, 50 });
						//std::string strIndex = "Veh Index: %d" + std::to_string(selected);
						//ImGui::Text(strIndex.c_str());
						//ImGui::SetCursorPosX((170 * 2) + 70);
						std::string strObj(base_info->Name);
						std::string strName = "Name : " + strObj;
						ImGui::Text(strName.c_str());
						ImGui::SetCursorPosX((170 * 2) + 70);
						std::string strLocation = "Location: X: " + std::to_string((int)Vehicle_Pos.x) + " Y: " + std::to_string((int)Vehicle_Pos.y) + " Z: " + std::to_string((int)Vehicle_Pos.z);
						ImGui::Text(strLocation.c_str());
						ImGui::SetCursorPosX((170 * 2) + 70);
						UINT32 lockstate = cVehicle->door_lock_status();
						std::string strLock;
						if (lockstate == 1) strLock = "Unlocked";
						if (lockstate == 2) strLock = "Locked";
						if (lockstate == 3) strLock = "LockedForPlayer";
						if (lockstate == 4) strLock = "StickPlayerInside";
						if (lockstate == 7) strLock = "CanBeBrokenInto";
						if (lockstate == 8) strLock = "CanBeBrokenIntoPersist";
						if (lockstate == 10) strLock = "CannotBeTriedToEnter";
						strLock = "Lock State: " + strLock;
						ImGui::Text(strLock.c_str());
						ImGui::SetCursorPosX((170 * 2) + 70);
						std::string strHealth = "Health: " + std::to_string((int)cVehicle->body_health());
						ImGui::Text(strHealth.c_str());
						//g_Math->set_aim_angels(Vehicle_Pos, 1, 40);
					}
					else selected = false;
					ImGui::Spacing();
					ImGui::SetCursorPos({ (170 * 2) + 70, 140 });
					ImGui::Checkbox_Item(Encrypt("Only Broken"), &onlyBroken);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Check If Can Take Control"), ImVec2((650 - 190) / 2, 22))) {
						if (cVehicle)
						{
							uintptr_t veh = g_pattern->pointer_to_handle((CPed*)(cVehicle));
							if (RequestControlOnce(veh))
								ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successful gained control!") });
						}

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Do Chosen Options On FreeCam"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->p_vehicle.FreeCamOpts = true;

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Repair Car"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->p_vehicle.repairVehicle = true;

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Lock Door"), ImVec2((650 - 190) / 2, 22))) {
						if (cVehicle)
						{
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
							auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(cVehicle));
							if (RequestControlOnce(cVehicleHandle))
								VEHICLE::SET_VEHICLE_DOORS_LOCKED((uintptr_t)cVehicleHandle, 2);
							//VEHICLE::SET_VEHICLE_FORWARD_SPEED(cVehicleHandle, 1200.0);
						}
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Lock Player Inside"), ImVec2((650 - 190) / 2, 22))) {
						if (cVehicle)
						{
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
							auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(cVehicle));
							if (RequestControlOnce(cVehicleHandle))
								VEHICLE::SET_VEHICLE_DOORS_LOCKED((uintptr_t)cVehicleHandle, 4);
						}
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Door Jammed"), ImVec2((650 - 190) / 2, 22))) {
						if (cVehicle)
						{
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
							auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(cVehicle));
							if (RequestControlOnce(cVehicleHandle))
								VEHICLE::SET_VEHICLE_DOORS_LOCKED((uintptr_t)cVehicleHandle, 10);
						}
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("UnLock Vehicle"), ImVec2((650 - 190) / 2, 22))) {
						if (cVehicle)
						{
							auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(cVehicle));
							if (RequestControlOnce(cVehicleHandle))
								VEHICLE::SET_VEHICLE_DOORS_LOCKED((uintptr_t)cVehicleHandle, 1);
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						}

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Bring And Steal"), ImVec2((650 - 190) / 2, 22))) {
						//not yet have the offset
						p_variables->p_vehicle.bringnSteal = true;
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("TP to seat"), ImVec2((650 - 190) / 2, 22))) {
						//not yet have the offset
						if (cVehicle)
						{
							auto player = PLAYER::PLAYER_PED_ID();
							uintptr_t pedHandle = g_pattern->pointer_to_handle((CPed*)cVehicle);
							auto asdas = (Entity)pedHandle;
							//auto cVehicleHandle = g_pattern->pointer_to_handle(reinterpret_cast<CPed*>(cVehicle));
							PED::SET_PED_INTO_VEHICLE(player, (uintptr_t)pedHandle, 0);
							PED::SET_PED_INTO_VEHICLE(player, (uintptr_t)pedHandle, -1);
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
							//LocalPlayer->m_set_visual_pos(Vehicle_Pos);
							//LocalPlayer->m_navigation_manager()->m_set_visual_pos(Vehicle_Pos);
						}
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Delete Vehicle"), ImVec2((650 - 190) / 2, 22))) {
						p_variables->p_vehicle.deleteVehicle = true;
						selected++;
					}
				}
				//World>SavedLocations
				if (subtab == 20)
				{
					static int selected = 0;
					static float x, y, z;
					static ImGuiTextFilter lname = "Player Pos#";
					ImGui::SetCursorPos({ 170 , 50 });
					if (ImGui::ListBoxHeader(Encrypt("##SavedLocations"), ImVec2(230, 440)))
					{
						for (int i = 0; i < locationManager->locations.size(); ++i) {
							const Location& loc = locationManager->locations[i];
							bool is_selected = (selected == i);
							if (ImGui::Selectable(loc.name.c_str(), is_selected))
							{
								selected = i;
								locationManager->selected = loc.coord;
								x = loc.coord.x; y = loc.coord.y, z = loc.coord.z;
							}
						}

						ImGui::ListBoxFooter();
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX(170);
					ImGui::Checkbox_Item(Encrypt("Draw Locations ESP"), &p_variables->radar3d.saveLocation);

					ImGui::SetCursorPos({ (170 * 2) + 70,50 });
					ImGui::InputText_Item(Encrypt("Enter Location Name"), lname.InputBuf, sizeof(lname.InputBuf));
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::PushItemWidth(50);
					ImGui::InputFloat("X", &x,0.f,0.f, "%.1f"); ImGui::SameLine(); ImGui::InputFloat("Y", &y, 0.f, 0.f, "%.1f"); ImGui::SameLine(); ImGui::InputFloat("Z", &z, 0.f, 0.f, "%.1f");
					ImGui::PopItemWidth();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Add Location"), ImVec2((650 - 190) / 2, 22))) {
						CPedFactory* ped_factory = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
						CPed* LocalPlayer = ped_factory->m_local_ped();
						
						locationManager->addLocation(lname.InputBuf, LocalPlayer->m_visual_pos());
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Find All Blip Locations"), ImVec2((650 - 190) / 2, 22))) {
						CBlipList* m_blipList = (CBlipList*)g_pattern->BlipLists;
						if (m_blipList)
						{
							for (int i = 0; i < 1500; i++)
							{
								auto item = m_blipList->m_Blips[i].m_pBlip;
								if (item != nullptr)
								{
									//Log::Msg("blip ID: %d | Message: %s | Cat: %d | Display ID: %d | Entity ID: %d", item->m_id, item->m_message, item->m_category, item->m_display_id, item->m_entity_id);
									//if (item->m_icon == 496 || item->m_icon == 378 || item->m_alpha < 254)
									locationManager->addLocation(std::to_string(item->m_id), { item->m_x,item->m_y,item->m_z });
								}
							}
						}
						else
						{
							ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("blip list empty!!!") });
						}
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Remove Selected"), ImVec2((650 - 190) / 2, 22))) {
						locationManager->removeLocation(selected);
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Remove All"), ImVec2((650 - 190) / 2, 22))) {
						locationManager->locations.clear();
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Set WayPoint"), ImVec2((650 - 190) / 2, 22))) {
						if (!locationManager->locations.empty())
						{
							auto loc = locationManager->locations[selected].coord;
							int blip = HUD::GET_FIRST_BLIP_INFO_ID(8);
							if (HUD::DOES_BLIP_EXIST(blip)) {
								HUD::SET_BLIP_COORDS(blip, loc.x, loc.y, loc.z);
							}
							else {
								HUD::SET_NEW_WAYPOINT(loc.x, loc.y);
							}
							
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Successed !!") });
						}

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Teleport to Location"), ImVec2((650 - 190) / 2, 32))) {
						if (!locationManager->locations.empty())
						{
							Vector3 loc = { x,y,z };
							//if (selected)loc = locationManager->locations[selected].coord;
							auto* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);

							if (g_pedFactory->m_local_ped())
							{
								if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 1))
								{
									auto veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0);
									ENTITY::SET_ENTITY_COORDS_NO_OFFSET(veh, loc.x, loc.y, -200.5f, 1, 1, 1);
								}
								else
								{
									ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), TRUE);
									g_pedFactory->m_local_ped()->m_set_visual_pos(Vector3(loc.x, loc.y, 200.5f));
									g_pedFactory->m_local_ped()->m_navigation_manager()->m_set_visual_pos(Vector3(loc.x, loc.y, -200.5f));
									ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);
								}
								ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Teleported !!") });
							}
						}

					}
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					if (ImGui::Button(Encrypt("Teleport to Waypoint"), ImVec2((650 - 190) / 2, 32))) {
						auto* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
						g_pattern->WayPointPos = *reinterpret_cast<Vector2*>(g_pattern->WayPointRead);
						if (g_pedFactory->m_local_ped())
						{
							if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 1))
							{
								auto veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0);
								ENTITY::SET_ENTITY_COORDS_NO_OFFSET(veh, g_pattern->WayPointPos.x, g_pattern->WayPointPos.y, -200.5f, 1, 1, 1);
							}
							else
							{
								ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), TRUE);
								g_pedFactory->m_local_ped()->m_set_visual_pos(Vector3(g_pattern->WayPointPos.x, g_pattern->WayPointPos.y, -200.5f));
								g_pedFactory->m_local_ped()->m_navigation_manager()->m_set_visual_pos(Vector3(g_pattern->WayPointPos.x, g_pattern->WayPointPos.y, -200.5f));
								ENTITY::FREEZE_ENTITY_POSITION(PLAYER::PLAYER_PED_ID(), FALSE);
							}
							ImGui::InsertNotification({ ImGuiToastType_Success, 3000, Encrypt("Teleported !!") });
						}
					}

					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Drive To WayPoint"), &p_variables->p_vehicle.driveWaypoint);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Drive Around"), &p_variables->p_vehicle.driveWander);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Drive To Location"), &p_variables->p_vehicle.driveCoord);
					ImGui::Spacing();
					ImGui::SetCursorPosX((170 * 2) + 70);
					ImGui::Checkbox_Item(Encrypt("Stop Driving"), &p_variables->p_vehicle.stopDrivingTask);
				}

			}
		}

	}
	
}

inline std::string tm_to_readable_time(tm ctx)
{
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) 
{
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) 
{
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}