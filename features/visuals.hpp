#ifndef _VISUALS_HPP
#define _VISUALS_HPP

// includes for this header
#include <future>
#include "../utils/globals.hpp"
#include "../utils/utils.hpp"
#include "../drawing/renderer.hpp"
#include "aimbot.hpp"

namespace visuals {
	ImColor GetHealthColor(Player entity)
	{
		const float  multiplier = 2.55; //number we multiply our health by to get our colors(multiply our health by 2.55 to give a number we then use for the color. since 255 is the max of any color for our esp ie. 100 full health * 2.55 = 255 or max color)

		int red, green, blue, alpha;	//colors we will solve for based on our entities health
		int health = entity.health;	//get entities health

		red = 255 - (health * multiplier);	//find red value (no health = max red, full health = no red)
		green = health * multiplier;	//find green value (full health = max green, no health = no green)
		blue = 0;	//no blue on color scale red to green
		alpha = 255;	//max alpha

		ImColor Color(red, green, blue, alpha);	//create color and fill it with values
		return Color;	//return color
	}

	void DrawCornerBox(int X, int Y, int W, int H, ImVec4 inline_color, ImVec4 outline_color)
	{
		float lineW = (W / 5);
		float lineH = (H / 6);
		float lineT = 0.2;

		// outline
		{
			// bottom right
			drawing->line(ImVec2{ X - lineT, Y - lineT }, ImVec2{ X + lineW, Y - lineT }, outline_color, 1.5f, true);
			drawing->line(ImVec2{ X - lineT, Y - lineT }, ImVec2{ X - lineT, Y + lineH }, outline_color, 1.5f, true);

			// top right
			drawing->line(ImVec2{ X - lineT, Y + H - lineH }, ImVec2{ X - lineT, Y + H + lineT }, outline_color, 1.5f, true);
			drawing->line(ImVec2{ X - lineT, Y + H + lineT }, ImVec2{ X + lineW, Y + H + lineT }, outline_color, 1.5f, true);

			// bottom left
			drawing->line(ImVec2{ X + W - lineW, Y - lineT }, ImVec2{ X + W + lineT, Y - lineT }, outline_color, 1.5f, true);
			drawing->line(ImVec2{ X + W + lineT, Y - lineT }, ImVec2{ X + W + lineT, Y + lineH }, outline_color, 1.5f, true);

			// top left
			drawing->line(ImVec2{ X + W + lineT, Y + H - lineH }, ImVec2{ X + W + lineT, Y + H + lineT }, outline_color, 1.5f, true);
			drawing->line(ImVec2{ X + W - lineW, Y + H + lineT }, ImVec2{ X + W + lineT, Y + H + lineT }, outline_color, 1.5f, true);
		}

		// inline
		{
			// top left
			drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y), static_cast<LONG>(X), static_cast<LONG>(Y + lineH) }, inline_color, 1.f);
			drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y), static_cast<LONG>(X + lineW), static_cast<LONG>(Y) }, inline_color, 1.f);

			// top right
			drawing->line({ static_cast<LONG>(X + W - lineW), static_cast<LONG>(Y), static_cast<LONG>(X + W), static_cast<LONG>(Y) }, inline_color, 1.f);
			drawing->line({ static_cast<LONG>(X + W), static_cast<LONG>(Y), static_cast<LONG>(X + W), static_cast<LONG>(Y + lineH) }, inline_color, 1.f);

			// bottom left
			drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y + H - lineH),  static_cast<LONG>(X),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
			drawing->line({ static_cast<LONG>(X), static_cast<LONG>(Y + H), static_cast<LONG>(X + lineW), static_cast<LONG>(Y + H) }, inline_color, 1.f);

			// bottom right
			drawing->line({ static_cast<LONG>(X + W - lineW),  static_cast<LONG>(Y + H),  static_cast<LONG>(X + W),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
			drawing->line({ static_cast<LONG>(X + W),  static_cast<LONG>(Y + H - lineH),  static_cast<LONG>(X + W),  static_cast<LONG>(Y + H) }, inline_color, 1.f);
		}
	}

	// render player esp
	void DrawPlayer(Player player)
	{
		// skip localplayer
		if (player.local_player) return;

		// get distance
		int distance = player.Distance(&globals->local_player);

		// ensure inside max distance
		if (distance > settings->visuals.players.visuals.max_distance) return;

		// skip sleepers if requested
		if (player.HasPlayerFlag(PlayerFlags::Sleeping) && !settings->visuals.players.visuals.show_sleepers) return;

		// bones map
		std::map<Bones, Vector2> screen_bones;

		// update view matrix
		Matrix view_matrix = rust->mem->Read<Matrix>(globals->camera + 0x2E4);

		// iterate over bones
		for (auto& bone : player.bones)
		{
			// get the Vector2 screen bones position
			Vector2 pos = W2S(bone.second, &view_matrix);

			if (pos.x < 0 || pos.x > globals->screen_size.x || pos.y < 0 || pos.y > globals->screen_size.y)
				return;

			screen_bones[bone.first] = pos;
		}

		// get middle of the player's feet
		Vector2 mid_feet = {
			(screen_bones[l_foot].x + screen_bones[r_foot].x) / 2,
			(screen_bones[l_foot].y + screen_bones[r_foot].y) / 2
		};

		// get player height
		int player_height = (screen_bones[head].y - mid_feet.y);

		// get player w
		int player_width = 2.0f * (player_height / 4.0f);

		// get left
		Vector2 screened_left = mid_feet - Vector2(player_width / 2, 0);

		// render player name
		if (settings->visuals.players.visuals.name.enabled)
		{
			// get player name
			std::string player_name = player.GetASCIIName();

			// convert name to uppercase
			std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::toupper);

			// calculate text size
			ImVec2 t_size = ImGui::CalcTextSize(player_name.c_str(), NULL, false, -1.f, 12.f);

			// draw player name
			drawing->text({ mid_feet.x - (t_size.x / 2), (screened_left.y + player_height - 5.f) - (t_size.y / 2) }, player_name, ImVec4(settings->visuals.players.visuals.name.color.x, settings->visuals.players.visuals.name.color.y, settings->visuals.players.visuals.name.color.z, settings->visuals.font_fading ? (1 - (0.015 * distance)) : settings->visuals.players.visuals.name.color.w), true, 12.f);
		}

		// render player skeleton
		if (settings->visuals.players.visuals.skeleton.enabled)
		{
			std::vector<std::pair<int, int>> bone_pairs =
			{
				{r_hand, r_forearm},
				{r_forearm, r_upperarm},
				{r_upperarm, spine4},
				{l_hand, l_forearm},
				{l_forearm, l_upperarm},
				{l_upperarm, spine4},
				{spine4, neck},
				{neck, head},
				{spine4, spine1},
				{spine1, r_hip},
				{spine1, l_hip},
				{r_hip, r_knee},
				{l_hip, l_knee},
				{r_knee, r_foot},
				{l_knee, l_foot},
				{pelvis, penis}
			};

			for (auto& bone_pair : bone_pairs)
			{
				auto bone1_w2s = W2S(player.bones[(Bones)bone_pair.first], &view_matrix);
				auto bone2_w2s = W2S(player.bones[(Bones)bone_pair.second], &view_matrix);

				drawing->line({ bone1_w2s.x, bone1_w2s.y }, { bone2_w2s.x, bone2_w2s.y }, { 1.f, 1.f, 1.f, 1.f }, true);
			}
		}

		int bottom_offset = screened_left.y + 1.f;

		// render player name
		if (settings->visuals.players.visuals.distance)
		{
			// get distance in string format
			std::string distance_str = std::to_string(distance) + "M";

			// calculate text size
			ImVec2 t_size = ImGui::CalcTextSize(distance_str.c_str(), NULL, false, -1.f, 12.f);

			// draw player name
			drawing->text({ mid_feet.x - (t_size.x / 2), bottom_offset + (t_size.y / 2) }, distance_str, ImVec4(0.88, 0.88, 0.88, settings->visuals.font_fading ? (1 - (0.015 * distance)) : settings->visuals.players.visuals.name.color.w), true, 12.f);

			// increment offset
			bottom_offset += t_size.y + 1.f;
		}

		// render player held item
		if (settings->visuals.players.visuals.held_weapon)
		{
			// get held item name
			std::string held_item = player.held_items[player.helditem].first;

			// convert name to uppercase
			std::transform(held_item.begin(), held_item.end(), held_item.begin(), ::toupper);

			// calculate text size
			ImVec2 t_size = ImGui::CalcTextSize(held_item.c_str(), NULL, false, -1.f, 9.f);

			// draw held item
			drawing->text({ (mid_feet.x) - (t_size.x / 2), bottom_offset + (t_size.y / 2) }, held_item, ImVec4(0.88, 0.88, 0.88, settings->visuals.font_fading ? (1 - (0.015 * distance)) : settings->visuals.players.visuals.name.color.w), true, 9.f);

			// increment offset
			bottom_offset += t_size.y + 1.f;
		}

		// render player box
		if (settings->visuals.players.visuals.box.enabled)
		{
			DrawCornerBox(screened_left.x, screened_left.y, player_width, player_height, ImVec4(settings->visuals.players.visuals.box.inline_color.x, settings->visuals.players.visuals.box.inline_color.y, settings->visuals.players.visuals.box.inline_color.z, settings->visuals.font_fading ? (1 - (0.015 * distance)) : settings->visuals.players.visuals.box.inline_color.w), ImVec4(settings->visuals.players.visuals.box.outline_color.x, settings->visuals.players.visuals.box.outline_color.y, settings->visuals.players.visuals.box.outline_color.z, settings->visuals.font_fading ? (1 - (0.015 * distance)) : settings->visuals.players.visuals.box.outline_color.w));
		}

		// render health bar
		if (settings->visuals.players.visuals.health_bar)
		{
			ImVec4 col = GetHealthColor(player).Value;

			// get health color
			drawing->filled_rect(
				ImVec2{ screened_left.x + player_width - 1.f, screened_left.y + 3.f - 1.f },
				ImVec2{ (player_width + 3.f) * -1, 2.5f + 2.f },
				ImVec4(0.f, 0.f, 0.f, settings->visuals.font_fading ? (1 - (0.005 * distance)) : col.w),
				false
			);

			drawing->filled_rect(
				ImVec2{ screened_left.x + player_width, screened_left.y + 3.f },
				ImVec2{ player_width * (player.health / 100.f) * -1 + 1.f, 2.5f },
				ImVec4(col.x, col.y, col.z, settings->visuals.font_fading ? (1 - (0.005 * distance)) : col.w),
				false
			);
		}
	}

	template <typename T>
	void DrawBasic(T entity, std::string name, bool b_distance, bool b_name, bool b_box, bool b_flags, ImVec4* color)
	{
		// update view matrix
		Matrix view_matrix = rust->mem->Read<Matrix>(globals->camera + 0x2E4);

		// w2s ent pos
		Vector2 ent_pos = W2S(entity.position, &view_matrix);

		// get ent name
		std::string ent_name = b_distance ? name + " [" + std::to_string(entity.Distance(&globals->local_player)) + "m]" : name;

		// convert name to uppercase
		std::transform(ent_name.begin(), ent_name.end(), ent_name.begin(), ::toupper);

		// draw ent box
		if (b_box)
			drawing->rect(ImVec2(ent_pos.x - (7.5f / 2), ent_pos.y - 7.5f), ImVec2(7.5f, 7.5f), ImVec4(color->x, color->y, color->z, settings->visuals.font_fading ? (float)((1 - (0.005 * entity.Distance(&globals->local_player)))) : 1.f), 2.f, true, false);

		// draw ent name
		if (b_name)
			drawing->text({ ent_pos.x - (ImGui::CalcTextSize(ent_name.c_str(), NULL).x / 2), ent_pos.y }, ent_name, { color->x, color->y, color->z, settings->visuals.font_fading ? (float)((1 - (0.005 * entity.Distance(&globals->local_player)))) : 1.f }, true);
	}

	// renders ESP
	void Draw()
	{
		// set ImGui window size
		ImGui::SetNextWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));

		// set imgui window pos
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		// push ESP font
		ImGui::PushFont(drawing->fonts.espFont);

		// begin to draw esp
		ImGui::Begin("main", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_None | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
		{
			if (!globals->has_launched) {
				drawing->text({ 10, 10 }, "skrrt | press F2 when loaded into a server", { 1.f, 1.f, 1.f, 1.f }, true);
			}
			else {
				drawing->text({ 10, 10 }, "skrrt", { 1.f, 1.f, 1.f, 1.f }, true);

				if (globals->local_player.ent)
				{
					// get the player's weapon type
					int weapon_type = aimbot->ItemType(globals->local_player.held_items[globals->local_player.helditem].first);

					// get held weapons aimbot fov
					float fov = (settings->aimbot.fov[0] != 0) ? (settings->aimbot.fov[0]) : (settings->aimbot.fov[weapon_type]);

					if (fov > 2.f)
						drawing->circle({ globals->screen_size.x / 2, globals->screen_size.y / 2 }, fov, { 1.f, 1.f, 1.f, 1.f }, 1.5f, true, false);
				}

				if (globals->local_player.ent)
					drawing->text({ 10, 20 }, ("LocalPlayer: " + globals->local_player.GetASCIIName()), { 1.f, 1.f, 1.f, 1.f }, true);

				if (settings->visuals.crosshair.enabled)
					drawing->crosshair(ImVec4(1.f, 1.f, 1.f, 1.f), 1.f, settings->visuals.crosshair.size, settings->visuals.crosshair.x_offset, settings->visuals.crosshair.y_offset);

				std::map<uint64_t, Player> players = globals->get_players();
				std::map<uint64_t, Ore> ores = globals->get_ores();

				for (auto& entry : players) {
					if (!IsValidPointer(entry.first)) {
						continue;
					}

					float health = rust->mem->Read<float>(entry.first + offsets->health);

					if (health < 1 || health > 500) {
						continue;
					}

					DrawPlayer(entry.second);
				}

				for (auto& entry : ores) {
					Ore ore = (Ore)entry.second; 
					if (
						settings->visuals.resources.stone_ore && entry.second.name == "Stone Ore" ||
						settings->visuals.resources.sulfur_ore && entry.second.name == "Sulfur Ore" || 
						settings->visuals.resources.metal_ore && entry.second.name == "Metal Ore" || 
						settings->visuals.resources.stash && entry.second.name == "Stash" ||
						settings->visuals.resources.hemp && entry.second.name == "Hemp"
					) { 
						if (entry.second.Distance(&globals->local_player) < settings->visuals.resources.max_distance) {
							DrawBasic<Ore>(entry.second, entry.second.name, settings->visuals.resources.distance, settings->visuals.resources.name, settings->visuals.resources.box, settings->visuals.resources.flags, &settings->visuals.resources.color);
						}
					}
				}
			}

		} ImGui::End();
	};
}
#endif // _VISUALS_HPP