#include <GameMath.h>
float Game_Math::degToRad(float degs)
{
	return degs * 3.141592653589793f / 180.f;
}
PVector3 Game_Math::Prot_to_direction(PVector3 rot) {
	float x = Game_Math::degToRad(rot.x);
	float z = Game_Math::degToRad(rot.z);

	float num = abs(cos(x));

	return PVector3{ -sin(z) * num, cos(z) * num, sin(x) };
}
Vector3 Game_Math::rot_to_direction(Vector3 rot) {

	float num = abs(cos(rot.x));

	return Vector3{ -sin(rot.z) * num, cos(rot.z) * num, sin(rot.x) };
}
bool Game_Math::raycast(Entity* ent, Cam camera)
{
	BOOL hit = false;
	PVector3 endCoords = PVector3(0, 0, 0);
	PVector3 surfaceNormal = PVector3(0, 0, 0);

	PVector3 camCoords = CAM::GET_CAM_COORD(camera);
	PVector3 rot = CAM::GET_CAM_ROT(camera, 2);
	PVector3 dir = Game_Math::Prot_to_direction(rot);
	Vector3 farCoords = Vector3(0, 0, 0);

	farCoords.x = camCoords.x + dir.x * 10000;
	farCoords.y = camCoords.y + dir.y * 10000;
	farCoords.z = camCoords.z + dir.z * 10000;

	int ray = WORLDPROBE::_CAST_RAY_POINT_TO_POINT(camCoords.x,
		camCoords.y,
		camCoords.z,
		farCoords.x,
		farCoords.y,
		farCoords.z,
		-1,
		0,
		7);
	WORLDPROBE::GET_SHAPE_TEST_RESULT(ray, &hit, &endCoords, &surfaceNormal, ent);

	return (bool)hit;
}

inline ImVec2 Game_Math::WorldToScreen(Vector3 pos)
{
	static float ScreenCX = ImGui::GetIO().DisplaySize.x;
	static float ScreenCY = ImGui::GetIO().DisplaySize.y;

	uint64_t viewport = *(uint64_t*)(g_pattern->g_ViewPort);
	D3DXMATRIX matrix = *(D3DXMATRIX*)(viewport + 0x24C);

	D3DXVECTOR3 screenPosition;

	D3DXMatrixTranspose(&matrix, &matrix);
	D3DXVECTOR4 vectorX = D3DXVECTOR4(matrix._21, matrix._22, matrix._23, matrix._24);
	D3DXVECTOR4 vectorY = D3DXVECTOR4(matrix._31, matrix._32, matrix._33, matrix._34);
	D3DXVECTOR4 vectorZ = D3DXVECTOR4(matrix._41, matrix._42, matrix._43, matrix._44);


	screenPosition.x = (vectorX.x * pos.x) + (vectorX.y * pos.y) + (vectorX.z * pos.z) + vectorX.w;
	screenPosition.y = (vectorY.x * pos.x) + (vectorY.y * pos.y) + (vectorY.z * pos.z) + vectorY.w;
	screenPosition.z = (vectorZ.x * pos.x) + (vectorZ.y * pos.y) + (vectorZ.z * pos.z) + vectorZ.w;
	if (screenPosition.z <= 0.01f)
		return ImVec2(0, 0);

	screenPosition.z = 1.0f / screenPosition.z;
	screenPosition.x *= screenPosition.z;
	screenPosition.y *= screenPosition.z;

	float xTmp = ScreenCX / 2;
	float yTmp = ScreenCY / 2;

	screenPosition.x += xTmp + (int)(0.5f * screenPosition.x * ScreenCX + 0.5f);
	screenPosition.y = yTmp - (int)(0.5f * screenPosition.y * ScreenCY + 0.5f);

	return ImVec2(screenPosition.x, screenPosition.y);
}


