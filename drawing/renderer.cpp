#define STB_IMAGE_IMPLEMENTATION
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/stb_image.h"
#include <tchar.h>
#include "renderer.hpp"
#include <future>
#include "imgui/imgui_internal.h"
#include <io.h>
#include "menu/controls.hpp"
#include "../utils/memory/memory.hpp"
#include "../features/visuals.hpp"

bool Renderer::Initialize()
{
    WINDOWPLACEMENT g_wpPrev;
    DWORD dwStyle = GetWindowLong(this->h_Game, GWL_STYLE);
    MONITORINFO mi = { sizeof(mi) };

    if (GetWindowPlacement(this->h_Game, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(this->h_Game, MONITOR_DEFAULTTOPRIMARY), &mi)) {
        SetWindowLong(this->h_Game, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(this->h_Game, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

    // create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), ACS_TRANSPARENT, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Windows Task Assistant"), NULL };
    ::RegisterClassEx(&wc);
    this->h_hWnd = ::CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, wc.lpszClassName, _T(""), WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, wc.hInstance, NULL);

    globals->screen_size.x = GetSystemMetrics(SM_CXSCREEN);
    globals->screen_size.y = GetSystemMetrics(SM_CYSCREEN);

    // extend window to take the whole screen
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(this->h_hWnd, &margins);

    // set Alpha and ColorKey attributes
    SetLayeredWindowAttributes(this->h_hWnd, 0, 1.0f, LWA_ALPHA);
    SetLayeredWindowAttributes(this->h_hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);

    // initialize Direct3D
    if (!CreateDeviceD3D(this->h_hWnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(this->h_hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(this->h_hWnd);

    // setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(this->h_hWnd);
    ImGui_ImplDX11_Init(this->g_pd3dDevice, this->g_pd3dDeviceContext);

    // init imgui
    menu->Init();

    // set window style
    long style = GetWindowLongPtr(this->h_hWnd, GWL_EXSTYLE);
    style |= WS_EX_LAYERED;
    SetWindowLongPtr(this->h_hWnd, GWL_EXSTYLE, style);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    // start menu thread
    while (msg.message != WM_QUIT)
    {
        if (b_Shutdown)
            break;

        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        visuals::Draw();

        if (menu->isOpen)
        {
            // fading 
            if (fade < 0.5)
                fade += 0.05f;

            // render menu
            menu->Update();
        }
        else {
            if (fade > 0.f)
                fade -= 0.05f;
        }

        // isDown = IsKeyDown || isClicked == Key Was Clicked
        static bool isDown = false;
        static bool isClicked = false;
        bool key = GetAsyncKeyState(VK_INSERT);
        if (key) {
            isClicked = false;
            isDown = true;
        }
        else if (!key && isDown) {
            isClicked = true;
            isDown = false;
        }
        else {
            isClicked = false;
            isDown = false;
        }

        // if INSERT key was clicked
        if (isClicked) {
            // toggle menu
            menu->isOpen = !menu->isOpen;

            // set window system
            style = GetWindowLongPtr(h_hWnd, GWL_EXSTYLE);
            if (menu->isOpen) {
                style &= ~WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
                SetForegroundWindow(h_hWnd);
            }
            else {
                style |= WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
                SetForegroundWindow(this->h_Game);
            }
        }

        // bind render targets
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

        // fade menu in/out
        float clear_color[4] = { 0.f, 0.f, 0.f, fade };
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);

        // render imgui 
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present w/o vsync

        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
}

void Renderer::HandleWindow()
{
    bool b_active = false;
    while (true) {
        SleepEx(1, false);

        // exit if the game window no longer exists
        if (!IsWindow(this->h_Game)) {
            this->b_Shutdown = true;

            break;
        }

        // if the current foreground window is not the game, don't draw the menu
        HWND h_Foreground = GetForegroundWindow();
        if (h_Foreground != this->h_Game && h_Foreground != this->h_hWnd) {
            if (!b_active) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // not leaking
                continue;
            }
            SetWindowPos(this->h_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            b_active = false;
        }
        else {
            if (b_active) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // not leaking
                continue;
            }
            SetWindowPos(this->h_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            b_active = true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

// helper functions
bool Renderer::CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void Renderer::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void Renderer::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void Renderer::CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// win32 message handler
LRESULT WINAPI Renderer::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

Renderer* drawing = new Renderer();