#include "controls.hpp"
#include <iostream>
#include <sstream>
#include <iostream>
#include <windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_internal.h"
#include <inttypes.h>
#include "..//renderer.hpp"
#include "controls/c_checkbox.hpp"
#include "controls/c_sliderint.hpp"
#include "controls/c_sliderfloat.hpp"
#include "controls/c_dropdown.hpp"
#include "controls/c_keybind.hpp"
#include "controls/c_spacer.hpp"
#include "controls/c_multidropdown.hpp"
#include "controls/c_button.hpp"
#include "controls/c_textfield.hpp"
#include "controls/c_textseperator.hpp"
#include "fonts.hpp"
#include <shlobj.h>
#include <shlwapi.h>
#include <fstream>
#include <filesystem>
#include "../../utils/globals.hpp"
#include "../../skCrypt.h"

namespace fs = std::filesystem;

extern ImFont* keybindsFont{};
extern ImFont* menuFont{};
extern ImFont* tabFont2;
extern ImFont* tabFont3;
extern ImFont* controlFont{};

bool state = false;

// Menu::Init implementation
void Menu::Init()
{
	ImGuiIO& io = ImGui::GetIO();

	drawing->fonts.menuFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf", 14.f);
	drawing->fonts.tabFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(fonts::guns_compressed_data_base85, 24.f);
	drawing->fonts.espFont = io.Fonts->AddFontFromMemoryTTF(fonts::esp_compressed_data, fonts::esp_compressed_size, 12.f);

	mtv->tmenu.area = rect_t(200, 200, 369, 340);

	const auto rage_tab = new C_Tab(this, 0, "e", std::string(skCrypt("aim")), { {"aimbot", 0, 1, false} }); {
		mtv->tmenu.tab_sub = 0;

		const auto legit_aimbot = new C_GroupBox(GROUP_LEFT, 8, "t"); {
			new C_CheckBox(std::string(skCrypt("aimbot enabled")), &settings->aimbot.enabled[0]);

			const auto fov_slider = new C_SliderInt(std::string(skCrypt("field of view")), &settings->aimbot.fov[0], 0, 180, "°", {});
			new C_Spacer(fov_slider, { 0, 10 });

			const auto vis = new C_CheckBox(std::string(skCrypt("visibility check")), &settings->aimbot.visibleCheck);
			new C_Spacer(vis, { 0, 15 });

			const auto bone_selection = new C_Dropdown(std::string(skCrypt("bone selection")), &settings->aimbot.bone_selection[0], { "head", "body", "cock", "closest to cursor" });
			new C_Spacer(bone_selection, { 0, 15 });
		}

		const auto legit_antiaim = new C_GroupBox(GROUP_RIGHT, 8, "t"); {
			new C_CheckBox(std::string(skCrypt("smoothing")), &settings->aimbot.smoothing[0]);

			const auto smoothing_slider = new C_SliderInt(std::string(skCrypt("smoothing amount")), &settings->aimbot.smoothing_amount[0], 0, 4, "°");
			new C_Spacer(smoothing_slider, { 0, 5 });

			const auto anti_recoil = new C_CheckBox(std::string(skCrypt("anti-recoil")), &settings->aimbot.anti_recoil[0]);
			new C_Spacer(anti_recoil, { 0, 15 });

			const auto prediction_toggle = new C_CheckBox(std::string(skCrypt("prediction")), &settings->aimbot.prediction[0]);
			new C_Spacer(prediction_toggle, { 0, 20 });

			const auto target_team = new C_CheckBox(std::string(skCrypt("target-team")), &settings->aimbot.target_team);
			new C_Spacer(target_team, { 0, 25 });

			const auto aimbot_keybind = new C_KeyBind(std::string(skCrypt("aimbot keybind")), &settings->aimbot.keybind);
			new C_Spacer(aimbot_keybind, { 0, 45 });
		}

	}
	const auto esp_tab = new C_Tab(this, 1, "e", "esp", { {"players", 0, 1, false}, {"resources + misc", 1, 1, false} }); {
		// players
		{
			mtv->tmenu.tab_sub = 0;

			const auto g_general = new C_GroupBox(GROUP_LEFT, 8, "t"); {
				const auto b_name = new C_CheckBox(std::string(skCrypt("player name")), &settings->visuals.players.visuals.name.enabled);
				const auto b_box = new C_CheckBox(std::string(skCrypt("player box")), &settings->visuals.players.visuals.box.enabled);
				const auto b_health = new C_CheckBox(std::string(skCrypt("player health bar")), &settings->visuals.players.visuals.health_bar);
				const auto b_distance = new C_CheckBox(std::string(skCrypt("player distance")), &settings->visuals.players.visuals.distance);
				const auto b_heldwep = new C_CheckBox(std::string(skCrypt("player active item")), &settings->visuals.players.visuals.held_weapon);
				const auto b_skeleton = new C_CheckBox(std::string(skCrypt("player skeleton")), &settings->visuals.players.visuals.skeleton.enabled);
			}

			const auto g_sleepers = new C_GroupBox(GROUP_RIGHT, 8, "t"); {
				const auto show_sleepers = new C_CheckBox(std::string(skCrypt("sleepers enabled")), &settings->visuals.players.visuals.show_sleepers);
				const auto show_ai = new C_CheckBox(std::string(skCrypt("ai enabled")), &settings->visuals.ai.scientist);
				const auto show_team = new C_CheckBox(std::string(skCrypt("display team")), &settings->visuals.players.visuals.show_team);

				const auto max_distance = new C_SliderInt(std::string(skCrypt("max distance")), &settings->visuals.players.visuals.max_distance, 30, 300, "m");
				new C_Spacer(max_distance, { 0, 30 });
			}
		}
		// resources + misc
		{
			mtv->tmenu.tab_sub = 1;

			const auto g_general = new C_GroupBox(GROUP_LEFT, 8, "t"); {
				const auto b_name = new C_CheckBox(std::string(skCrypt("name")), &settings->visuals.resources.name);
				const auto b_box = new C_CheckBox(std::string(skCrypt("box")), &settings->visuals.resources.box);
				const auto b_distance = new C_CheckBox(std::string(skCrypt("distance")), &settings->visuals.resources.distance);
			}
			const auto g_enabled = new C_GroupBox(GROUP_RIGHT, 8, "t"); {
				const auto b_stone = new C_CheckBox(std::string(skCrypt("stone ore")), &settings->visuals.resources.stone_ore);
				const auto b_metal = new C_CheckBox(std::string(skCrypt("metal ore")), &settings->visuals.resources.metal_ore);
				const auto b_sulfur = new C_CheckBox(std::string(skCrypt("sulfur ore")), &settings->visuals.resources.sulfur_ore);

				const auto b_stash = new C_CheckBox(std::string(skCrypt("stashes")), &settings->visuals.resources.stash);
				const auto b_hemp = new C_CheckBox(std::string(skCrypt("hemp")), &settings->visuals.resources.hemp);

				const auto max_distance = new C_SliderInt(std::string(skCrypt("max distance")), &settings->visuals.resources.max_distance, 30, 300, "m");
				new C_Spacer(max_distance, { 0, 30 });
			}
		}
	}
	const auto features_tab = new C_Tab(this, 2, "e", "misc", { {"page 1", 0, 1, false}, {"page 2", 1, 1, false},  {"page 3", 2, 1, false} }); {
		// page 1
		{
			mtv->tmenu.tab_sub = 0;

			const auto esp_group = new C_GroupBox(GROUP_LEFT, 8, "t"); {
				const auto f_recoil = new C_SliderFloat(std::string(skCrypt("weapon recoil percentage")), &settings->features.recoil, 0.f, 1.f, "%");
				new C_Spacer(f_recoil, { 0, 5 });

				const auto f_spread = new C_SliderFloat(std::string(skCrypt("weapon spread percentage")), &settings->features.spread, 0.f, 1.f, "%");
				new C_Spacer(f_spread, { 0, 15 });

				const auto f_sway = new C_SliderFloat(std::string(skCrypt("sway percentage")), &settings->features.sway, 0.f, 1.f, "%");
				new C_Spacer(f_sway, { 0, 25 });

				//const auto f_thickness = new C_SliderFloat("bullet thickness", &settings->features.thick_bullet, 0.f, 2.f, "P");
				//new C_Spacer(f_thickness, { 0, 35 });
			}

			const auto radar_group = new C_GroupBox(GROUP_RIGHT, 8, "t"); {
				const auto b_automatic = new C_CheckBox(std::string(skCrypt("force automatic")), &settings->features.force_auto);
				new C_Spacer(b_automatic, { 0, 5 });

				const auto b_thickbullet = new C_CheckBox(std::string(skCrypt("instant eoka")), &settings->features.instant_eoka);
				new C_Spacer(b_thickbullet, { 0, 6 });

				const auto b_instantbow = new C_CheckBox(std::string(skCrypt("instant bow")), &settings->features.instant_bow);
				new C_Spacer(b_instantbow, { 0, 7 });

				const auto b_extendedmelee = new C_CheckBox(std::string(skCrypt("extended melee")), &settings->features.extended_melee);
				new C_Spacer(b_extendedmelee, { 0, 8 });

				//const auto b_blocksprint = new C_CheckBox("no slowdown", &settings->features.no_slowdown);
				//new C_Spacer(b_blocksprint, { 0, 9 });

				const auto b_compound = new C_CheckBox(std::string(skCrypt("instant compound bow")), &settings->features.instant_compound);
				new C_Spacer(b_compound, { 0, 10 });
			}
		}
		// page 2
		{
			mtv->tmenu.tab_sub = 1;

			const auto esp_group = new C_GroupBox(GROUP_LEFT, 8, "t"); {
				new C_CheckBox(std::string(skCrypt("fly")), &settings->features.fly.enabled);

				const auto f_keybind = new C_KeyBind(std::string(skCrypt("fly keybind")), &settings->features.fly.keybind);
				new C_Spacer(f_keybind, { 0, 10 });
			}

			const auto radar_group = new C_GroupBox(GROUP_RIGHT, 8, "t"); {
				const auto b_cjump = new C_CheckBox(std::string(skCrypt("infinite jump")), &settings->features.can_jump);
				new C_Spacer(b_cjump, { 0, 6 });

				const auto b_spiderman = new C_CheckBox("spiderman", &settings->features.spiderman);
				new C_Spacer(b_spiderman, { 0, 7 });

				//const auto b_unlockangles = new C_CheckBox("unlock angles", &settings->features.unlock_angles);
				//new C_Spacer(b_unlockangles, { 0, 8 });

				const auto b_adminflags = new C_CheckBox(std::string(skCrypt("admin flags")), &settings->features.admin_flags);
				new C_Spacer(b_adminflags, { 0, 8 });

				const auto b_settime = new C_CheckBox(std::string(skCrypt("set time")), &settings->features.set_time);
				new C_Spacer(b_settime, { 0, 8 });
				const auto tod = new C_SliderFloat(std::string(skCrypt("time")), &settings->features.time, 0, 24, "h");
				new C_Spacer(tod, { 0, 8 });
			}
		}
		// page 3
		{
			mtv->tmenu.tab_sub = 2;

			const auto esp_group = new C_GroupBox(GROUP_LEFT, 8, "t"); {
				const auto crosshair_enabled = new C_CheckBox("crosshair", &settings->visuals.crosshair.enabled);
				new C_Spacer(crosshair_enabled, { 0, 5 });

				const auto crosshair_size = new C_SliderInt("crosshair size", &settings->visuals.crosshair.size, 0, 10, "°");
				new C_Spacer(crosshair_size, { 0, 10 });
			}

			const auto radar_group = new C_GroupBox(GROUP_RIGHT, 8, "t"); {
				const auto update_latency = new C_SliderInt("update latency", &settings->config.latency_update, 1, 15, "c");
				new C_Spacer(update_latency, { 0, 5 });

				const auto discovery_latency = new C_SliderInt("discovery latency", &settings->config.latency_discovery, 1, 15, "c");
				new C_Spacer(discovery_latency, { 0, 15 });
			}
		}
	}
}

