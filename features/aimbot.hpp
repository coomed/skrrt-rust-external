#ifndef _AIMBOT_HPP
#define _AIMBOT_HPP

// includes for this header
#include "../utils/utils.hpp"
#include "../utils/globals.hpp"
#include "../classes/ents/player.hpp"
#include "../classes/game_object_manager.hpp"
#include "../features/features.hpp"

// aimbot class
class Aimbot
{
public: // public variables for this class

	bool m_TargetExist;
	Vector3 prediction_pos;
	Player pTarget;
	uint64_t camera;

public: // public methods for this class

	// converts degree to a radian
	float to_radian(float degree)
	{
		return degree * 3.141592f / 180.f;
	}

	// converts radian to a degree
	float to_degree(float radian)
	{
		return radian * 180.f / 3.141592f;
	}

	// gets the length of the vector
	inline float Length(Vector3 v) {
		return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	}

	// normalizes a vec2
	void Normalize(float& Yaw, float& Pitch) {
		if (Pitch < -89)
			Pitch = -89;

		else if (Pitch > 89)
			Pitch = 89;

		if (Yaw < -360)
			Yaw += 360;

		else if (Yaw > 360)
			Yaw -= 360;
	}

	// normalizes a vec2
	void Normalize(float& Yaw, float& Pitch, float&z) {
		if (Pitch < -89)
			Pitch = -89;

		else if (Pitch > 89)
			Pitch = 89;

		if (Yaw < -360)
			Yaw += 360;

		else if (Yaw > 360)
			Yaw -= 360;

		if (z < -360)
			z += 360;

		else if (z > 360)
			z -= 360;
	}

