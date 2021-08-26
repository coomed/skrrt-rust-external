#ifndef _FEATURES_HPP
#define _FEATURES_HPP

// includes for this header
#include "../utils/utils.hpp"
#include "../utils/globals.hpp"
#include "../classes/ents/player.hpp"
#include "../classes/game_object_manager.hpp"

// features class
class Features
{
public: // public variables for this class
	std::map<uint64_t, HeldItem> weapons;  // stores all weapon properties, so it's 
											// easier for us to revert changes to them
	std::map<std::string, bool> hooked;

public: // public methods for this class
	void SetRecoil(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		if (weapon.second <= 0xFFFFFFFF) return;

		// insert held weapon into weapons table if it doesn't exist already
		if (weapons.find(weapon.second) == weapons.end())
			weapons[weapon.second] = HeldItem(weapon.second);

		// ensure bp was read correctly
		if (weapons[weapon.second].bp <= 0xFFFFFFFF)
		{
			weapons[weapon.second] = HeldItem(weapon.second);
			return;
		}

		// edit recoil
		if (weapons[weapon.second].recoil != settings->features.recoil)
			weapons[weapon.second].SetRecoil(settings->features.recoil);
	}

	void SetSpread(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		if (weapon.second <= 0xFFFFFFFF) return;

		// insert held weapon into weapons table if it doesn't exist already
		if (weapons.find(weapon.second) == weapons.end())
			weapons[weapon.second] = HeldItem(weapon.second);

		// ensure bp was read correctly
		if (weapons[weapon.second].bp <= 0xFFFFFFFF)
		{
			weapons[weapon.second] = HeldItem(weapon.second);
			return;
		}

		// edit spread
		if (weapons[weapon.second].spread != settings->features.spread)
			weapons[weapon.second].SetSpread(settings->features.spread);
	}

	void SetAutomatic(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		if (weapon.second <= 0xFFFFFFFF) return;

		// insert held weapon into weapons table if it doesn't exist already
		if (weapons.find(weapon.second) == weapons.end())
			weapons[weapon.second] = HeldItem(weapon.second);

		// ensure bp was read correctly
		if (weapons[weapon.second].bp <= 0xFFFFFFFF)
		{
			weapons[weapon.second] = HeldItem(weapon.second);
			return;
		}

		// edit automatic if automatic is set to false for the weapon and force auto is toggled on
		if (weapons[weapon.second].automatic != settings->features.force_auto && (weapons[weapon.second].c_properties.automatic == false || weapons[weapon.second].automatic == true))
			weapons[weapon.second].SetAutomatic(settings->features.force_auto);
	}

	void CanJump(uint64_t base_movement)
	{
		pCheck(base_movement);

		rust->mem->Write<float>(base_movement + offsets->jumpTime, 0.f);

		rust->mem->Write<float>(base_movement + offsets->groundTime, rust->mem->Read<float>(base_movement + 0xBC) + 0.5f);

		rust->mem->Write<float>(base_movement + offsets->landTime, 0.f);
	}

	void SetSway(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		if (weapon.second <= 0xFFFFFFFF) return;

		// insert held weapon into weapons table if it doesn't exist already
		if (weapons.find(weapon.second) == weapons.end())
			weapons[weapon.second] = HeldItem(weapon.second);

		// ensure bp was read correctly
		if (weapons[weapon.second].bp <= 0xFFFFFFFF)
		{
			weapons[weapon.second] = HeldItem(weapon.second);
			return;
		}

		// edit automatic if automatic is set to false for the weapon and force auto is toggled on
		if (weapons[weapon.second].sway != settings->features.sway)
			weapons[weapon.second].SetSway(settings->features.sway);
	}

	void InstantBow(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		pCheck(weapon.second);

		// ensure weapon is bow
		if (weapon.first != std::string(skCrypt("bow.hunting"))) return;

		// ensure weapon has been processed
		if (weapons.find(weapon.second) == weapons.end()) return;

		pCheck(weapons[weapon.second].bp);

		// write to attack ready & aiming
		rust->mem->Write<bool>(weapons[weapon.second].bp + offsets->attackReady, true);
		rust->mem->Write<bool>(weapons[weapon.second].bp + offsets->aiming, true);
	}

	void InstantCompound(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		pCheck(weapon.second);

		// ensure weapon is bow
		if (weapon.first != std::string(skCrypt("bow.compound"))) return;

		// ensure weapon has been processed
		if (weapons.find(weapon.second) == weapons.end()) return;

		pCheck(weapons[weapon.second].bp);

		// write to current hold progress
		rust->mem->Write<float>(weapons[weapon.second].bp + offsets->stringHoldDurationMax, 0.f);
		rust->mem->Write<float>(weapons[weapon.second].bp + offsets->movementPenaltyRampUpTime, 0.f);
	}