//Implementation copied from WINE project d3dx9_36/math.c
inline void D3DXVec3Transformx(D3DXVECTOR4* out, const D3DXVECTOR3* pv, const D3DXMATRIX* pm) {
	D3DXVECTOR4 result;

	result.x = pm->m[0][0] * pv->x + pm->m[1][0] * pv->y + pm->m[2][0] * pv->z + pm->m[3][0];
	result.y = pm->m[0][1] * pv->x + pm->m[1][1] * pv->y + pm->m[2][1] * pv->z + pm->m[3][1];
	result.z = pm->m[0][2] * pv->x + pm->m[1][2] * pv->y + pm->m[2][2] * pv->z + pm->m[3][2];
	result.w = pm->m[0][3] * pv->x + pm->m[1][3] * pv->y + pm->m[2][3] * pv->z + pm->m[3][3];

	*out = result;
	//return out;
}
inline Vector3 Game_Math::GetBone(CPed* cPed, unsigned int Bone_Mask)
{
	auto bonemanager = *(D3DXMATRIX*)(cPed + 0x60);
	auto bone = *(D3DXVECTOR3*)(cPed + ((g_offsets->isb2802 ? 0x410 : 0x430) + Bone_Mask * 0x10));
	D3DXVECTOR4 transform;
	D3DXVec3Transformx(&transform, &bone, &bonemanager);
	return Vector3(transform.x, transform.y, transform.z);
}
/*
ImVec2 Game_Math::GetBone_W2S(CPed* cPed, unsigned int Bone_Mask)
{
	__m128 tempVec4;
	g_pattern->tGet_BONE_POS_func((uint64_t)cPed, &tempVec4, (int32_t)Bone_Mask);
	return this->WorldToScreen(Vector3::FromM128(tempVec4));
}*/
ImVec2 Game_Math::GetBone_W2S_Test(CPed* cPed, const char* bone_name)
{
	return { 0, 0 }; 
}

ImVec2 Game_Math::GetBone_W2S(CPed* cPed, unsigned int Bone_Mask)
{
	return this->WorldToScreen(this->GetBone(cPed, Bone_Mask));
}