// Menu::Render implementation
void Menu::Render()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(6, 6);

	// set imgui window size
	ImGui::SetNextWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));

	// set imgui window pos
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::PushFont(drawing->fonts.menuFont);
	ImGui::Begin("skrrt rust client", &isOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground); {

		auto outline = [&](int offset, ImColor color) {
			rect_t m_pos = rect_t(
				mtv->tmenu.area.x - 75, mtv->tmenu.area.y, mtv->tmenu.area.w + 75, mtv->tmenu.area.h
			);

			auto box = [](int x, int y, int w, int h, ImColor c) {
				drawing->line(ImVec2(x, y), ImVec2(x, y + h), c, 1.f);
				drawing->line(ImVec2(x, y + h), ImVec2(x + w + 1, y + h), c, 1.f);
				drawing->line(ImVec2(x + w, y), ImVec2(x + w, y + h), c, 1.f);
				drawing->line(ImVec2(x, y), ImVec2(x + w, y), c, 1.f);
			};

			box(m_pos.x - offset, m_pos.y - offset, m_pos.w + offset * 2, m_pos.h + offset * 2, color);
		};

		auto top_bar = [this]() -> void {
			rect_t top_area = rect_t(
				mtv->tmenu.area.x - 75,
				mtv->tmenu.area.y - 27,
				mtv->tmenu.area.w + 75,
				20
			);

			auto box = [](int x, int y, int w, int h, ImColor c) {
				drawing->line(ImVec2(x, y), ImVec2(x, y + h), c, 1.f);
				drawing->line(ImVec2(x, y + h), ImVec2(x + w + 1, y + h), c, 1.f);
				drawing->line(ImVec2(x + w, y), ImVec2(x + w, y + h), c, 1.f);
				drawing->line(ImVec2(x, y), ImVec2(x + w, y), c, 1.f);
			};

			drawing->filled_rect(ImVec2(top_area.x, top_area.y), ImVec2(top_area.w, top_area.h), { 0.1372549019607843f, 0.1372549019607843f, 0.1372549019607843f, 1.f }, false);

			box(top_area.x - 0, top_area.y - 0, top_area.w + 0 * 2, top_area.h + 0 * 2, { 0.f, 0.f, 0.f, 1.f });
			box(top_area.x - 1, top_area.y - 1, top_area.w + 1 * 2, top_area.h + 1 * 2, { 0.1882352941176471f, 0.1882352941176471f, 0.1882352941176471f, 1.f });
			box(top_area.x - 2, top_area.y - 2, top_area.w + 2 * 2, top_area.h + 2 * 2, { 0.f, 0.f, 0.f, 1.f });

			auto w = ImGui::CalcTextSize("skrrt: ", NULL);
			auto w2 = ImGui::CalcTextSize(__DATE__, NULL);

			drawing->text({ (float)(top_area.x + 8), (float)(top_area.y + 11) - (w.y * 0.5f) }, "skrrt: ", { 0.8235294117647059f, 0.8235294117647059f, 0.8235294117647059f, 1.f }, true);
			drawing->text({ top_area.x + 8 + w.x, (float)(top_area.y + 11) - (w2.y * 0.5f) }, __DATE__, { 0.8235294117647059f, 0.8235294117647059f, 0.8235294117647059f, 1.f }, true);
		};

		rect_t main_tab_area = rect_t(
			mtv->tmenu.area.x - 75, mtv->tmenu.area.y,
			75, mtv->tmenu.area.h
		);

		drawing->filled_rect(ImVec2(mtv->tmenu.area.x, mtv->tmenu.area.y), ImVec2(mtv->tmenu.area.w, mtv->tmenu.area.h), { 0.1372549019607843f, 0.1372549019607843f, 0.1372549019607843f, 1.f }, false);

		drawing->filled_rect(ImVec2(main_tab_area.x, main_tab_area.y), ImVec2(main_tab_area.w, main_tab_area.h), { 0.1176470588235294f, 0.1176470588235294f, 0.1176470588235294f, 1.f }, false);

		outline(0, { 0.f, 0.f, 0.f, 1.f });
		outline(1, { 0.1882352941176471f, 0.1882352941176471f, 0.1882352941176471f, 1.f });
		outline(2, { 0.f, 0.f, 0.f, 1.f });

		for (int i = 0; i < tabs.size(); i++) {
			tabs[i]->paint();
			if (tabs[i]->index == mtv->tmenu.selected_tab) {
				for (int c = tabs[i]->group_boxes.size() - 1; c >= 0; c--) {
					if (tabs[i]->group_boxes[c]->get_parent_sub() == tabs[i]->selected_sub)
						tabs[i]->group_boxes[c]->update(mtv->tmenu.area);
				}
			}
		}

		// function
		top_bar();
		ImGui::PopFont();

	} ImGui::End();
}

