#ifndef _TEXTFIELD_HPP
#define _TEXTFIELD_HPP

// includes for this header
#include <map>
#include "../controls.hpp"

// textfield control class
class C_TextField : public C_Control {
private:
	const char* lowercase[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
	"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	const char* uppercase[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
	"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
	"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "_", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	std::map<int, bool> key_held;
public:
	void draw()
	{
		rect_t n_area = rect_t(
			area.x,
			area.y + height_offset,
			area.w,
			19
		);

		if (label != "")
			drawing->text(ImVec2(area.x, area.y), label, ImVec4(1.f, 1.f, 1.f, 1.f), true);

		POINT mouse; GetCursorPos(&mouse);
		rect_t item_region = rect_t(n_area.x, n_area.y, n_area.w, 19);

		if (item_region.contains_point(mouse)) {
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, 19), ImVec4(0.2549019607843137f, 0.2549019607843137f, 0.2549019607843137f, 1.f), false);
		}
		else
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, 19), ImVec4(0.2156862745098039f, 0.2156862745098039f, 0.2156862745098039f, 1.f), false);

		drawing->rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, 19), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

		std::string to_render = *input;

		if (GetTickCount64() >= next_blink) {
			next_blink = GetTickCount64() + 800;
		}

		if (taking_input && GetTickCount64() > (next_blink - 400)) {
			to_render += "_";
		}

		if (taking_input)
			drawing->text(ImVec2(n_area.x + 10, n_area.y + 3), to_render, ImVec4(1.f, 1.f, 1.f, 1.f), true);
		else
			drawing->text(ImVec2(n_area.x + 10, n_area.y + 3), to_render, ImVec4(0.8627450980392157f, 0.8627450980392157f, 0.8627450980392157f, 1.f), true);
	}
	void update()
	{
		if (!taking_input) {
			return;
		}

		std::string str = *input;

		for (int i = 0; i < 255; i++) {
			if (GetAsyncKeyState(i) && !key_held[i]) {
				key_held[i] = true;

				if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT) {
					taking_input = false;
					return;
				}

				if (strlen(str.c_str()) != 0) {
					if (GetAsyncKeyState(VK_BACK)) {
						*input = str.substr(0, strlen(str.c_str()) - 1);
					}
				}

				if (strlen(str.c_str()) < 42 && i != NULL && uppercase[i] != nullptr) {
					if (GetAsyncKeyState(VK_SHIFT)) {
						*input = str + uppercase[i];
					}
					else {
						*input = str + lowercase[i];
					}

					return;
				}

				if (strlen(str.c_str()) < 42 && i == 32) {
					*input = str + " ";
					return;
				}
			}
			else if (!GetAsyncKeyState(i) && key_held[i])
			{
				key_held[i] = false;
			}
		}
	}
	void click()
	{
		if (parent->get_tab()->index != mtv->tmenu.selected_tab) return;
		taking_input = !taking_input;
	}


public:
	std::string label;
	std::string* input;
	bool taking_input = false;
	float next_blink;

	C_TextField(std::string n_label, std::string* n_input)
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		input = n_input;

		area.h = 19;

		if (n_label != "") {
			height_offset = 14;
			area.h += 14;
		}

		parent->add_control(this);
	}
};


#endif // _TEXTFIELD_HPP