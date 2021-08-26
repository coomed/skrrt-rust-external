#ifndef _CHECKBOX_HPP
#define _CHECKBOX_HPP

// includes for this header
#include "../controls.hpp"

// checkbox class
class C_CheckBox : public C_Control {
public:
	void draw()
	{
		drawing->filled_rect(ImVec2(area.x, area.y), ImVec2(9, 9), ImVec4(0.2431372549019608f, 0.2431372549019608f, 0.2431372549019608f, 1.f), false);

		POINT mouse; GetCursorPos(&mouse);
		rect_t item_region = rect_t(area.x, area.y, 9, 9);

		if (item_region.contains_point(mouse))
		{
			drawing->filled_rect(ImVec2(area.x, area.y), ImVec2(9, 9), ImVec4(0.2823529411764706f, 0.2823529411764706f, 0.2823529411764706f, 1.f), false);
		}

		if (*state)
		{
			drawing->filled_rect(ImVec2(area.x, area.y), ImVec2(9, 9), mtv->tmenu.menu_theme[0], false);
		}

		drawing->rect(ImVec2(area.x, area.y), ImVec2(9, 9), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

		ImVec2 tsize = ImGui::CalcTextSize(label.c_str(), NULL);
		drawing->text(ImVec2(area.x + 15, (area.y + 5) - (tsize.y / 2)), label, ImVec4(1.f, 1.f, 1.f, 1.f), false);
	}
	void update() {}
	void click()
	{
		if (parent->get_tab()->index != mtv->tmenu.selected_tab) return;
		*state = !(*state);
	}
public:
	std::string label;
	bool* state;

	C_CheckBox(std::string n_label, bool* n_state, std::vector< C_ControlFlag* > n_flags = { })
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		state = n_state;

		flags = n_flags;

		area.h = 9;

		parent->add_control(this);
	}

};

#endif