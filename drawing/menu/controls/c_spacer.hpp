#ifndef _SPACER_HPP
#define _SPACER_HPP

// includes for this header
#include "../controls.hpp"

// spacing control class
class C_Spacer : public C_Control {
public:
	C_Spacer(C_Control* control, ImVec2 spacing)
	{
		parent = mtv->tmenu.last_group;

		parent->add_control(this);

		control->offset = spacing;
	}

	void draw() {}
	void update() {}
	void click() {}
};

#endif // _SPACER_HPP