ImVec2 Game_Math::WorldToRadar(Vector3 Location, FLOAT RadarX, FLOAT RadarY, float RadarSize, float RadarZoom)
{
	CPedFactory* World = (CPedFactory*)*(uint64_t*)(g_pattern->g_worldFactory);
	CPed* LocalPlayer = World->m_local_ped();

	Vector3 localpos = LocalPlayer->m_visual_pos();

	ImVec2 Return(0, 0);
	DWORD64 addr = g_pattern->GetCameraViewAngles();
	if (!addr) { /*Log::Msg("addy null");*/ return Return; }
	Vector3 TPS = *(Vector3*)(addr + 0x03D0);

	float rot = acosf(TPS.x) * 180.0f / DirectX::XM_PI;
	if (asinf(TPS.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;

	float ForwardDirection = DirectX::XMConvertToRadians(rot);
	FLOAT CosYaw = cos(ForwardDirection);
	FLOAT SinYaw = sin(ForwardDirection);

	FLOAT DeltaX = Location.x - localpos.x;
	FLOAT DeltaY = Location.y - localpos.y;

	FLOAT LocationX = (DeltaY * CosYaw - DeltaX * SinYaw) / RadarZoom;
	FLOAT LocationY = (DeltaX * CosYaw + DeltaY * SinYaw) / RadarZoom;

	if (LocationX > RadarSize / 2.0f - 2.5f)
		LocationX = RadarSize / 2.0f - 2.5f;
	else if (LocationX < -(RadarSize / 2.0f - 2.5f))
		LocationX = -(RadarSize / 2.0f - 2.5f);

	if (LocationY > RadarSize / 2.0f - 2.5f)
		LocationY = RadarSize / 2.0f - 2.5f;
	else if (LocationY < -(RadarSize / 2.0f - 2.5f))
		LocationY = -(RadarSize / 2.0f - 2.5f);

	Return.x = -LocationX + RadarX;
	Return.y = -LocationY + RadarY;

	return Return;
}

float Game_Math::pythag(ImVec2 src, ImVec2 dst)
{
	return sqrtf(pow(src.x - dst.x, 2) + pow(src.y - dst.y, 2));
}

CVehicleManager* Game_Math::GetBestVehicle()
{
	CVehicleManager* ClosestPlayer = nullptr;
	float closestdist = FLT_MAX;
	float dist;
	CPedFactory* g_pedFactory = (CPedFactory*)*(DWORD64*)(g_pattern->g_worldFactory);
	if (!g_pedFactory)
		return NULL;

	CPed* LocalPlayer = g_pedFactory->m_local_ped();
	if (!LocalPlayer)
		return NULL;

	CReplayInterface* ReplayInterface = (CReplayInterface*)*(DWORD64*)(g_pattern->g_replayInterface);
	if (!ReplayInterface)
		return NULL;

	CVehicleInterface* VehicleInterface = ReplayInterface->m_vehicleinterface();
	if (!VehicleInterface)
		return NULL;

	for (int index = 0; index < VehicleInterface->m_vehiclemax(); index++)
	{
		CVehicleManager* Vehicle = VehicleInterface->m_vehiclelist()->m_getvehicle(index);
		if (!Vehicle) continue;

		Vector3 DistanceCalculation = (LocalPlayer->m_visual_pos() - Vehicle->m_visual_pos());
		double Distance = sqrtf(DistanceCalculation.x * DistanceCalculation.x + DistanceCalculation.y * DistanceCalculation.y + DistanceCalculation.z * DistanceCalculation.z);

		if (Distance <= 150)
		{
			ImVec2 PedPos = g_Math->WorldToScreen(Vehicle->m_visual_pos());
			static ImVec2 middle = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
			dist = g_Math->pythag(middle, PedPos);

			if (dist < closestdist)
			{
				{
					closestdist = dist;
					ClosestPlayer = Vehicle;
				}
			}
		}
	}
	return ClosestPlayer;
}
inline void FixAimVectors(DWORD64 addr)
{
	//__try
	{
		DWORD64 camParams = *(DWORD64*)(addr + 0x10);
		if (p_variables->p_vehicle.imOnVehicle)
		{
			if (*(float*)(camParams + 0x2AC) == -2)
			{
				*(float*)(camParams + 0x2AC) = 0.0f;
				*(float*)(camParams + 0x2C0) = 111.0f;
				*(float*)(camParams + 0x2C4) = 111.0f;
			}
		}
		else
		{
			if (*(float*)(camParams + 0x130) == 8.0f)
			{
				*(float*)(camParams + 0x130) = 111.0f; //def 8.0f
				*(float*)(camParams + 0x134) = 111.0f; //def 10.0f
				*(float*)(camParams + 0x4CC) = 0.0f;   //def 4.0f

				if (*(float*)(camParams + 0x49C) == 1.0f)
					*(float*)(camParams + 0x49C) = 0.0f;   //def 1.0f

				*(float*)(camParams + 0x2AC) = 0.0f;   //def -3.0f
				*(float*)(camParams + 0x2B0) = 0.0f;   //def -8.0f
			}
		}
	}
	//__except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {}
}
void Game_Math::set_aim_angels(Vector3 point, float deltaTime, int smoothSet)
{
	CPedFactory* g_pedFactory = (CPedFactory*)*(DWORD64*)(g_pattern->g_worldFactory);
	CPed* LocalPlayer = g_pedFactory->m_local_ped();

	DWORD64 addr = g_pattern->GetCameraViewAngles();
	if (addr)
	{
		FixAimVectors(addr);

		Vector3 velocity;
		if (!smoothSet) smoothSet = p_variables->p_aimbot.smoothness;
		float threshold = 0.25f / smoothSet;
		Vector3 eyesPos = *(Vector3*)(addr + 0x60);

		Vector3 vec3Diff(eyesPos.x - point.x, eyesPos.y - point.y, eyesPos.z - point.z);
		float distance = vec3Diff.Length();

		auto vehicle = LocalPlayer->m_vehicle_manager();
		if (vehicle &&p_variables->p_vehicle.imOnVehicle)
			velocity = vehicle->GetVelocity();
		else
			velocity = LocalPlayer->velocity();

		if(p_variables->p_aimbot.prediction)
			eyesPos += (velocity / deltaTime);

		
		Vector3 Output = Vector3((point.x - eyesPos.x) / distance, (point.y - eyesPos.y) / distance, (point.z - eyesPos.z) / distance);
		Output.Normalize();

		Vector3 Origin = *(Vector3*)(addr + 0x40);
		Vector3 Delta = Origin - Output;
		Delta.Normalize();

		if (p_variables->p_aimbot.aimbot_type == 0)
		{
			*(Vector3*)(addr + 0x40) = Output;  //FPS
			*(Vector3*)(addr + 0x3D0) = Output; //TPS
			//*(Vector3*)(addr + 0x3C0) = Output; //TPS
		}
		else
		{
			if(smoothSet > 0)
			{
				if (Delta.x < 0 && -Delta.x > threshold) Delta.x = -threshold;//no faster than step
				if (Delta.y < 0 && -Delta.y > threshold) Delta.y = -threshold;
				if (Delta.z < 0 && -Delta.z > threshold) Delta.z = -threshold;
				Vector3 afterSmooth = Origin - Delta;

				*(Vector3*)(addr + 0x40) = afterSmooth;  //FPS
				*(Vector3*)(addr + 0x3D0) = afterSmooth; //TPS

			}
			else
			{
				if (p_variables->p_aimbot.aimbot_type == 1)
				{
					*(Vector3*)(addr + 0x40) = Output;  //FPS
					*(Vector3*)(addr + 0x3D0) = Output; //TPS
				}
				else if (p_variables->p_aimbot.aimbot_type == 2)
				{
					*(Vector2*)(addr + 0x40) = Vector2(Output.x, Output.y);  //FPS
					*(Vector2*)(addr + 0x3D0) = Vector2(Output.x, Output.y);  //TPS
				}
			}
		}
	}
}

void Game_Math::MouseAim(Vector3 AimCoord, int smooth)
{
	static float center_x = ImGui::GetIO().DisplaySize.x/2.f;
	static float center_y = ImGui::GetIO().DisplaySize.y/2.f;
	auto AimCoordW2s = WorldToScreen(AimCoord);

	float ScreenLocX = AimCoordW2s.x - center_x;
	float ScreenLocY = AimCoordW2s.y - center_y;
	
	if (!smooth) smooth = p_variables->p_aimbot.smoothness;
	float step = 10.f * ((float)smooth / 100.f);//convert smooth to step
	float AimLocX = ScreenLocX /= step + 0.5f;
	float AimLocY = ScreenLocY /= step + 0.5f;


	/*
	float step = 60 - 60.f * ((float)smooth / 100.f);//convert smooth to step
	float distance2d = std::sqrt(ScreenLocX * ScreenLocX + ScreenLocY * ScreenLocY);
	if (distance2d < step) step = distance2d/10;
	
	if (ScreenLocX<0 && -ScreenLocX > step) ScreenLocX = -step;//no faster than step
	if (ScreenLocX > step) ScreenLocX = step;
	if (ScreenLocY<0 && -ScreenLocY > step) ScreenLocY = -step;
	if(ScreenLocY > step) ScreenLocY = step;
	*/
	static auto TickCount = GetTickCount();
	if (GetTickCount() - TickCount > 20)
	{
		TickCount = GetTickCount();
		SAFE_CALL(mouse_event)(MOUSEEVENTF_MOVE, static_cast<DWORD>(AimLocX), static_cast<DWORD>(AimLocY), 0, 0);
	}


}