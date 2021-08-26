#ifndef _PLAYER_HPP
#define _PLAYER_HPP

// includes for this header
#include <cstdint>
#include <string>
#include <xmmintrin.h>
#include <mutex>
#include <emmintrin.h>
#include "../../utils/utils.hpp"
#include <future>
#include "../../utils/memory/memory.hpp"
#include "../../utils/utils.hpp"

// playerflags enum from assembly
enum class PlayerFlags
{
	Unused1 = 1,
	Unused2 = 2,
	IsAdmin = 4,
	ReceivingSnapshot = 8,
	Sleeping = 16,
	Spectating = 32,
	Wounded = 64,
	IsDeveloper = 128,
	Connected = 256,
	ThirdPersonViewmode = 1024,
	EyesViewmode = 2048,
	ChatMute = 4096,
	NoSprint = 8192,
	Aiming = 16384,
	DisplaySash = 32768,
	Relaxed = 65536,
	SafeZone = 131072,
	ServerFall = 262144,
	Workbench1 = 1048576,
	Workbench2 = 2097152,
	Workbench3 = 4194304
};

// boneid enum 
enum Bones : int
{
	l_hip = 1,
	l_knee = 2,
	l_foot = 3,
	l_toe = 4,
	l_ankle_scale = 5,
	pelvis = 6,
	penis = 7,
	r_hip = 13,
	r_knee = 14,
	r_foot = 15,
	r_toe = 16,
	spine1 = 18,
	spine2 = 20,
	spine3 = 21,
	spine4 = 22,
	l_upperarm = 24,
	l_forearm = 25,
	l_hand = 26,
	neck = 46,
	head = 47, // head 47
	jaw = 48,
	r_upperarm = 55,
	r_forearm = 56,
	r_hand = 57,
};

// class to represent a player in rust
class Player
{
public:

	// default constructor
	Player() {}

	// constructor for this class
	Player(uint64_t _ent)
	{
		// store the constructor arguments
		this->ent = _ent;

		// get the player's name
		this->name = this->GetPlayerName();
		//this->name = L"";

		// check if the player is localplayer
		this->local_player = rust->mem->ReadChain<bool>(this->ent, { (uint64_t)offsets->playerModel, (uint64_t)offsets->isLocalPlayer });

		// get the bone transform addy
		this->bone_transforms = rust->mem->ReadChain<uint64_t>(this->ent, { (uint64_t)offsets->entityModel, (uint64_t)offsets->boneTransforms });
	
		// update this player
		this->Update();
	}

	// used to calculate the distance to another player
	int Distance(Player* player)
	{
		return this->position.Distance(player->position);
	}

	// used to get the distance as a string
	std::string DistanceStr(Player* player)
	{
		// will store the distance string
		char distance[10];

		// get the distance
		float dist = this->Distance(player);

		// copy the string into the output array
		printf(distance, "%.2f", dist);

		// return the distance string
		return distance;
	}

	// used to update the player object
	bool Update()
	{
		// read the player's position
		this->UpdatePosition();

		// read the player's health
		this->health = rust->mem->Read<float>(this->ent + offsets->health);

		// check player health
		if (this->health < 1 || this->health > 500) return false;

		// read the playerflags
		this->player_flags = rust->mem->Read<int>(this->ent + offsets->playerFlags);

		// update held items
		this->UpdateHeldItems();

		return true;
	}

	Vector3 GetVelocity()
	{
		// get to the playermodel
		uint64_t player_model = rust->mem->Read<uint64_t>(this->ent + offsets->playerModel);

		// read and return the velocity vector
		return rust->mem->Read<Vector3>(player_model + offsets->playerVelocity);
	}

	// used to update the player's position
	void UpdatePosition()
	{
		// update the bones map
		this->UpdateBones();

		// read the player's position
		this->position = (this->bones[l_foot] + this->bones[r_foot]) / 2;
	}

	// used to check if the player has a flag
	bool HasPlayerFlag(PlayerFlags flag)
	{
		return (this->player_flags & static_cast<int32_t>(flag)) == static_cast<int32_t>(flag);
	}

	// used to give the player a flag
	void GivePlayerFlag(PlayerFlags flag)
	{
		// write to playerflags
		rust->mem->Write(this->ent + offsets->playerFlags, this->player_flags | static_cast<int32_t>(flag));;
	}

