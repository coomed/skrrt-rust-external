#ifndef _GLOBALS_HPP
#define _GLOBALS_HPP

// includes for this header file
#include <list>
#include <mutex>
#include <shared_mutex>
#include <d3d11.h>
#include "memory/memory.hpp"
#include "../drawing/imgui/imgui.h"
#include "utils.hpp"
#include "../classes/ents/player.hpp"
#include "../classes/ents/ore.hpp"

// namespace to interact with global variables
class Globals {
public:
	uint64_t camera = 0;  // camera address

	bool has_launched = false;

	Player local_player;

	// screen size
	Vector2 screen_size = { 2560, 1080 };

	std::map<uint64_t, Player> get_players();
	void set_players(std::map<uint64_t, Player> map);
	void clear_players();
	
	std::map<uint64_t, Ore> get_ores();
	void set_ores(std::map<uint64_t, Ore> map);
	void clear_ores();

private:
	ThreadMap<uint64_t, Player> players;
	ThreadMap<uint64_t, Ore> ores;
};

// globals obj
extern Globals* globals;

// class that holds all cheat settings
class Settings
{
public:
	struct config {
		int menu_bind = 0x2D;
		int latency_update = 8;
		int latency_discovery = 4;
	} config;

	struct aimbot
	{
		std::map<int, bool> enabled;		// if the aimbot is enabled
		int keybind = 0x2;					// aimbot keybind
		bool target_team = false;			// if the aimbot should target the team members too
		std::map<int, int> fov;				// aimbot FOV in pixels

		std::map<int, int> selection_type;	// 0 = FOV Selection || 1 = Distance
		std::map<int, int> bone_selection;	// 0 = head || 1 = body || 2 = cock || 3 = closest to cursor

		std::map<int, bool> anti_recoil;	// if the aimbot should compensate for recoil
		std::map<int, bool> prediction;		// if the aimbot should utilize prediction
		std::map<int, bool> smoothing;		// if aimbot should be smoothed
		std::map<int, int> smoothing_amount;// how much the aimbot should be smoothed
		bool visibleCheck = 0;
	} aimbot;

	struct visuals
	{
		struct players {
			struct visuals
			{
				bool enabled;					// player esp visuals enabled
				bool distance;					// player esp distance enabled
				struct name
				{
					bool enabled;				// player name esp enabled
					ImVec4 color = {			// player name esp color
						1.f, 1.f, 1.f, 1.f		//
					};							//
				} name;
				struct skeleton
				{
					bool enabled;				// player name skeleton enabled
					ImVec4 color = {			// player name skeleton color
						1.f, 1.f, 1.f, 1.f		//
					};							//
				} skeleton;
				bool held_weapon;				// held weapon esp visuals enabled
				struct box
				{
					bool enabled;				// box esp visuals enabled
					int box_type;				// box esp type || 0 = Default || 1 = Corner ||

					ImVec4 outline_color = {	// box outline color
						1.f, 1.f, 1.f, 1.f };	//  

					ImVec4 inline_color = {		// box inline color
						0.9294117647058824f,	// 
						0.2509803921568627f,	//  
						0.2156862745098039f,	// 
						1.f };
				} box;
				bool health_bar;				// health bar esp visuals enabled
				bool hotbar;					// hot bar esp visuals enabled
				bool show_team = true;				    // teammate visuals enabled
				bool show_sleepers;				// sleeper visuals enabled
				int max_distance = 150;
			} visuals;
			struct radar
			{
				bool enabled;
				int scaling = 15;
				ImVec4 enemy_color = {};
				ImVec4 friendly_color = {};
				ImVec4 localplayer_color = {};
			} radar;
		} players;

		struct world {
			bool name;
			bool box;
			bool flags;
			bool distance;

			ImVec4 color = { 0.73f, 0.37f, 0.74f, 1.f };

			int max_distance = 150;

			bool stash_esp;
			bool item_esp;
		} world;

		struct resources {
			bool name;
			bool box;
			bool flags;
			bool distance;

			ImVec4 color = { 1.f, 1.f, 1.f, 1.f };

			int max_distance = 150;

			bool stone_ore;
			bool sulfur_ore;
			bool metal_ore;

			bool hemp;
			bool stash;
		} resources;

		struct ai {
			bool name;
			bool box;
			bool flags;
			bool distance;

			bool scientist;
			bool boar;
			bool stag;

			int max_distance = 150;

			ImVec4 color = { 1.f, 1.f, 1.f, 1.f };
		} ai;

		struct collectibles {
			bool name;
			bool box;
			bool flags;
			bool distance;

			int max_distance = 150;

			ImVec4 color = { 1.f, 1.f, 1.f, 1.f };

			bool hemp;
		} collectibles;

		struct crosshair
		{
			bool enabled = true;
			int size = 5;
			int x_offset;
			int y_offset;
		} crosshair;

		bool font_fading = false;				// if there should be font fading 
	} visuals;

	struct features
	{
		float recoil = 1.f;
		float spread = 1.f;
		float sway = 1.f;

		struct c_fov {
			bool enabled;
			float fov;
		} c_fov;

		bool force_auto = false;
		bool instant_eoka = false;
		bool no_slowdown = false;
		bool shotgun_spread = false;
		bool instant_compound = false;
		bool spiderman = false;
		bool unlock_angles = false;
		bool admin_flags = false;
		bool instant_bow = false;
		bool extended_melee = false;
		bool can_jump = false;
		bool set_time = false;
		float time = 12.f;

		struct fly {
			bool enabled;
			int keybind;
		} fly;
	} features;
};

// settings obj
extern Settings* settings;

#endif // _GLOBALS_HPP