#include <cache.h>
#include <GameMath.h>
#include <invoker.hpp>
void Initializezzzz()
{

	auto middle_x = g_Math->WindowSize.x / 2;
	auto middle_y = g_Math->WindowSize.y / 2;


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



	for (int i = 0; i < pedinterface->m_pedmax(); i++)
	{
		CPed* cPed = pedinterface->m_pedlist()->m_ped(i);
		if (!cPed) continue;

		float health = cPed->health() - 100;
		float armor = *(float*)(cPed + g_offsets->m_armor);
		if (health <= 0) continue;


		if (cPed->IsAnimals())
		{
			continue;
		}
		cEntity newEntity;

		uintptr_t player_info = *(uintptr_t*)(cPed + g_offsets->m_player_info);
		void* addr = (void*)(player_info + 0x20);
		const char* name = "nullptr";
		if (*(BYTE*)g_pattern->GetPlayerNameFromScAddr == 0xE9)
			name = g_pattern->GetPlayerNameFromScAddr(addr);

		newEntity.id = 1;
		newEntity.index = i;
		newEntity.relativePos = cPed->m_visual_pos();
		newEntity.Name = name;
		newEntity.Head = g_Math->GetBone(cPed, ePedBoneType::HEAD);
		newEntity.Neck = g_Math->GetBone(cPed, ePedBoneType::NECK);
		newEntity.Pelvis = g_Math->GetBone(cPed, ePedBoneType::ABDOMEN);
		newEntity.RFoot = g_Math->GetBone(cPed, ePedBoneType::R_FOOT);
		newEntity.LFoot = g_Math->GetBone(cPed, ePedBoneType::L_FOOT);
		newEntity.LHand = g_Math->GetBone(cPed, ePedBoneType::L_HAND);
		newEntity.RHand = g_Math->GetBone(cPed, ePedBoneType::R_HAND);
		p_cached.entities.push_back(newEntity);
	}
}