	// used to determine weather or not the player is connected
	bool IsConnected()
	{
		return this->HasPlayerFlag(PlayerFlags::Connected);
	}

	struct Matrix3x4
	{
		byte v0[16];
		byte v1[16];
		byte v2[16];
	};

	template<unsigned i>
	float vectorGetByIndex(__m128 V) {
		union {
			__m128 v;
			float a[4];
		} converter;
		converter.v = V;
		return converter.a[i];
	}

	// used to update the player's bone position
	void UpdateBones()
	{
		// an bone array we can iterate over
		int bone_array[] = { 1, 2, 3, 4, 5, 6, 7, 13, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 46, 47, 48, 55, 56, 57 };

		// iterate through the bone array
		for (int i : bone_array)
		{
			// get the address of the bone
			uint64_t bone = rust->mem->Read<uint64_t>(this->bone_transforms + (0x20 + (i * 0x8)));

			// check that we got the bone
			if (!bone)
			{
				// add an empty transform to the list
				this->bones[(Bones)i] = Vector3();

				// skip this bone
				continue;
			}

			// get the transform address
			uint64_t transform = rust->mem->Read<uint64_t>(bone + 0x10);

			// _m128 -> Vec4
			const __m128 mul_vec0 = { -2.000, 2.000, -2.000, 0.000 };
			const __m128 mul_vec1 = { 2.000, -2.000, -2.000, 0.000 };
			const __m128 mul_vec2 = { -2.000, -2.000, 2.000, 0.000 };

			// read the transform index for this bone
			int index = rust->mem->Read<int>(transform + 0x40);

			// read the transform data ptr
			uint64_t p_transform_data = rust->mem->Read<uint64_t>(transform + 0x38);

			// designate the transform data array
			uint64_t transform_data[2];

			// read the player transform data into the transform data array
			rust->mem->Read((p_transform_data + 0x18), &transform_data, 16);

			// verify transform data
			if (!transform_data[0] || !transform_data[1])
			{
				// add an empty transform to the list
				this->bones[(Bones)i] = Vector3();

				// skip this bone
				continue;
			}

			// set the size of the matricies buffer
			size_t size_matricies_buffer = 48 * index + 48;

			// set the indicies buffer size
			size_t size_indices_buffer = 4 * index + 4;

			// allocate memory to the matricies buffer
			PVOID p_matricies_buffer = malloc(size_matricies_buffer);

			// allocate memory to the indices buffer
			PVOID p_indices_buffer = malloc(size_indices_buffer);

			if (p_matricies_buffer == nullptr || p_indices_buffer == nullptr)
			{
				// prevent mem leak
				free(p_matricies_buffer);
				free(p_indices_buffer);

				// add an empty transform to the list
				this->bones[(Bones)i] = Vector3();

				// skip this bone
				continue;
			}

			if (p_matricies_buffer && p_indices_buffer)
			{
				// transform position if matrices buffer and indices buffer are both correct
				uint64_t max_indicies_addy = (uint64_t)p_indices_buffer + size_indices_buffer;
				uint64_t min_indicies_addy = (uint64_t)p_indices_buffer;

				uint64_t max_matricies_addy = (uint64_t)p_matricies_buffer + size_matricies_buffer;
				uint64_t min_matricies_addy = (uint64_t)p_matricies_buffer;

				// read the matricies buffer into an array
				rust->mem->Read(transform_data[0], p_matricies_buffer, static_cast<uint32_t>(size_matricies_buffer));

				// read the indices buffer into an array
				rust->mem->Read(transform_data[1], p_indices_buffer, static_cast<uint32_t>(size_indices_buffer));

				// get result addy
				uint64_t a_result = (uint64_t)p_matricies_buffer + 0x30 * index;

				// sanity check
				if (a_result < min_matricies_addy || a_result > max_matricies_addy)
				{
					// prevent mem leak
					free(p_matricies_buffer);
					free(p_indices_buffer);

					// add an empty transform to the list
					this->bones[(Bones)i] = Vector3();

					continue;
				}

				// uint64_t -> __m128
				__m128 result = *(__m128*)(a_result);

				// get transform index addy
				uint64_t transformindex_a = (uint64_t)p_indices_buffer + 0x4 * index;

				// sanity check
				if (transformindex_a < min_indicies_addy || transformindex_a > max_indicies_addy)
				{
					// prevent mem leak
					free(p_matricies_buffer);
					free(p_indices_buffer);

					// add an empty transform to the list
					this->bones[(Bones)i] = Vector3();

					continue;
				}

				// uint64_t -> int
				int transform_index = *(int*)(transformindex_a);

				// used to track loop iterations
				int iterations = 0;

				while (transform_index >= 0 && iterations < 10)
				{
					// get matrix 3x4 address
					uint64_t matrix32_a = (uint64_t)p_matricies_buffer + 0x30 * transform_index;

					// sanity check
					if (matrix32_a < min_matricies_addy || matrix32_a > max_matricies_addy)
					{
						// add an empty transform to the list
						this->bones[(Bones)i] = Vector3();

						continue;
					}

					Matrix3x4 matrix34 = *(Matrix3x4*)(matrix32_a);

					__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.v1), 0x00));
					__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.v1), 0x55));
					__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.v1), 0x8E));
					__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.v1), 0xDB));
					__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.v1), 0xAA));
					__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.v1), 0x71));
					__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.v2), result);

					result = _mm_add_ps(
						_mm_add_ps(
							_mm_add_ps(
								_mm_mul_ps(
									_mm_sub_ps(
										_mm_mul_ps(_mm_mul_ps(xxxx, mul_vec1), zwxy),
										_mm_mul_ps(_mm_mul_ps(yyyy, mul_vec2), wzyw)),
									_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
								_mm_mul_ps(
									_mm_sub_ps(
										_mm_mul_ps(_mm_mul_ps(zzzz, mul_vec2), wzyw),
										_mm_mul_ps(_mm_mul_ps(xxxx, mul_vec0), yxwy)),
									_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
							_mm_add_ps(
								_mm_mul_ps(
									_mm_sub_ps(
										_mm_mul_ps(_mm_mul_ps(yyyy, mul_vec0), yxwy),
										_mm_mul_ps(_mm_mul_ps(zzzz, mul_vec1), zwxy)),
									_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
								tmp7)), *(__m128*)(&matrix34.v0));

					uint64_t new_transform_index_a = (uint64_t)p_indices_buffer + 0x4 * transform_index;

					// sanity check
					if (new_transform_index_a < min_indicies_addy || new_transform_index_a > max_indicies_addy)
					{
						// prevent mem leak
						free(p_matricies_buffer);
						free(p_indices_buffer);

						// add an empty transform to the list
						this->bones[(Bones)i] = Vector3();

						continue;
					}

					transform_index = *(int*)(new_transform_index_a);

					// increment iterations for cpu management
					iterations++;
				}

				// get the world position
				//Vector3 world = Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
				Vector3 world = Vector3(vectorGetByIndex<0>(result), vectorGetByIndex<1>(result), vectorGetByIndex<2>(result));

				// update bone
				bones[(Bones)i] = world;
			}

			// free the allocated memory
			free(p_matricies_buffer);
			free(p_indices_buffer);
		}
	}

	// used to get the player's ASCII name
	std::string GetASCIIName()
	{
		return std::string(this->name.begin(), this->name.end());
	}

	// used to get the player's recoilangles
	Vector3 GetRecoilAngles()
	{
		return rust->mem->ReadChain<Vector3>(this->ent, { (uint64_t)offsets->playerInput, (uint64_t)offsets->recoilAngles });
	}

	// used to get the player's viewangles
	Vector3 GetViewAngles()
	{
		return rust->mem->ReadChain<Vector3>(this->ent, { (uint64_t)offsets->playerInput, (uint64_t)offsets->bodyAngles });
	}

	// used to set the player's viewangles
	void SetViewAngles(Vector3 angles)
	{
		if (angles.x < -89)
			angles.x = -89;

		else if (angles.x > 89)
			angles.x = 89;

		while (angles.y < -360)
			angles.y += 360;

		while (angles.y > 360)
			angles.y -= 360;

		// check for nan (not a number)
		if (!std::isnan(angles.x) && !std::isnan(angles.y)) {
			rust->mem->Write<Vector3>(rust->mem->Read<uint64_t>(this->ent + offsets->playerInput) + offsets->bodyAngles, angles);
		}
	}

	// used to get the address of the mounted entity
	uint64_t GetMountedEntity()
	{
		// read ent_cached from 'mounted' EntityRef
		return rust->mem->Read<uint64_t>(this->ent + offsets->mountedEntRef);
	}

	// used to read the item name of an item
	std::string GetItemName(uint64_t item)
	{
		// read items information
		uint64_t info = rust->mem->Read<uint64_t>(item + 0x20);

		// read display name
		uint64_t display_name = rust->mem->Read<uint64_t>(info + 0x20);

		// read mono string for name
		std::wstring wide_name = rust->mem->ReadUnicode(display_name + 0x14);

		// if string isnt empty, return value
		if (!wide_name.empty())
			return std::string(wide_name.begin(), wide_name.end()).c_str();

		// return nothing if the item is empty
		return "No Item";
	}

