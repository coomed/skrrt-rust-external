#ifndef _TEXTSEPERATOR_HPP
#define _TEXTSEPERATOR_HPP

// includes for this header
#include "../controls.hpp"

// text seperator type
enum TextSeperatorType {
	SEPERATOR_NORMAL,
	SEPERATOR_BOLD
};

// text seperator class
class C_TextSeperator : public C_Control {
public:
	void draw()
	{
		if (type == SEPERATOR_BOLD) {
			drawing->text(ImVec2(area.x + 1, area.y + 4), label, { 153, 153, 153, 255 }, true);
			drawing->line(ImVec2(area.x, area.y + 18), ImVec2(area.x + area.w, area.y + 18), { 153, 153, 153, 255 }, 1.f);
		}
		else {
			drawing->text(ImVec2(area.x, area.y), label, { 255, 255, 255, 255 }, true);
		}
	}

	void update() {};
	void click() {};
public:
	std::string label;
	TextSeperatorType type;

	C_TextSeperator(std::string n_label, TextSeperatorType n_type, std::vector< C_ControlFlag* > n_flags = { })
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		type = n_type;

		flags = n_flags;

		area.h = (type == SEPERATOR_NORMAL) ? 12 : 18;

		parent->add_control(this);
	}
};

#endif // _TEXTSEPERATOR_HPP