	void PlayerFly(uint64_t base_movement)
	{
		pCheck(base_movement);

		// ensure player is not jumping and player is flying
		if (GetAsyncKeyState(settings->features.fly.keybind) && !rust->mem->Read<bool>(base_movement + 0x138))
		{
			rust->mem->Write<float>(base_movement + offsets->groundAngle, 0.f);
			rust->mem->Write<float>(base_movement + offsets->groundAngleNew, 0.f);
			rust->mem->Write<float>(base_movement + offsets->gravityMultiplier, 0.f);
			rust->mem->Write<float>(base_movement + offsets->gravityMultiplierSwimming, -3.f);
		}
		else
		{
			rust->mem->Write<float>(base_movement + offsets->gravityMultiplier, 2.5f);
			rust->mem->Write<float>(base_movement + offsets->gravityMultiplierSwimming, 1.f);
		}
	}

	void SetTime(float time) {
		uint64_t skyAddr = rust->mem->ReadChain<uint64_t>(gom->GetSky(), { 0x30, 0x18, 0x28, 0x38});

		pCheck(skyAddr);

		rust->mem->Write<float>(skyAddr + 0x10, time);
	}

	void SetFov(float fov)
	{
		uint64_t cam_manager = rust->mem->ReadChain<uint64_t>(rust->mem->ga_addy, { 0x2FF7FE0, 0xB8 });

		pCheck(cam_manager);

		rust->mem->Write<float>(cam_manager + offsets->camera_fov, 1.f);
	}

	void AdminFlags()
	{
		if (!globals->local_player.HasPlayerFlag(PlayerFlags::IsAdmin))
			globals->local_player.GivePlayerFlag(PlayerFlags::IsAdmin);
	}

	void Spiderman(uint64_t base_movement)
	{
		pCheck(base_movement);

		rust->mem->Write<float>(base_movement + offsets->groundAngle, 0.f);
		rust->mem->Write<float>(base_movement + offsets->groundAngleNew, 0.f);
		
	}

	void UnlockAngles()
	{
		rust->mem->Write<bool>(globals->local_player.ent + offsets->clothingBlocksAiming, false);
		rust->mem->Write<bool>(globals->local_player.ent + offsets->equippingBlocked, false);
	}

	void NoShotgunSpread(std::pair<std::string, uint64_t> weapon) {
		// ensure weapon is valid
		if (weapon.second <= 0xFFFFFFFF) return;

		// ensure weapon has been processed
		if (weapons.find(weapon.second) == weapons.end()) return;

		uint64_t bp = weapons[weapon.second].bp;

		uint64_t projectileUnityList = rust->mem->Read<uint64_t>(bp + offsets->createdProjectiles);
		uint64_t projectileList = driver::read<uintptr_t>(projectileUnityList + 0x10);

		UINT size = rust->mem->Read<UINT>(projectileList + 0x18);

		for (UINT i = 0; i < size; i++) {
			uint64_t projectile = rust->mem->Read<uint64_t>(projectileList + 0x20 + (i * 0x08));
			uint64_t projectileMod = rust->mem->Read<uint64_t>(projectile + offsets->projectileItemMod);

			if (IsValidPointer(projectileMod)) {
				rust->mem->Write(projectileMod + offsets->projectileSpread, 0.f);
			}
		}
	}

	void InstantEoka(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		pCheck(weapon.second <= 0xFFFFFFFF);

		// ensure weapon is eoka
		if (weapon.first != std::string(skCrypt("pistol.eoka"))) return;

		// ensure weapon has been processed
		if (weapons.find(weapon.second) == weapons.end()) return;

		pCheck(weapons[weapon.second].bp);
		// write to success fraction
		rust->mem->Write<float>(weapons[weapon.second].bp + offsets->successFraction, 1.f);
	}

	void ExtendedMelee(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		pCheck(weapon.second);

		// ensure weapon has been processed
		if (weapons.find(weapon.second) == weapons.end()) return;

		// ensure weapon isnt a gun
		if (weapons[weapon.second].IsWeapon()) return;

		pCheck(weapons[weapon.second].bp);

		// write to maxDistance
		rust->mem->Write<float>(weapons[weapon.second].bp + offsets->maxDistance, 5.f);
	}


	void NoBlockSprint(std::pair<std::string, uint64_t> weapon)
	{
		// ensure weapon is valid
		if (weapon.second <= 0xFFFFFFFF) return;

		// ensure weapon has been processed
		if (weapons.find(weapon.second) == weapons.end()) return;

		// ensure weapon isnt a gun
		if (weapons[weapon.second].IsWeapon()) return;

		// write to blockSprintOnAttack
		rust->mem->Write<bool>(weapons[weapon.second].bp + offsets->blockSprintOnAttack, false);
	}

};

extern Features* features;

#endif // _FEATURES_HPP