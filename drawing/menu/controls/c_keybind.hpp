#ifndef _KEYBIND_HPP
#define _KEYBIND_HPP

// includes for this header
#include "../controls.hpp"
#include <algorithm>
#include <map>

// button codes
const char* button_codes[] = {
    "NO KEY",
    "LEFT CLICK",
    "RIGHT CLICK",
    "CANCEL",
    "MOUSE 3",
    "MOUSE 4",
    "MOUSE 5",
    "NO KEY",
    "BACKSPACE",
    "TAB KEY",
    "NO KEY",
    "NO KEY",
    "CLEAR",
    "RETURN",
    "NO KEY",
    "NO KEY",
    "SHIFT",
    "CONTROL",
    "ALT",
    "PAUSE",
    "CAPS LOCK",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "EXIT",
    "IME Convert",
    "IME NConvert",
    "IME ACCEPT",
    "IME MCR",
    "SPACE BAR",
    "PAGE UP",
    "PAGE DOWN",
    "END",
    "HOME",
    "LEFT ARROW",
    "UP ARROW",
    "RIGHT ARROW",
    "DOWN ARROW",
    "SELECT KEY",
    "PRINT KEY",
    "EXECUTE KEY",
    "PRINT KEY",
    "INSERT KEY",
    "DELETE KEY",
    "HELP KEY",
    "NUMBER 0",
    "NUMBER 1",
    "NUMBER 2",
    "NUMBER 3",
    "NUMBER 4",
    "NUMBER 5",
    "NUMBER 6",
    "NUMBER 7",
    "NUMBER 8",
    "NUMBER 9",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "A KEY",
    "B KEY",
    "C KEY",
    "D KEY",
    "E KEY",
    "F KEY",
    "G KEY",
    "H KEY",
    "I KEY",
    "J KEY",
    "K KEY",
    "L KEY",
    "M KEY",
    "N KEY",
    "O KEY",
    "P KEY",
    "Q KEY",
    "R KEY",
    "S KEY",
    "T KEY",
    "U KEY",
    "V KEY",
    "W KEY",
    "X KEY",
    "Y KEY",
    "Z KEY",
    "WINDOWS KEY",
    "WINDOWS KEY",
    "APP KEY",
    "NO KEY",
    "SLEEP KEY",
    "NUMPAD 0",
    "NUMPAD 1",
    "NUMPAD 2",
    "NUMPAD 3",
    "NUMPAD 4",
    "NUMPAD 5",
    "NUMPAD 6",
    "NUMPAD 7",
    "NUMPAD 8",
    "NUMPAD 9",
    "NUMPAD MULTIPLY",
    "NUMPAD ADD",
    "NUMPAD SEPERATOR",
    "NUMPAD SUBTRACT",
    "NUMPAD DECIMAL",
    "NUMPAD DIVIDE",
    "FUNCTION 1",
    "FUNCTION 2",
    "FUNCTION 3",
    "FUNCTION 4",
    "FUNCTION 5",
    "FUNCTION 6",
    "FUNCTION 7",
    "FUNCTION 8",
    "FUNCTION 9",
    "FUNCTION 10",
    "FUNCTION 11",
    "FUNCTION 12",
    "FUNCTION 13",
    "FUNCTION 14",
    "FUNCTION 15",
    "FUNCTION 16",
    "FUNCTION 17",
    "FUNCTION 18",
    "FUNCTION 19",
    "FUNCTION 20",
    "FUNCTION 21",
    "FUNCTION 22",
    "FUNCTON 23",
    "FUNCTION 24",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NUMLOCK",
    "SCROLL LOCK",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "NO KEY",
    "LEFT SHIFT",
    "RIGHT SHIFT",
    "LEFT CONTROL",
    "RIGHT CONTROL",
    "LEFT MENU",
    "RIGHT MENU"
};

// keybind control class
class C_KeyBind : public C_Control {
private:
    std::map<int, bool> key_held;
    ULONGLONG time_clicked;
public:
	void draw()
	{
		rect_t n_area = rect_t(
			area.x,
			area.y + height_offset,
			area.w,
			20
		);

		if (label != "")
			drawing->text(ImVec2(area.x, area.y), label, ImVec4(1.f, 1.f, 1.f, 1.f), true);

		POINT mouse; GetCursorPos(&mouse);
		rect_t item_region = rect_t(n_area.x, n_area.y, n_area.w, 20);

		if (item_region.contains_point(mouse)) {
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, 20), ImVec4(0.2549019607843137f, 0.2549019607843137f, 0.2549019607843137f, 1.f), false);
		}
		else
			drawing->filled_rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, 20), ImVec4(0.2156862745098039f, 0.2156862745098039f, 0.2156862745098039f, 1.f), false);

		drawing->rect(ImVec2(n_area.x, n_area.y), ImVec2(n_area.w, 20), ImVec4(0.f, 0.f, 0.f, 1.f), 1.f, false, false);

        std::string to_render = button_codes[*key];

		if (taking_input) {
			to_render = "PRESS KEY";
		}

        ImVec2 tsize = ImGui::CalcTextSize(to_render.c_str(), NULL);
        drawing->text(ImVec2(n_area.x + 10, (n_area.y + 10) - (tsize.y / 2)), to_render, taking_input ? mtv->tmenu.menu_theme[0] : ImVec4(0.6f, 0.6f, 0.6f, 1.f), true);
	}
	void update()
    {
        if (!taking_input || (GetTickCount64() - time_clicked) <= 300) {
            return;
        }

        parent->focus_control(this);

        for (int i = 0; i < 255; i++) {
            if (GetAsyncKeyState(i) && !key_held[i]) {
                key_held[i] = true;
                *key = i;

                if (*key == VK_ESCAPE) {
                    *key = 0x0;
                    taking_input = false;

                    parent->reset_block();

                    return;
                }

                taking_input = false;
                parent->reset_block();
                return;
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

        if (!taking_input) {
            time_clicked = GetTickCount64();
            taking_input = true;
        }
    }
public:
	std::string label;
    int *key;
	bool taking_input;

	C_KeyBind(std::string n_label, int* n_key)
	{
		parent = mtv->tmenu.last_group;
		label = n_label;
		key = n_key;

		area.h = 20;

		if (n_label != "") {
			height_offset = 14;
			area.h += 14;
		}

		parent->add_control(this);
	}
};

#endif // _KEYBIND_HPP