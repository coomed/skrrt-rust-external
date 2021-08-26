#ifndef _MENU_HPP
#define _MENU_HPP

// includes for this header
#include <vector>
#include <string>
#include <functional>
#include "../imgui/imgui.h"
#include "../../utils/utils.hpp"
#include "../renderer.hpp"
#include "../../utils/globals.hpp"

// Groupbox & Tab Definitions
class C_GroupBox;
class C_Tab;

// rect class for bounding
struct rect_t {
	int x, y, w, h;

	rect_t() { }

	rect_t(int n_x, int n_y, int n_w, int n_h) {
		x = n_x;
		y = n_y;
		w = n_w;
		h = n_h;
	}

	bool contains_point(POINT pnt) {
		return
			pnt.x > x &&
			pnt.y > y &&
			pnt.x < x + w &&
			pnt.y < y + h;
	}
};

// flags for each control
enum ControlFlagTypes {
	CONTROLFL_KEYBIND,
	CONTROLFL_COLORSELECTOR,
	AIMBOT_TOGGLES
};
// control flag class
class C_ControlFlag {
public:
	virtual void draw() = 0;
	virtual void update() = 0;
	virtual void click() = 0;

	C_GroupBox* parent;

	rect_t area;
};

// flags for each animation
enum {
	TAB_ANIMATION_UP,
	TAB_ANIMATION_DOWN
};

// menu class
class Menu
{
public:
	void Init();				// initiates the menu
	void Update();				// updates shit nigga
	void poll_mouse();			// draggin shit
	void Render();				// renders the menu

	static Menu& get() {
		static Menu instance;
		return instance;
	}

	POINT cursor{};
public:
	bool
		isOpen = false,
		mouse_enable,
		dragging;

	void toggle() {
		isOpen = !isOpen;
	}

	bool active() {
		return isOpen;
	}

	int
		drag_x,
		drag_y;

	std::vector< C_Tab* > tabs; // tabs vector

	// inserts a tab into the tabs vector
	void add_tab(C_Tab* tab) {
		tabs.push_back(tab);
	}
};

// contains global menu variables ( GAY )
class MTV
{
public:
	struct Menu_t {
		C_GroupBox* last_group;

		ImVec4 menu_theme[2] = { ImVec4(0.8509803921568627f, 0.3254901960784314f, 0.3058823529411765f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f) };

		C_Tab* last_tab;
		int group_sub;
		int tab_sub;

		std::vector< C_GroupBox* > group_boxes; // group boxs vector
		std::vector< C_Tab* > tabs; // tabs vector

		rect_t area;
		rect_t m_pos = rect_t(
			area.x - 74,
			area.y,
			area.w + 74, 1
		);
		bool
			animating,
			animate_direction;

		float
			animation_offset,
			animation_destination;

		int selected_tab = 0;

		std::vector< std::string > configs;
		std::string new_config_name;
		int selected_config = 0;
	}tmenu;
};
extern MTV* mtv;

// control class
class C_Control {
public:
	virtual void draw() = 0;
	virtual void update() = 0;
	virtual void click() = 0;

	std::vector< C_ControlFlag* > flags;

	C_GroupBox* parent;

	rect_t area;
	ImVec2 offset = {};
	int height_offset = 0;
	int sub = -1;

	bool* override_function;
	bool last_override_state;

	void set_override(bool* n_override);
};

// tab class
class C_Tab {
public:
	std::string name, icon;
	int index;
	std::vector< C_GroupBox* > group_boxes;
	Menu* parent;

	C_Control* overriding_control;
	C_ControlFlag* overriding_flag;
	bool block_input, reset_inputblock, blocked_control_flag;

	struct subtab_t {
		const char* name;
		int index;
		int font;
		bool chuj;
		float selected_opacity = 0.f;
	};

	enum {
		SUBTAB_ANIMATION_LEFT,
		SUBTAB_ANIMATION_RIGHT
	};

	bool animating;
	int from_anim;

	int selected_sub;
	std::vector< subtab_t > subtabs;