private:
	// used to get the player's name
	std::wstring GetPlayerName()
	{
		uint64_t nameaddy = rust->mem->Read<uint64_t>(this->ent + offsets->playerDisplayName);

		if (IsValidPointer(nameaddy)) {
			std::wstring name = rust->mem->ReadUnicode(nameaddy + 0x14);

			if (name.empty()) {
				return L"name not found";
			}

			if (name.find(L"Scientist") == 0)
				return L"Scientist";

			return name;
		} else {
			return L"name not found";
		}
	}

public:
	uint64_t	ent = 0;				// the BaseEntity address
	uint64_t	bone_transforms;		// the address of the player's bone transforms 

	std::pair<std::string, uint64_t>	// holds the player's held items
		held_items[6];

	int helditem = 0;					// holds the player's currently held item index
	bool local_player = false;			// used to determine whether the player is the LocalPlayer.
	std::wstring name = std::wstring();	// the player's in-game name.
	int32_t	player_flags = 0;			// the player's playerflags.
	float health = 0.f;					// the player's current health
	Vector3	position = {};				// the player's 3d position#
	std::map<Bones, Vector3> bones = {};// the player's bones map
	bool same_team;
	void UpdateHeldItems();
};


// WeaponProperties struct
struct WeaponProperties {
	struct recoil_properties {
		float recoilYawMax;
		float recoilYawMin;
		float recoilPitchMax;
		float recoilPitchMin;
		float timeToTakeMin;
		float timeToTakeMax;
	} recoil_properties;
	float aimCone;
	float hipAimCone;
	float aimConePenaltyMax;
	float aimconePenaltyPerShot;
	float stancePenaltyScale;
	bool automatic;
	float aimSway;
};

