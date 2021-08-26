#ifndef _BUTTON_HPP
#define _BUTTON_HPP

// includes for this header
#include "../controls.hpp"

// button control class
class C_Button : public C_Control {
public:
	std::string label;
	std::function< void() > func;

	C_Button(std::string n_label, std::function< void() > n_func = { })
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		func = n_func;

		area.h = 20;

		parent->add_control(this);
	}
public:
	void draw()
	{
		drawing->filled_rect(ImVec2(area.x, area.y), ImVec2(area.w, 20), ImVec4(0.2156862745098039f, 0.2156862745098039f, 0.2156862745098039f, 1.f), false);

		POINT mouse; GetCursorPos(&mouse);
		rect_t item_region = rect_t(area.x, area.y, area.w, 20);

		if (item_region.contains_point(mouse)) {
			drawing->filled_rect(ImVec2(area.x, area.y), ImVec2(area.w, 20), ImVec4(0.2549019607843137f, 0.2549019607843137f, 0.2549019607843137f, 1.f), false);
		}

		drawing->rect(ImVec2(area.x, area.y), ImVec2(area.w, 20), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

		ImVec2 tsize = ImGui::CalcTextSize(label.c_str(), NULL);
		drawing->text(ImVec2(area.x + 10, (area.y + 10) - (tsize.y / 2)), label, ImVec4(1.f, 1.f, 1.f, 1.f), true);
	}
	void update() {

	}
	void click()
	{
		if (parent->get_tab()->index != mtv->tmenu.selected_tab) return;
		func();
	}
};

#endif // _BUTTON_HPP