	// calculates a vector2's distance to the cursor
	float distance_cursor(Vector2 vec)
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			float ydist = (vec.y - p.y);
			float xdist = (vec.x - p.x);
			float ret = sqrt(pow(ydist, 2) + pow(xdist, 2));
			return ret;
		}
	}

	std::map<std::string, float> BulletSpeeds = {
		/* For 5.56 Fed Weapons */
		{ std::string("ammo.rifle"), 1.0f },
		{ std::string("ammo.rifle.hv"), 1.2f },
		{ std::string("ammo.rifle.explosive"), 0.6f },
		{ std::string("ammo.rifle.incendiary"), 0.6f },

		/* For Pistol Ammunition Fed Weapons */
		{ std::string("ammo.pistol"), 1.0f },
		{ std::string("ammo.pistol.hv"), 1.33333f },
		{ std::string("ammo.pistol.incendiary"), 0.75f },

		/* For Weapons That Use Arrows */
		{ std::string("arrow.wooden"), 1.0f },
		{ std::string("arrow.hv"), 1.6f },
		{ std::string("arrow.fire"), 0.8f },
		{ std::string("arrow.bone"), 0.9f },

		/* For Shotguns */
		{ std::string("ammo.handmade.shell"), 1.0f },
		{ std::string("ammo.shotgun.slug"), 2.25f },
		{ std::string("ammo.shotgun.fire"), 1.0f },
		{ std::string("ammo.shotgun"), 2.25f },

		{ std::string("ammo.nailgun.nails"), 1.0f },

		{ std::string("No Ammo"), 1.0f }
	};
	std::map<std::string, float> BulletGravity = {
		/* For 5.56 Fed Weapons */
		{ std::string("ammo.rifle"), 1.0f },
		{ std::string("ammo.rifle.hv"), 1.0f },
		{ std::string("ammo.rifle.explosive"), 1.25f },
		{ std::string("ammo.rifle.incendiary"), 1.0f },

		/* For Pistol Ammunition Fed Weapons */
		{ std::string("ammo.pistol"), 1.0f },
		{ std::string("ammo.pistol.hv"), 1.0f },
		{ std::string("ammo.pistol.incendiary"), 1.0f },

		/* For Weapons That Use Arrows */
		{ std::string("arrow.wooden"), 0.75f },
		{ std::string("arrow.hv"), 0.5f },
		{ std::string("arrow.fire"), 1.0f },
		{ std::string("arrow.bone"), 0.75f },

		/* For Shotguns */
		{ std::string("ammo.handmade.shell"), 1.0f },
		{ std::string("ammo.shotgun.slug"), 1.0f },
		{ std::string("ammo.shotgun.fire"), 1.0f },
		{ std::string("ammo.shotgun"), 1.0f },

		{ std::string("ammo.nailgun.nails"), 0.75f },
		{ std::string("No Ammo"), 1.f }
	};

	// calculates AimAngles for aimbot
	Vector2 CalcAngle(const Vector3& src, const Vector3& dst)
	{
		Vector3 dir = Vector3(src.x - dst.x, src.y - dst.y, src.z - dst.z);

		float Yaw = to_degree(-atan2(dir.x, -dir.z));
		float Pitch = to_degree(asin(dir.y / Length(dir)));

		return { Pitch, Yaw };
	}

	// calculates bullet drop
	float BulletDrop(Vector3 v1, Vector3 v2, float BulletSpeed, float BulletGravity, float kin_deltatime)
	{
		float Dist = v1.Distance(v2);

		if (Dist < 0.001)
			return 0;

		return (float)(-0.5 * (BulletGravity * (9.81)) * (pow(kin_deltatime, 2)));
	}

	// returns the item name
	std::string GetItemName(uint64_t item)
	{
		/* Read the items information */
		uintptr_t info = rust->mem->Read<uintptr_t>((uintptr_t)item + 0x20);

		/* Read the display name */
		uintptr_t display_name = rust->mem->Read<uintptr_t>(info + 0x20);

		/* Read mono string for the name */
		std::wstring wide_name = rust->mem->ReadUnicode(display_name + 0x14);

		/* If Mono-String isn't empty then return the found characters as a string */
		if (!wide_name.empty())
			return std::string(wide_name.begin(), wide_name.end()).c_str();

		/* Return nothing if item is nothing */
		return "No Item";
	}

	// calculates the projectile speed
	float ProjectileSpeed_Normal(uint64_t weapon, float Ammunition_Multiplier)
	{
		/* Read held item to a string value */
		std::string held_item = GetItemName(weapon);

		/* Return some bogus value if it isn't held just a safety check if other functions don't work */
		if (held_item == "No Item")                 return 300.0f;

		/*
		* Decide weapons velocity based off name (( DEFAULT BULLET TYPE ))
		*/

		/* Normal 556 rifle ammunition */
		if (held_item == "rifle.ak")                return 375.0f * Ammunition_Multiplier;
		if (held_item == "rifle.lr300")             return 375.0f * Ammunition_Multiplier;
		if (held_item == "rifle.bolt")              return 656.0f * Ammunition_Multiplier;
		if (held_item == "rifle.l96")               return 1125.0f * Ammunition_Multiplier;
		if (held_item == "rifle.m39")               return 469.0f * Ammunition_Multiplier;
		if (held_item == "rifle.semiauto")          return 375.0f * Ammunition_Multiplier;

		/* Default 556 ammunition */
		if (held_item == "lmg.m249")                return 488.0f * Ammunition_Multiplier;

		/* Normal pistol bullet ammunition */
		if (held_item == "smg.thompson")            return 300.0f * Ammunition_Multiplier;
		if (held_item == "smg.custom")              return 240.0f * Ammunition_Multiplier;
		if (held_item == "smg.mp5")                 return 240.0f * Ammunition_Multiplier;

		/* Normal pistol bullet ammunition, hand made shell for Eoka  */
		if (held_item == "pistol.python")           return 300.0f * Ammunition_Multiplier;
		if (held_item == "pistol.semi")             return 300.0f * Ammunition_Multiplier;
		if (held_item == "pistol.revolver")         return 300.0f * Ammunition_Multiplier;
		if (held_item == "pistol.m92")              return 300.0f * Ammunition_Multiplier;
		if (held_item == "pistol.eoka")             return 100.0f * Ammunition_Multiplier;
		if (held_item == "pistol.nailgun")          return 50.0f;

		/* Plain arrow */
		if (held_item == "crossbow")               return 75.0f * Ammunition_Multiplier;
		if (held_item == "bow.compound")            return 100.0f * Ammunition_Multiplier;
		if (held_item == "bow.hunting")             return 50.0f * Ammunition_Multiplier;

		/* Hand made shell */
		if (held_item == "shotgun.pump")            return 100.0f * Ammunition_Multiplier;
		if (held_item == "shotgun.spas12")          return 100.0f * Ammunition_Multiplier;
		if (held_item == "shotgun.waterpipe")       return 100.0f * Ammunition_Multiplier;
		if (held_item == "shotgun.doublebarrel")    return 100.0f * Ammunition_Multiplier;

		/* If all fails then return a zero value to void all post calculations */
		return 300.0f;
	}

	// smooths the aimbot
	void SmoothAim(Vector2& Angle, float smooth) {
		Angle.x /= smooth;
		Angle.y /= smooth;
	}

	// gets the held item's type
	int ItemType(std::string name)
	{
		if ((name.find("l96") != std::string::npos) || (name.find("bolt") != std::string::npos))
		{
			return 2;	// snipers
		}
		else if (name.find("rifle") != std::string::npos)
		{
			return 1;	// rifles
		}
		else if (name.find("smg") != std::string::npos)
		{
			return 3;	// smgs
		}
		else if (name.find("pistol") != std::string::npos)
		{
			return 4;	// pistols
		}
		else if (name.find("bow") != std::string::npos)
		{
			return 5;
		}
		else if (name.find("shotgun") != std::string::npos)
		{
			return 6;
		}
		else if (name.find("lmg") != std::string::npos)
		{
			return 7;
		}
		else
			return 0;
	};


	// calculates the prediction 
	Vector3 Prediction(Vector3 playerPos, Vector3 localPos)
	{
		// calculate the distance
		float distance = localPos.Distance(playerPos);

		// set up result with default origin value
		Vector3 result = playerPos;

		if (distance > 0.001f)
		{
			// ensure held weapon has been processed by features thread
			if (features->weapons.find(globals->local_player.held_items[globals->local_player.helditem].second) == features->weapons.end()) return result;

			HeldItem item = features->weapons[globals->local_player.held_items[globals->local_player.helditem].second];

			// get the held item's ammo type
			std::string ammo_type = item.GetAmmoType();

			// calculate bullet speed of the held item
			float bullet_speed = this->ProjectileSpeed_Normal(item.ent, this->BulletSpeeds[ammo_type]);

			// get the gravity modifier of the bullet
			float this_gmod = this->BulletGravity[ammo_type];

			// calculates the time to for the bullet to hit ( distance / speed )
			float time_to_hit = distance / bullet_speed;

			// gets the velocity of the player
			Vector3 vel = pTarget.GetVelocity();

			// trig
			double angleRadians = (3.141593658979 / 180.0) * (180 - 90);

			// predict player velocity
			result += Vector3((pTarget.GetVelocity().x * 0.75) * time_to_hit, 0, (pTarget.GetVelocity().z * 0.75) * time_to_hit);;

			// calculate bullet drop
			result.y -= BulletDrop(localPos, playerPos, bullet_speed, this_gmod, time_to_hit);
		}

		// return the result
		return result;
	}

	void DoAimbot(int weapon_type)
	{
		// check and make sure player is valid 
		pCheck(pTarget.ent);

		// ensure the target is still networked
			//bool net = static_cast<bool>(rust->mem->Read<uint64_t>(pTarget.ent + 0x50));
			//if (!net)
				//return;

		// return if aimbot isn't enabled for this weapon
		if (!settings->aimbot.enabled[0]) return;

		if (settings->aimbot.visibleCheck == 1) {
			uintptr_t playerModel = rust->mem->Read<uintptr_t>(pTarget.ent + offsets->playerModel);
			if (IsValidPointer(playerModel)) {
				bool visible = rust->mem->Read<bool>(playerModel + offsets->visible);

				if (visible != 1) return;
			}
		}

		// get the camera's position
		Vector3 cam_pos = rust->mem->ReadChain<Vector3>(globals->camera, { 0x30, 0x30, 0x8, 0x38, 0x90 });

		// holds the aimbot's target bone pos
		Vector3 target_bone;

		// switch the bone selection type
		switch (settings->aimbot.bone_selection[0])
		{
		case 0:	// head bone
			target_bone = pTarget.bones[head];
			break;
		case 1: // body bone
			target_bone = pTarget.bones[spine2];
			break;
		case 2: // cock bone
			target_bone = pTarget.bones[penis];
			break;
		case 3: // closest to cursor
			// an bone list we can iterate over
			int targetBones[] = { head, spine2, penis };
			float best_pos = 1000.f;

			// update view matrix
			Matrix view_matrix = rust->mem->Read<Matrix>(globals->camera + 0x2E4);

			// iterate over bones
			for (auto& bone : targetBones)
			{
				// bone position
				Vector3 pos = pTarget.bones[(Bones)bone];

				// w2s bone
				Vector2 w2s_pos = W2S(pos, &view_matrix);

				// get the distance of the bone's w2s to the cursor
				float dist_to_cursor = distance_cursor(w2s_pos);

				// continue if this bone is further to the cursor than the previous bone
				if (dist_to_cursor > best_pos) continue;

				// set best_pos
				best_pos = dist_to_cursor;

				// set target bone position
				target_bone = pos;
			}

			break;
		}

		// set prediction_pos
		this->prediction_pos = target_bone;

		// if prediction is enabled for this weapon, calculate in for prediction
		if ((settings->aimbot.prediction[0]) ? (settings->aimbot.prediction[0]) : (settings->aimbot.prediction[weapon_type]))
		{
			target_bone = this->Prediction(target_bone, cam_pos);
		}

		// holds the localplayer's view angles
		Vector3 view_angles = globals->local_player.GetViewAngles();

		// calculate aimbot angle
		Vector2 v2_aim = this->CalcAngle(cam_pos, target_bone);

		// subtract view angles
		v2_aim -= Vector2{ view_angles.x, view_angles.y };

		// if recoil compensation is enabled for this weapon, compensate
		if (settings->aimbot.anti_recoil[0])
		{
			// get the recoil angles for this weapon
			Vector3 recoil_angles = globals->local_player.GetRecoilAngles();

			// compensate recoil
			v2_aim -= Vector2{ recoil_angles.x, recoil_angles.y };
		}

		
		// if smoothing is enabled for this weapon, smooth
		if (settings->aimbot.smoothing[0])
		{
			// smooth aimbot
			this->SmoothAim(v2_aim, (settings->aimbot.smoothing_amount[0]));
		}
		else
		{
			// smooth aimbot
			this->SmoothAim(v2_aim, 1);
		}
		
		// normalize aim angle
		this->Normalize(v2_aim.y, v2_aim.x);

		// add view angles back
		v2_aim += Vector2{ view_angles.x, view_angles.y };

		// ensure the target isnt sleeping
			//if (globals->local_player.HasPlayerFlag(PlayerFlags::Sleeping)) return;

		// set the player's view angles
		globals->local_player.SetViewAngles({ v2_aim.x, v2_aim.y, 0 });
	}

	void FindTarget(std::map<uint64_t, Player> players)
	{
		pCheck(globals->local_player.ent);

		// get the local player's weapon type
		int weapon_type = this->ItemType(globals->local_player.held_items[globals->local_player.helditem].first);

		if (settings->aimbot.enabled[0] && GetAsyncKeyState(settings->aimbot.keybind))
		{
			// holds the closest fov
			float closest_fov = settings->aimbot.fov[0];

			// holds the closest distance
			float closest_distance = settings->visuals.players.visuals.max_distance;

			// iterate over player list
			int iterations = 0;

			for (auto& entry : players)
			{
				Player ply = (Player)entry.second;

				if (!ply.Update()) continue;

				// ensure it's not the localplayer
				if (ply.local_player) continue;

				int pHealth = ply.health;

				if (pHealth < 1 || pHealth > 500) continue;

				// sleep every 10 iterations
					//if (iterations % 10 == 0) Delay(1);

				// increase iterations
					//iterations++;

				// ensure the target isnt sleeping
					//if (ply.HasPlayerFlag(PlayerFlags::Sleeping) && !settings->visuals.players.visuals.show_sleepers) continue;

				// get distance to local player
				float distance = ply.Distance(&globals->local_player);

				// update view matrix
				Matrix view_matrix = rust->mem->Read<Matrix>(globals->camera + 0x2E4);

				// w2s player's spine bone
				Vector2 w2s_pos = W2S(ply.bones[(Bones)46], &view_matrix);

				// ensure distance
				if (distance > settings->visuals.players.visuals.max_distance) continue;

				// grab player's distance 2 cursor
				float distance_to_cursor = distance_cursor(w2s_pos);

				// ensure player is in fov
				if (distance_to_cursor < closest_fov)
				{
					// set target
					closest_fov = distance_to_cursor;
					pTarget = ply;
					m_TargetExist = true;
				}
			}

			// if the target exists, proceed with the aimbot
			if (m_TargetExist)
			{
				DoAimbot(weapon_type);
			}
			else
			{
				pTarget = {};

				return;
			}
		}
		else {
			m_TargetExist = false;
			pTarget = {};

			return;
		}
	}
};

extern Aimbot* aimbot;

#endif // _AIMBOT_HPP