// helditem class
class HeldItem
{
public:
	// default constructor
	HeldItem() {}

	/* Used to initialize an HeldItem object */
	HeldItem(uint64_t _ent)
	{
		// store the constructor arguments
		this->ent = _ent;

		// read the name of the item
		this->name = GetItemName();

		// get base projectile
		this->bp = rust->mem->Read<uint64_t>(_ent + 0x98);

		// get recoil properties of this weapon
		this->recoil_properties = rust->mem->Read<uint64_t>(this->bp + 0x2C8);

		// init weapon properties
		InitWeaponProperties();

		// get the primary magazine
		this->primary_mag = rust->mem->Read<uint64_t>(this->bp + 0x2A8);
	}

	std::string GetItemName()
	{
		// read the items information 
		uint64_t info = rust->mem->Read<uint64_t>(this->ent + 0x20);

		// read the display name 
		uint64_t display_name = rust->mem->Read<uint64_t>(info + 0x20);

		// read mono string for the name 
		std::wstring wide_name = rust->mem->ReadUnicode(display_name + 0x14);

		// if Mono-String isn't empty then return the found characters as a string 
		if (!wide_name.empty())
			return std::string(wide_name.begin(), wide_name.end()).c_str();

		// return nothing if item is nothing 
		return "No Item";
	}