// Menu::Update implementation
void Menu::Update()
{
	double gradient[3] = {
		mtv->tmenu.menu_theme[0].x - 0.0784313725490196,
		mtv->tmenu.menu_theme[0].y - 0.0784313725490196,
		mtv->tmenu.menu_theme[0].z - 0.0784313725490196
	};

	for (int i = 0; i < 3; i++) {
		if (gradient[i] < 42) {
			switch (i)
			{
			case 1:
				gradient[i] = mtv->tmenu.menu_theme[0].x + 0.0784313725490196;
				break;
			case 2:
				gradient[i] = mtv->tmenu.menu_theme[0].y + 0.0784313725490196;
				break;
			case 3:
				gradient[i] = mtv->tmenu.menu_theme[0].z + 0.0784313725490196;
				break;
			}
		}
	}

	mtv->tmenu.menu_theme[1] = ImVec4(gradient[0], gradient[1], gradient[2], 1.f);

	Render();

	// update our mouse and window position
	poll_mouse();

	for (int i = 0; i < tabs.size(); i++) {
		tabs[i]->update();
		for (int c = tabs[i]->group_boxes.size() - 1; c >= 0; c--) {
			tabs[i]->group_boxes[c]->update(mtv->tmenu.area);
		}
	}

	// tab clicks
	rect_t main_tab_area = rect_t(
		mtv->tmenu.area.x - 95, mtv->tmenu.area.y, 95, mtv->tmenu.area.h
	);

	if (mtv->tmenu.animating) {
		if (mtv->tmenu.animate_direction == TAB_ANIMATION_DOWN) {
			if ((main_tab_area.y + mtv->tmenu.animation_offset) < mtv->tmenu.animation_destination) {
				mtv->tmenu.animation_offset += 2;
			}
			else {
				mtv->tmenu.animating = false;
			}
		}
		else {
			if ((main_tab_area.y + mtv->tmenu.animation_offset) > mtv->tmenu.animation_destination) {
				mtv->tmenu.animation_offset -= 2;
			}
			else {
				mtv->tmenu.animating = false;
			}
		}
	}

}


void Menu::poll_mouse() {
	rect_t top_area = rect_t(
		mtv->tmenu.area.x - 75,
		mtv->tmenu.area.y - 27,
		mtv->tmenu.area.w + 75,
		20
	);

	if (dragging && !GetAsyncKeyState(VK_LBUTTON)) {
		dragging = false;
	}

	if (GetAsyncKeyState(VK_LBUTTON) && top_area.contains_point(cursor)) {
		dragging = true;
	}

	if (dragging) {
		drag_x = cursor.x - mtv->tmenu.area.x;
		drag_y = cursor.y - mtv->tmenu.area.y;

		GetCursorPos(&cursor);

		mtv->tmenu.area.x = cursor.x - drag_x;
		mtv->tmenu.area.y = cursor.y - drag_y;
	}
	else {
		GetCursorPos(&cursor);
	}
}

Menu* menu = new Menu();
MTV* mtv = new MTV();
Globals* globals = new Globals();
Settings* settings = new Settings();