	void paint() {
		rect_t tab_area = rect_t(
			mtv->tmenu.area.x - 75,
			mtv->tmenu.area.y + index * 20,
			75, 20
		);
		rect_t tab_selected = rect_t(
			mtv->tmenu.area.x - 75,
			mtv->tmenu.area.y + (mtv->tmenu.animating ? mtv->tmenu.animation_offset : (index * 20)),
			75, 20
		);

		rect_t tab_bar = rect_t(
			mtv->tmenu.area.x - 10,
			mtv->tmenu.area.y + mtv->tmenu.animation_offset,
			10, 20
		);

		rect_t top2bottom = rect_t(
			mtv->tmenu.area.x,
			mtv->tmenu.area.y,
			1,
			mtv->tmenu.area.h
		);

		ImVec2 tsize = ImGui::CalcTextSize(name.c_str(), NULL);

		drawing->text(ImVec2(tab_area.x + 8, (tab_area.y + 10) - (tsize.y / 2)), name, ImVec4(0.8235294117647059f, 0.8235294117647059f, 0.8235294117647059f, 1.f), true);

		if (index == mtv->tmenu.selected_tab) {
			drawing->rect(ImVec2(top2bottom.x, top2bottom.y), ImVec2(top2bottom.w, top2bottom.h), ImVec4(0.1882352941176471f, 0.1882352941176471f, 0.1882352941176471f, 1.f), 1.f, false, false);
			drawing->rect(ImVec2(top2bottom.x - 1, top2bottom.y), ImVec2(top2bottom.w, top2bottom.h), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

			drawing->filled_rect(ImVec2(tab_selected.x, tab_selected.y), ImVec2(tab_selected.w + 2, tab_selected.h), ImVec4(0.1372549019607843f, 0.1372549019607843f, 0.1372549019607843f, 1.f), false);
			drawing->text(ImVec2(tab_area.x + 8, (tab_area.y + 10) - (tsize.y / 2)), name, ImVec4(mtv->tmenu.menu_theme[0].x, mtv->tmenu.menu_theme[0].y, mtv->tmenu.menu_theme[0].z, 1.f), true);

			/* black lines */
			drawing->line(ImVec2(tab_selected.x, tab_selected.y - 1), ImVec2(tab_selected.x + tab_selected.w, tab_selected.y - 1), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f);
			drawing->line(ImVec2(tab_selected.x, tab_selected.y + tab_selected.h), ImVec2(tab_selected.x + tab_selected.w, tab_selected.y + tab_selected.h), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f);

			/* grey lines */
			drawing->line(ImVec2(tab_selected.x, tab_selected.y), ImVec2(tab_selected.x + tab_selected.w + 1, tab_selected.y), ImVec4(0.1882352941176471f, 0.1882352941176471f, 0.1882352941176471f, 1.f), 1.f);
			drawing->line(ImVec2(tab_selected.x, tab_selected.y + tab_selected.h - 1), ImVec2(tab_selected.x + tab_selected.w + 1, tab_selected.y + tab_selected.h - 1), ImVec4(0.1882352941176471f, 0.1882352941176471f, 0.1882352941176471f, 1.f), 1.f);

			paint_subtabs();
		}
	}
	void update()
	{
		rect_t main_tab_area = rect_t(
			mtv->tmenu.area.x - 75, mtv->tmenu.area.y,
			75, mtv->tmenu.area.h
		);

		rect_t tab_area = rect_t(
			main_tab_area.x,
			main_tab_area.y + (index * 20),
			75, 20
		);

		POINT cursor;
		GetCursorPos(&cursor);
		if (tab_area.contains_point(cursor) && GetAsyncKeyState(VK_LBUTTON)) {
			if (!mtv->tmenu.animating && index != mtv->tmenu.selected_tab) {
				mtv->tmenu.animating = true;
				mtv->tmenu.animate_direction = (index > mtv->tmenu.selected_tab) ? TAB_ANIMATION_DOWN : TAB_ANIMATION_UP;
				mtv->tmenu.animation_destination = tab_area.y;
				mtv->tmenu.selected_tab = index;
			}
		}

		if (index == mtv->tmenu.selected_tab) {
			update_subtabs();
		}

	}
	void add_group(C_GroupBox* gb) { group_boxes.push_back(gb); }

	C_Tab(Menu* n_parent, int n_index, std::string n_icon, std::string n_name, std::vector< subtab_t > n_subtabs = { })
	{
		parent = n_parent;
		index = n_index;
		icon = n_icon;
		name = n_name;

		selected_sub = -1;

		if (n_subtabs.size()) {
			subtabs = n_subtabs;
			selected_sub = subtabs[0].index;
			subtabs[0].selected_opacity = 1.f;
		}

		n_parent->add_tab(this);

		mtv->tmenu.tab_sub = -1;
		mtv->tmenu.last_tab = this;
	}
	void paint_subtabs()
	{
		if (subtabs.size()) {
			rect_t area = rect_t(
				mtv->tmenu.area.x + 8,
				mtv->tmenu.area.y + 8,
				mtv->tmenu.area.w - 16,
				15
			);

			float sub_width = area.w / subtabs.size();

			drawing->filled_rect(ImVec2(area.x, area.y), ImVec2(area.w, 26), ImVec4(0.1176470588235294f, 0.1176470588235294f, 0.1176470588235294f, 1.f), false);
			drawing->rect(ImVec2(area.x, area.y), ImVec2(area.w, 27), ImVec4(0.1882352941176471f, 0.1882352941176471f, 0.1882352941176471f, 1.f), 1.f, false, false);
			drawing->rect(ImVec2(area.x - 1, area.y - 1), ImVec2(area.w + 2, 29), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

			for (int i = 0; i < subtabs.size(); i++) {
				int text_x = area.x + (sub_width * i) + (sub_width / 2);

				int add = 0;

				if (subtabs[i].font == 3) {
					ImGui::PopFont();
					ImGui::PushFont(drawing->fonts.tabFont);
				}

				ImVec2 tsize = ImGui::CalcTextSize(subtabs[i].name, NULL);

				drawing->text(ImVec2(text_x - (tsize.x * 0.5f), (area.y + 13 + add) - (tsize.y * 0.5f)), subtabs[i].name, ImVec4(0.6f, 0.6f, 0.6f, 1.f), true);
				
				drawing->text(ImVec2(text_x - (tsize.x * 0.5f), (area.y + 13 + add) - (tsize.y * 0.5f)), subtabs[i].name, ImVec4(mtv->tmenu.menu_theme[0].x, mtv->tmenu.menu_theme[0].y, mtv->tmenu.menu_theme[0].z, subtabs[i].selected_opacity), true);

				if (subtabs[i].font == 3) {
					ImGui::PopFont();
					ImGui::PushFont(drawing->fonts.menuFont);
				}
			}
		}
	}
	void update_subtabs()
	{
		if (subtabs.size()) {
			rect_t area = rect_t(
				mtv->tmenu.area.x + 8,
				mtv->tmenu.area.y + 8,
				mtv->tmenu.area.w - 16,
				30
			);

			float sub_width = area.w / subtabs.size();

			for (int i = 0; i < subtabs.size(); i++) {
				rect_t tab_area = rect_t(
					area.x + (sub_width * i),
					area.y, sub_width, 26
				);

				POINT cursor;
				GetCursorPos(&cursor);
				if (GetAsyncKeyState(VK_LBUTTON) && tab_area.contains_point(cursor)) {
					if (!animating && selected_sub != subtabs[i].index) {
						animating = true;
						from_anim = selected_sub;
						selected_sub = subtabs[i].index;

						reset_inputblock = true;
					}
				}
			}

			for (int i = 0; i < 10; i++) {
				if (!animating)
					break;

				if(subtabs[from_anim].selected_opacity > 0.f)
					subtabs[from_anim].selected_opacity -= 0.01f;
				
				if(subtabs[selected_sub].selected_opacity < 1.f)
					subtabs[selected_sub].selected_opacity += 0.01f;

				if (!(subtabs[selected_sub].selected_opacity < 1.f || subtabs[from_anim].selected_opacity > 0.f))
					animating = false;
			}
		}
	}

};

// groupbox class
enum GroupRow {
	GROUP_LEFT,
	GROUP_RIGHT
};
class C_GroupBox {
private:
	C_Tab* parent;
	float last_pos = 8;
	std::vector< C_Control* > controls;
	rect_t area;
	std::string name;


	struct SubData_t {
		int
			selected_sub = -1,
			last_sub;
		std::vector< int > sub_tabs;
	}subdata;

	int parent_sub;
public:
	int get_parent_sub() {
		return parent_sub;
	}

	C_Tab* get_tab() {
		return parent;
	}

	void reset_block() {
		parent->reset_inputblock = true;
	}

	void focus_control(C_Control* pointer) {
		parent->block_input = true;
		parent->reset_inputblock = false;
		parent->blocked_control_flag = false;
		parent->overriding_flag = nullptr;
		parent->overriding_control = pointer;
	}

	void focus_subcontrol(C_ControlFlag* pointer) {
		parent->block_input = true;
		parent->reset_inputblock = false;
		parent->blocked_control_flag = false;
		parent->overriding_flag = pointer;
		parent->overriding_control = nullptr;
	}
public:
	void render();
	void update(rect_t mparent_area);
	void update_subs();

	void reset_control_positions();
	void add_control(C_Control* new_control, bool first = true);

	C_GroupBox(GroupRow row, int y, std::string n_name, int sub_tab_count = 0);
};

extern Menu* menu;

#endif