	void InitWeaponProperties()
	{
		this->o_properties.recoil_properties.recoilPitchMax = rust->mem->Read<float>(this->recoil_properties + offsets->recoilPitchMax);
		this->c_properties.recoil_properties.recoilPitchMax = this->o_properties.recoil_properties.recoilPitchMax;

		this->o_properties.recoil_properties.recoilPitchMin = rust->mem->Read<float>(this->recoil_properties + offsets->recoilPitchMin);
		this->c_properties.recoil_properties.recoilPitchMin = this->o_properties.recoil_properties.recoilPitchMin;

		this->o_properties.recoil_properties.recoilYawMax = rust->mem->Read<float>(this->recoil_properties + offsets->recoilYawMax);
		this->c_properties.recoil_properties.recoilYawMax = this->o_properties.recoil_properties.recoilYawMax;

		this->o_properties.recoil_properties.recoilYawMin = rust->mem->Read<float>(this->recoil_properties + offsets->recoilYawMin);
		this->c_properties.recoil_properties.recoilYawMin = this->o_properties.recoil_properties.recoilYawMin;

		this->o_properties.recoil_properties.timeToTakeMax = rust->mem->Read<float>(this->recoil_properties + offsets->timeToTakeMax);
		this->c_properties.recoil_properties.timeToTakeMax = this->o_properties.recoil_properties.timeToTakeMax;

		this->o_properties.recoil_properties.timeToTakeMin = rust->mem->Read<float>(this->recoil_properties + offsets->timeToTakeMin);
		this->c_properties.recoil_properties.timeToTakeMin = this->o_properties.recoil_properties.timeToTakeMin;

		this->o_properties.aimCone = rust->mem->Read<float>(this->bp + offsets->aimCone);
		this->c_properties.aimCone = this->o_properties.aimCone;

		this->o_properties.hipAimCone = rust->mem->Read<float>(this->bp + offsets->hipAimCone);
		this->c_properties.hipAimCone = this->o_properties.hipAimCone;

		this->o_properties.aimConePenaltyMax = rust->mem->Read<float>(this->bp + offsets->aimConePenaltyMax);
		this->c_properties.aimConePenaltyMax = this->o_properties.aimConePenaltyMax;

		this->o_properties.aimSway = rust->mem->Read<float>(this->bp + offsets->aimSway);
		this->c_properties.aimSway = this->o_properties.aimSway;

		this->o_properties.aimconePenaltyPerShot = rust->mem->Read<float>(this->bp + offsets->aimconePenaltyPerShot);
		this->c_properties.aimconePenaltyPerShot = this->o_properties.aimconePenaltyPerShot;

		this->o_properties.stancePenaltyScale = rust->mem->Read<float>(this->bp + offsets->stancePenaltyScale);
		this->c_properties.stancePenaltyScale = this->o_properties.stancePenaltyScale;

		this->o_properties.automatic = rust->mem->Read<bool>(this->bp + offsets->automatic);
		this->c_properties.automatic = this->o_properties.automatic;
	}

	void SetRecoil(float percentage)
	{
		// ensure this is a valid weapon
		if (!this->IsWeapon()) return;

		// set recoilPitchMax
		this->c_properties.recoil_properties.recoilPitchMax = this->o_properties.recoil_properties.recoilPitchMax * percentage;
		rust->mem->Write<float>(recoil_properties + offsets->recoilPitchMax, this->c_properties.recoil_properties.recoilPitchMax);

		// set recoilPitchMin
		this->c_properties.recoil_properties.recoilPitchMin = this->o_properties.recoil_properties.recoilPitchMin * percentage;
		rust->mem->Write<float>(recoil_properties + offsets->recoilPitchMin, this->c_properties.recoil_properties.recoilPitchMin);

		// set recoilYawMax
		this->c_properties.recoil_properties.recoilYawMax = this->o_properties.recoil_properties.recoilYawMax * percentage;
		rust->mem->Write<float>(recoil_properties + offsets->recoilYawMax, this->c_properties.recoil_properties.recoilYawMax);

		// set recoilYawMin
		this->c_properties.recoil_properties.recoilYawMin = this->o_properties.recoil_properties.recoilYawMin * percentage;
		rust->mem->Write<float>(recoil_properties + offsets->recoilYawMin, this->c_properties.recoil_properties.recoilYawMin);

		// set timeToTakeMax
		this->c_properties.recoil_properties.timeToTakeMax = this->o_properties.recoil_properties.timeToTakeMax * percentage;
		rust->mem->Write<float>(recoil_properties + offsets->timeToTakeMax, this->c_properties.recoil_properties.timeToTakeMax);

		// set timeToTakeMin
		this->c_properties.recoil_properties.timeToTakeMin = this->o_properties.recoil_properties.timeToTakeMin * percentage;
		rust->mem->Write<float>(recoil_properties + offsets->timeToTakeMin, this->c_properties.recoil_properties.timeToTakeMin);

		// update spread
		this->recoil = percentage;
	}

