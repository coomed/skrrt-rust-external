#ifndef _MULTIDROPDOWN_HPP
#define _MULTIDROPDOWN_HPP

// includes for this header 
#include "../controls.hpp"
#include "../../../utils/utils.hpp"
#define gg 19

// multi dropdown item struct
struct MultiDropdownItem_t {
	bool enabled;
	std::string name;

	MultiDropdownItem_t(bool n_enabled, std::string n_name) {
		enabled = n_enabled;
		name = n_name;
	}
};

// multi dropdown control class
class C_MultiDropdown : public C_Control {
public:
	void draw()
	{

		std::vector< MultiDropdownItem_t > items = *items_ptr;

		rect_t n_area = rect_t(
			area.x,
			area.y + height_offset,
			area.w,
			gg
		);

		if (label != "")
			drawing->text(ImVec2(area.x, area.y), label, ImVec4(1.f, 1.f, 1.f, 1.f), true);
		else
			drawing->text(ImVec2(area.x, area.y), "none", ImVec4(1.f, 1.f, 1.f, 1.f), true);

		POINT mouse; GetCursorPos(&mouse);
		rect_t item_region = rect_t(n_area.x, n_area.y, n_area.w, gg);

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

				drawing->line(ImVec2(x + offset, y + offset), ImVec2(x + offset + clamp(i - offset, 0, 5), y + offset), ImVec4(.6f, .6f, .6f, 1.f), 1.f);
			}
		};

		arrow(n_area.x + n_area.w - 10, n_area.y + 9);

		if (!items.size()) {
			return;
		}

		std::string ss;
		for (auto n = 0; n < items.size(); ++n) {
			auto format_len = ss.length() < 13;
			auto first = ss.length() <= 0;
			if ((items[n].enabled && format_len)) {
				if (!first) {
					ss.append(", ");
				}

				ss.append(items[n].name);
			}
			else if (!format_len) {
				ss.append("...");
				break;
			}
		}

		if (ss.length() <= 0) {
			ss += "none";
		}
		ImVec2 tsize = ImGui::CalcTextSize(ss.c_str(), NULL);
		drawing->text(ImVec2(n_area.x + 10, (n_area.y + 10) - (tsize.y / 2)), ss, ImVec4(0.6f, 0.6f, 0.6f, 1.f), true);

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

			tsize = ImGui::CalcTextSize(current.name.c_str(), NULL);

			if (current.enabled) {
				drawing->text(ImVec2(area.x + 10, (n_area.y + (gg * i) + 10) - (tsize.y / 2)), current.name, mtv->tmenu.menu_theme[0], true);
			}
			else {
				drawing->text(ImVec2(area.x + 10, (n_area.y + (gg * i) + 10) - (tsize.y / 2)), current.name, ImVec4(0.6f, 0.6f, 0.6f, 1.f), true);
			}
		}

		drawing->rect(ImVec2(n_area.x, n_area.y), ImVec2(area.w, items.size() * gg), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);
	}
	void update()
	{
		std::vector< MultiDropdownItem_t > items = *items_ptr;

		if (!open || !items.size())
			return;

		POINT mouse; GetCursorPos(&mouse);

		parent->focus_control(this);

		for (int i = 0; i < items.size(); i++) {
			rect_t item_region = rect_t(area.x, area.y + gg + (height_offset)+(i * gg), area.w, gg);

			if (GetAsyncKeyState(VK_LBUTTON) && item_region.contains_point(mouse)) {
				items[i].enabled = !items[i].enabled;
			}
		}

		rect_t full_area = rect_t(area.x, area.y + height_offset, area.w, area.h + (items.size() * gg));
		if (GetAsyncKeyState(VK_LBUTTON) && !full_area.contains_point(mouse)) {
			open = false;
			parent->reset_block();
		}

		*items_ptr = items;
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
	std::vector< MultiDropdownItem_t >* items_ptr;
	bool open = false;

	C_MultiDropdown(std::string n_label, std::vector< MultiDropdownItem_t >* n_items, std::vector< C_ControlFlag* > n_flags = { }) 
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		items_ptr = n_items;

		flags = n_flags;

		area.h = gg;

		if (n_label != "") {
			height_offset = 14;
			area.h += 14;
		}

		parent->add_control(this);
	}
};

#endif // _MULTIDROPDOWN_HPP