#ifndef _SLIDERINT_HPP
#define _SLIDERINT_HPP

// includes for this header
#include <sstream>
#include "../controls.hpp"
#define gg 9

// class for a int slider
class C_SliderInt : public C_Control {
public:
	void draw()
	{
		rect_t n_area = area;

		drawing->text(ImVec2(area.x, area.y), label, ImVec4(1.f, 1.f, 1.f, 1.f), true);

		n_area.y += height_offset;
		n_area.h -= height_offset;

		POINT mouse; GetCursorPos(&mouse);
		rect_t item_region = rect_t(n_area.x, n_area.y, n_area.w, gg);

		if (item_region.contains_point(mouse)) {
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(area.w, gg), ImVec4(0.2823529411764706f, 0.2823529411764706f, 0.2823529411764706f, 1.f), false);
		}
		else
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(area.w, gg), ImVec4(0.2431372549019608f, 0.2431372549019608f, 0.2431372549019608f, 1.f), false);

		float ratio = (*value - min) / (max - min);
		float location = ratio * area.w;

		drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(location, gg), mtv->tmenu.menu_theme[0], false);

		drawing->rect(ImVec2(n_area.x, n_area.y), ImVec2(area.w, gg), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

		std::stringstream ss;
		ss << *value << follower.c_str();

		ImVec2 tsize = ImGui::CalcTextSize(ss.str().c_str(), NULL);
		drawing->text(ImVec2((n_area.x + location) - (tsize.x / 2), n_area.y + 6), ss.str(), ImVec4(1.f, 1.f, 1.f, 1.f), true);
	}
	void update()
	{
		if (dragging) {
			if (GetAsyncKeyState(VK_LBUTTON)) {
				POINT mouse;  GetCursorPos(&mouse);

				float
					new_x,
					ratio;

				new_x = mouse.x - area.x;

				if (new_x < 0) { new_x = 0; }
				if (new_x > area.w) { new_x = area.w; }

				ratio = new_x / float(area.w);
				*value = min + (max - min) * ratio;
			}
			else {
				dragging = false;
			}
		}
	}
	void click() 
	{
		if (parent->get_tab()->index != mtv->tmenu.selected_tab) return;

		dragging = true;
	}
public:
	std::string label;
	float min, max;
	int* value;
	std::string follower;
	bool dragging;

	C_SliderInt(std::string n_label, int* n_value, int n_min, int n_max, std::string n_follower, std::vector< C_ControlFlag* > n_flags = { })
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		value = n_value;
		min = n_min;
		max = n_max;
		follower = n_follower;

		flags = n_flags;

		area.h = 15;

		if (n_label != "") {
			height_offset = 14;
			area.h += 14;
		}

		parent->add_control(this);
	}
};

#endif // _SLIDERINT_HPP