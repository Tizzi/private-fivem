#include <Weapon.h>
#include <invoker.hpp>
#include <GameMath.h>
 void WeaponFuncs::Initialize(struct timer timer)
{
	 static bool once = true;
	 if (0)
	 {
		 // Create a vector
		 std::vector<int> myVector = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};

		 // Use a pointer to member function to get the address of size()
		 size_t(std::vector<int>:: * sizeFunctionPointer)() const = &std::vector<int>::size;

		 // Call size() using the pointer
		 size_t vectorSize = (myVector.*sizeFunctionPointer)();

		 // Print the size and the address of the size() function
		 Log::Msg("Vector size: %d", vectorSize);
		 Log::Msg("Address of size() function: %llx", &std::vector<char>::size);
		 //d3d10.std::vector<char, std::allocator<char> >::size - 48 8B 41 08 - mov rax, [rcx + 08]

		 once = false;
	 }

	 

	CPedFactory* g_pedFactory = (CPedFactory*)*(uintptr_t*)(g_pattern->g_worldFactory);
	if (!g_pedFactory) return;
	CWeaponInfo* g_weaponInfo = g_pedFactory->m_local_ped()->m_weapon_manager()->m_weapon_info(); 
	if (!g_weaponInfo) return;


	if (p_variables->p_Weapons.NoRecoil)
	{
		g_weaponInfo->m_shake_recoil(0.0f); 
	}
	if (p_variables->p_Weapons.NoSpread)
	{
		g_weaponInfo->m_spread(0.0f);
	}
	if (p_variables->p_Weapons.NoReload)
	{
		g_weaponInfo->m_reload_anim(150.0f);
	}
	if (p_variables->p_Weapons.Rapid_Fire)
	{
		//g_weaponInfo->m_bullet_in_patch(300.f); 
		g_weaponInfo->SetSpeedBullet(p_variables->p_Weapons.fireRate);
		PED::SET_PED_SHOOT_RATE(PLAYER::PLAYER_PED_ID(), p_variables->p_Weapons.fireRate);
		
	}
	if (p_variables->p_Weapons.range)
	{
		g_weaponInfo->SetRange(1000);
	}
	cAmmoInfo* AmmoInfo = g_weaponInfo->AmmoInfo();
	if (!AmmoInfo)
		return;

	cAmmoCount* AmmoCount = AmmoInfo->AmmoCount();
	if (!AmmoCount)
		return;
	cFixedAmmoCount* FixedAmmoCount = AmmoCount->FixedAmmoCount();
	if (!FixedAmmoCount)
		return;
	if (p_variables->p_Weapons.InfiniteAmmo)
	{
		FixedAmmoCount->SetAmmo(9999);
	}
	if (p_variables->p_Weapons.ammoSet)
	{
		FixedAmmoCount->SetAmmo(p_variables->p_Weapons.ammoCount);

	}
	if (p_variables->p_Weapons.damageMultiply)
	{
		//PLAYER::SET_PLAYER_WEAPON_DAMAGE_MODIFIER(PLAYER::PLAYER_PED_ID(), p_variables->p_Weapons.dmgModifier);
		WEAPON::SetWeaponDamageModifier(WEAPON::GET_SELECTED_PED_WEAPON(PLAYER::PLAYER_PED_ID()), p_variables->p_Weapons.dmgModifier);
	}
}	