#ifndef _DRAWING_HPP
#define _DRAWING_HPP

// includes for this header
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <string>
#include <dwmapi.h>
#include "imgui/imgui_internal.h"
#pragma comment(lib, "dwmapi.lib")

// forward declare message handler from imgui_impl_win32.cpp
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// class that handles drawing/rendering
class Renderer
{
private: // private methods for this class
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WNDCLASSEX wc;

	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	float fade = 0.f;
public: // public methods for this class
	struct fonts
	{
		ImFont* espFont{};
		ImFont* menuFont{};
		ImFont* tabFont{};
	} fonts;

	bool Initialize();
	void HandleWindow();

	// hwnd window
	HWND h_Game;
	HWND h_hWnd;

	void text(ImVec2 v2_pos, std::string str_text, ImVec4 v4_col, bool b_outline, double font_size = 0.f)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const char* text_end = str_text.c_str() + strlen(str_text.c_str());
		const char* text_display_end = ImGui::FindRenderedTextEnd(str_text.c_str(), text_end);

		if (font_size == 0.f) font_size = g.FontSize;

		if (b_outline) {
			window->DrawList->AddText(g.Font, font_size, v2_pos - ImVec2(1, 0), ImGui::GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
			window->DrawList->AddText(g.Font, font_size, v2_pos - ImVec2(0, 1), ImGui::GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
			window->DrawList->AddText(g.Font, font_size, v2_pos + ImVec2(1, 0), ImGui::GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
			window->DrawList->AddText(g.Font, font_size, v2_pos + ImVec2(0, 1), ImGui::GetColorU32(ImVec4(0, 0, 0, v4_col.w)), str_text.c_str(), NULL, 0.0f, NULL);
		}
		window->DrawList->AddText(g.Font, font_size, v2_pos, ImGui::GetColorU32(v4_col), str_text.c_str(), NULL, 0.0f, NULL);
	}
	auto line(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness, bool b_outline = false)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		window->DrawList->AddLine(v2_pos, v2_size, ImGui::GetColorU32(v4_col), f_thickness);
		if (b_outline) {
			window->DrawList->AddLine(v2_pos, v2_size, ImGui::GetColorU32({0.f, 0.f, 0.f, 1.f}), f_thickness + 1.f);
		}
	}
	auto line(RECT rect, ImVec4 v4_col, float f_thickness)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImRect line_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

		window->DrawList->AddLine(ImVec2(line_bb.Min.x, line_bb.Min.y), ImVec2(line_bb.Max.x, line_bb.Max.y), ImGui::GetColorU32(v4_col), f_thickness);
	}
	void rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImRect rect_bb(v2_pos, v2_pos + v2_size);

		if (b_outline) {
			window->DrawList->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
		}
		if (b_inline) {
			window->DrawList->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
		}
		window->DrawList->AddRect(rect_bb.Min, rect_bb.Max, ImGui::GetColorU32(v4_col), 0.0f, 15, f_thickness);
	}
	void rect(RECT rect, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImRect rect_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

		if (b_outline) {
			window->DrawList->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
		}
		if (b_inline) {
			window->DrawList->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, 15, f_thickness);
		}
		window->DrawList->AddRect(rect_bb.Min, rect_bb.Max, ImGui::GetColorU32(v4_col), 0.0f, 15, f_thickness);
	}
	void crosshair(ImVec4 v4_col, float f_thickness, float len, float xoffset, float yoffset)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		int CenterX = GetSystemMetrics(0) / 2 - 1;
		int CenterY = GetSystemMetrics(1) / 2 - 1;

		rect(ImVec2(CenterX - (len * 2) + xoffset, CenterY + yoffset), ImVec2(len, 1), ImVec4(1.f, 1.f, 1.f, 1.f), 0.5f, true, true);//Left line
		rect(ImVec2(CenterX + len + xoffset, CenterY + yoffset), ImVec2(len, 1), ImVec4(1.f, 1.f, 1.f, 1.f), 0.5f, true, true);//Right line
		circle(ImVec2(CenterX, CenterY), 1, ImVec4(0.f, 0.f, 0.f, 1.f), 0.5, true, true);
	}
	void filled_rect(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, bool b_outline, ImVec4 v2_outline_col = { 0.f, 0.f, 0.f, 1.f }, float outline_width = 1.f)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImRect rect_bb(v2_pos, v2_pos + v2_size);

		if (b_outline) {
			window->DrawList->AddRectFilled(rect_bb.Min - ImVec2(outline_width, outline_width), rect_bb.Max + ImVec2(outline_width, outline_width), ImGui::GetColorU32(v2_outline_col), 0.0f);
		}
		window->DrawList->AddRectFilled(rect_bb.Min, rect_bb.Max, ImGui::GetColorU32(v4_col), 0.0f);
	}
	void filled_rect_gradient(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, ImVec4 v4_col2)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImRect rect_bb(v2_pos, v2_pos + v2_size);

		window->DrawList->AddRectFilledMultiColor(rect_bb.Min, rect_bb.Max, ImGui::GetColorU32(v4_col), ImGui::GetColorU32(v4_col), ImGui::GetColorU32(v4_col2), ImGui::GetColorU32(v4_col2));
	}
	void gradient(ImVec2 v2_pos, ImVec2 v2_size, ImVec4 v4_col, ImColor v4_col2)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImRect rect_bb(v2_pos, v2_pos + v2_size);

		auto gradient = ImGui::GetColorU32(v4_col) - v4_col2;
		window->DrawList->AddRectFilledMultiColor(rect_bb.Min, rect_bb.Max, ImGui::GetColorU32(v4_col), ImGui::GetColorU32(v4_col), gradient, gradient);
	}
	void filled_rect(RECT rect, ImVec4 v4_col, bool b_outline)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImRect rect_bb(ImVec2(rect.left, rect.top), ImVec2(rect.right, rect.bottom));

		if (b_outline) {
			window->DrawList->AddRectFilled(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f);
		}
		window->DrawList->AddRectFilled(rect_bb.Min, rect_bb.Max, ImGui::GetColorU32(v4_col), 0.0f);
	}
	void circle(ImVec2 v2_pos, float f_radius, ImVec4 v4_col, float f_thickness, bool b_outline, bool b_inline, float indicies = 50)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		if (b_outline) {
			window->DrawList->AddCircle(v2_pos, f_radius + 1.0f, ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
		}
		if (b_inline) {
			window->DrawList->AddCircle(v2_pos, f_radius - 1.0f, ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
		}
		window->DrawList->AddCircle(v2_pos, f_radius, ImGui::GetColorU32(v4_col), indicies, f_thickness);
	}
	void filled_circle(ImVec2 v2_pos, float f_radius, ImVec4 v4_col, ImVec4 v4_inline_col, float f_thickness, bool b_outline, bool b_inline)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		if (b_outline) {
			window->DrawList->AddCircle(v2_pos, f_radius + 1.0f, ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
		}
		if (b_inline) {
			window->DrawList->AddCircle(v2_pos, f_radius - 1.0f, ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), (int)f_radius, f_thickness);
		}
		window->DrawList->AddCircleFilled(v2_pos, f_radius, ImGui::GetColorU32(v4_inline_col), 50);
	}

	bool b_Shutdown = false;

	ImFont* font1;
	ImFont* font2;
	ImFont* font3;
};
extern Renderer* drawing;

#endif // _DRAWING_HPP