	void SetSpread(float percentage)
	{
		// ensure this is a valid weapon
		if (!this->IsWeapon()) return;

		// set aimCone
		this->c_properties.aimCone = this->o_properties.aimCone * percentage;
		rust->mem->Write<float>(bp + offsets->aimCone, this->c_properties.aimCone);

		// set hipAimCone
		this->c_properties.hipAimCone = this->o_properties.hipAimCone * percentage;
		rust->mem->Write<float>(bp + offsets->hipAimCone, this->c_properties.hipAimCone);

		// set aimConePenaltyMax
		this->c_properties.aimConePenaltyMax = this->o_properties.aimConePenaltyMax * percentage;
		rust->mem->Write<float>(bp + offsets->aimConePenaltyMax, this->c_properties.aimConePenaltyMax);

		// set aimconePenaltyPerShot
		this->c_properties.aimconePenaltyPerShot = this->o_properties.aimconePenaltyPerShot * percentage;
		rust->mem->Write<float>(bp + offsets->aimconePenaltyPerShot, this->c_properties.aimconePenaltyPerShot);

		// set stancePenaltyScale
		this->c_properties.stancePenaltyScale = this->o_properties.stancePenaltyScale * percentage;
		rust->mem->Write<float>(bp + offsets->stancePenaltyScale, this->c_properties.stancePenaltyScale);

		// update spread
		this->spread = percentage;
	}

	void SetAutomatic(bool value)
	{
		// ensure this is a valid weapon
		if (!this->IsWeapon()) return;

		// set automatic
		rust->mem->Write<bool>(bp + offsets->automatic, value);
		this->c_properties.automatic = value;

		// update automatic
		this->automatic = value;
	}

	void SetSway(float percentage)
	{
		// ensure this is a valid weapon
		if (!this->IsWeapon()) return;

		// set automatic
		this->c_properties.aimSway = this->o_properties.aimSway * percentage;
		rust->mem->Write<float>(bp + offsets->aimSway, this->c_properties.aimSway);

		// update automatic
		this->sway = percentage;
	}

	bool IsWeapon()
	{
		if (this->name.find("rifle") != std::string::npos)
		{
			return true;
		}
		if (this->name.find("pistol") != std::string::npos)
		{
			return true;
		}
		if (this->name.find("bow") != std::string::npos)
		{
			return true;
		}
		if (this->name.find("lmg") != std::string::npos)
		{
			return true;
		}
		if (this->name.find("shotgun") != std::string::npos)
		{
			return true;
		}
		if (this->name.find("smg") != std::string::npos)
		{
			return true;
		}
		else
			return false;
	}

	std::string GetAmmoType()
	{
		uint64_t item_def = rust->mem->Read<uint64_t>(this->primary_mag + 0x20);
		uint64_t short_name = rust->mem->Read<uint64_t>(item_def + 0x20);
		std::wstring wide_name = rust->mem->ReadUnicode(short_name + 0x14);

		// if Mono-String isn't empty then return the found characters as a string
		if (!wide_name.empty())
			return std::string(wide_name.begin(), wide_name.end()).c_str();

		// Return nothing if item is nothing 
		return "No Ammo";
	}

public:
	uint64_t	ent;			// the BaseItem address
	uint64_t	bp;				// the BaseProjectile address
	uint64_t recoil_properties;// the RecoilProperties address
	uint64_t primary_mag;		// the PrimaryMagazines address
	std::string name;
	WeaponProperties			// holds the current properties of this weapon
		c_properties;
	WeaponProperties			// holds the original properties of this weapon
		o_properties;
	float spread = 1.f;			// spread percentage
	float recoil = 1.f;			// recoil percentage
	float sway = 1.f;			// sway percentage
	bool automatic;
};

#endif // _PLAYER_HPP