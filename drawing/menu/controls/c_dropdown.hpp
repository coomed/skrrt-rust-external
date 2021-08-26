#ifndef _DROPDOWN_HPP
#define _DROPDOWN_HPP

// includes for this header
#include "../controls.hpp"
#define gg 19

// dropdown control class
class C_Dropdown : public C_Control {
public:
	void draw() 
	{
		rect_t n_area = rect_t(
			area.x,
			area.y + height_offset,
			area.w,
			gg
		);

		POINT mouse; GetCursorPos(&mouse);
		rect_t item_region = rect_t(n_area.x, n_area.y, n_area.w, gg);

		if (label != "")
			drawing->text(ImVec2(area.x, area.y), label, ImVec4(1.f, 1.f, 1.f, 1.f), true);

		if (item_region.contains_point(mouse)) {
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, gg), ImVec4(0.2549019607843137f, 0.2549019607843137f, 0.2549019607843137f, 1.f), false);
		}
		else
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, gg), ImVec4(0.2156862745098039f, 0.2156862745098039f, 0.2156862745098039f, 1.f), false);

		drawing->rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, gg), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

		// lambda for the arrow
		auto arrow = [](int x, int y) {
			for (auto i = 5; i >= 2; --i) {
				auto offset = 5 - i;

				drawing->line(ImVec2(x + offset, y + offset), ImVec2(x + offset + clamp(i - offset, 0, 5), y + offset), ImVec4(0.6f, 0.6f, 0.6f, 1.f), 1.f);
			}
		};

		arrow(n_area.x + n_area.w - 10, n_area.y + 9);

		if (!items.size()) {
			return;
		}

		ImVec2 tsize = ImGui::CalcTextSize(items[*selected].c_str(), NULL);
		drawing->text(ImVec2(n_area.x + 10, (n_area.y + 10) - (tsize.y / 2)), items[*selected], ImVec4(0.6f, 0.6f, 0.6f, 1.f), true);

		if (!open) {
			return;
		}

		n_area.y += 21;
		n_area.h -= gg;

		drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(area.w, items.size() * gg), ImVec4(0.2156862745098039f, 0.2156862745098039f, 0.2156862745098039f, 1.f), false);

		for (int i = 0; i < items.size(); i++) {
			auto& current = items.at(i);

			rect_t item_region = rect_t(n_area.x, n_area.y + (gg * i), n_area.w, gg);

			if (item_region.contains_point(mouse)) {
				drawing->filled_rect(ImVec2(n_area.x, n_area.y + (gg * i)), ImVec2(n_area.w, gg), ImVec4(0.1568627450980392f, 0.1568627450980392f, 0.1568627450980392f, 1.f), false);
			}

			if (i == *selected) {
				ImVec2 tsize = ImGui::CalcTextSize(current.c_str(), NULL);
				drawing->text(ImVec2(area.x + 10, (n_area.y + (gg * i) + 10) - (tsize.y / 2)), current, mtv->tmenu.menu_theme[0], true);
			}
			else {
				ImVec2 tsize = ImGui::CalcTextSize(current.c_str(), NULL);
				drawing->text(ImVec2(area.x + 10, (n_area.y + (gg * i) + 10) - (tsize.y / 2)), current, ImVec4(0.6f, 0.6f, 0.6f, 1.f), true);
			}
		}

		drawing->rect(ImVec2(n_area.x, n_area.y), ImVec2(area.w, items.size() * gg), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);
	}
	void update()
	{
		if (using_pointer)
			items = *items_pointer;

		if (!open || !items.size())
			return;

		POINT mouse; GetCursorPos(&mouse);

		parent->focus_control(this);

		for (int i = 0; i < items.size(); i++) {
			rect_t item_region = rect_t(area.x, area.y + gg + (height_offset)+(i * gg), area.w, gg);

			if (GetAsyncKeyState(VK_LBUTTON) && item_region.contains_point(mouse)) {
				open = false;
				*selected = i;

				parent->reset_block();
			}
		}

		rect_t full_area = rect_t(area.x, area.y + height_offset, area.w, area.h + (items.size() * gg));
		if (GetAsyncKeyState(VK_LBUTTON) && !full_area.contains_point(mouse)) {
			open = false;
			parent->reset_block();
		}
	}
	void click()
	{
		if (parent->get_tab()->index != mtv->tmenu.selected_tab) return;

		if (open)
			parent->reset_block();

		open = !open;
	}
public:
	std::string label;
	int* selected;
	std::vector< std::string > items;
	std::vector< std::string >* items_pointer;
	bool open = false, using_pointer;

	C_Dropdown(std::string n_label, int* n_selected, std::vector< std::string > n_items, std::vector< C_ControlFlag* > n_flags = { })
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		selected = n_selected;
		items = n_items;
		using_pointer = false;

		flags = n_flags;

		area.h = gg;

		if (n_label != "") {
			height_offset = 14;
			area.h += 14;
		}

		parent->add_control(this);
	}
	C_Dropdown(std::string n_label, int* n_selected, std::vector< std::string >* n_items, std::vector< C_ControlFlag* > n_flags = { }) 
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		selected = n_selected;
		items_pointer = n_items;
		using_pointer = true;

		flags = n_flags;

		area.h = gg;

		if (n_label != "") {
			height_offset = 14;
			area.h += 14;
		}

		parent->add_control(this);
	}
};

#endif