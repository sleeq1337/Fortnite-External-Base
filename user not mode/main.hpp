#pragma once
#include "Public/keybind.hpp"

#include "Global.hpp"
#include <iostream>
#include <dwmapi.h>
#include <d3d9.h>
#include <format>
#include <bit>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <cmath>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

inline IDirect3D9Ex* p_object = nullptr;
inline IDirect3DDevice9Ex* p_device = nullptr;
inline D3DPRESENT_PARAMETERS p_params = { };
inline MSG messager = { };
inline HWND my_wnd = nullptr;
inline HWND Window = nullptr;
ImFont* m_pFont = nullptr;


inline HWND get_process_wnd(uint32_t pid)
{
    std::pair<HWND, uint32_t> params = { 0, pid };
    BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
        {
            auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
            uint32_t processid = 0;
            if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
            {
                SetLastError((uint32_t)-1);
                pparams->first = hwnd;
                return FALSE;
            }
            return TRUE;
        }, (LPARAM)&params);

    if (!bresult && GetLastError() == -1 && params.first)
        return params.first;
    return 0;
}

inline HRESULT DX_Init()
{
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object))) exit(3);

    D3DPRESENT_PARAMETERS local_p_params = {};
    ZeroMemory(&local_p_params, sizeof(local_p_params));

    local_p_params.Windowed = TRUE;
    local_p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    local_p_params.hDeviceWindow = my_wnd;
    local_p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    local_p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
    local_p_params.BackBufferWidth = settings::width;
    local_p_params.BackBufferHeight = settings::height;
    local_p_params.EnableAutoDepthStencil = TRUE;
    local_p_params.AutoDepthStencilFormat = D3DFMT_D16;
    local_p_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &local_p_params, 0, &p_device)))
    {
        p_object->Release();
        exit(4);
    }
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(my_wnd);
    ImGui_ImplDX9_Init(p_device);
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\calibrib.ttf", 18.0f);
    ImGui_ImplDX9_InvalidateDeviceObjects();
    p_object->Release();
    return S_OK;
}


inline void create_overlay()
{
    WNDCLASSEXA wcex = {
        sizeof(WNDCLASSEXA),
        0,
        DefWindowProcA,
        0,
        0,
        0,
        LoadIcon(0, IDI_APPLICATION),
        LoadCursor(0, IDC_ARROW),
        0,
        0,
        "Chrome_WidgetWin_0",
        LoadIcon(0, IDI_APPLICATION)
    };
    ATOM rce = RegisterClassExA(&wcex);
    RECT rect;
    GetWindowRect(GetDesktopWindow(), &rect);
    my_wnd = gui::create_window_in_band(0, rce, L"Chrome_WidgetWin_0", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
    SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
    SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(my_wnd, &margin);
    ShowWindow(my_wnd, SW_SHOW);
    UpdateWindow(my_wnd);
}

namespace memoryk
{
    constexpr uintptr_t NetConnection = 0x528;
    constexpr uintptr_t RotationInput = NetConnection + 0x8;
}

inline Vector3 last_aim_angle = Vector3(0, 0, 0);

inline void aimbot(uintptr_t target_mesh)
{
    if (!target_mesh || !cache::player_controller)
        return;

    Vector3 head3d = get_entity_bone(target_mesh, 110);
    Vector2 head2d = project_world_to_screen(head3d);

    if (head2d.x == 0 || head2d.y == 0)
        return;

    float deltaX = head2d.x - settings::screen_center_x;
    float deltaY = head2d.y - settings::screen_center_y;

    Vector3 target_angle;
    target_angle.x = -deltaY * 0.016f;
    target_angle.y = deltaX * 0.016f;
    target_angle.z = 0.0f;

    float smooth_factor = std::clamp(Aimbot::smoothness, 1.0f, 20.0f);

    Vector3 new_angle;
    new_angle.x = last_aim_angle.x + (target_angle.x - last_aim_angle.x) / smooth_factor;
    new_angle.y = last_aim_angle.y + (target_angle.y - last_aim_angle.y) / smooth_factor;
    new_angle.z = 0.0f;

    last_aim_angle = new_angle;

    static constexpr float MAX_ANGULAR_SPEED = 20.0f;
    if (smooth_factor > 1.0f)
    {
        float delta_pitch = new_angle.x - last_aim_angle.x;
        float delta_yaw = new_angle.y - last_aim_angle.y;
        float max_delta = MAX_ANGULAR_SPEED / smooth_factor;

        if (std::abs(delta_pitch) > max_delta)
            new_angle.x = last_aim_angle.x + (delta_pitch > 0 ? max_delta : -max_delta);
        if (std::abs(delta_yaw) > max_delta)
            new_angle.y = last_aim_angle.y + (delta_yaw > 0 ? max_delta : -max_delta);
    }

    write<Vector3>(cache::player_controller + memoryk::RotationInput, new_angle);
}




inline float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
{
    std::stringstream stream(text);
    std::string line;

    float y = 0.0f;
    int i = 0;

    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    while (std::getline(stream, line))
    {
        ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

        if (center)
        {
            drawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());
        }
        else
        {
            drawList->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + textSize.y * i + 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + textSize.y * i - 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + textSize.y * i - 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + textSize.y * i + 1), IM_COL32(0, 0, 0, 255), line.c_str());
            drawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());
        }

        y = pos.y + textSize.y * (i + 1);
        i++;
    }
    return y;
}


inline void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
    auto draw = ImGui::GetForegroundDrawList();
    ImColor outline_color = ImColor(0, 0, 0, 255); 

    int outline_offset = 1;
    draw->AddLine(ImVec2(x - outline_offset, y - outline_offset), ImVec2(x - outline_offset, y + (h / 3) - outline_offset), outline_color, thickness);
    draw->AddLine(ImVec2(x - outline_offset, y - outline_offset), ImVec2(x + (w / 3) - outline_offset, y - outline_offset), outline_color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3) + outline_offset, y - outline_offset), ImVec2(x + w + outline_offset, y - outline_offset), outline_color, thickness);
    draw->AddLine(ImVec2(x + w + outline_offset, y - outline_offset), ImVec2(x + w + outline_offset, y + (h / 3) - outline_offset), outline_color, thickness);
    draw->AddLine(ImVec2(x - outline_offset, y + h - (h / 3) + outline_offset), ImVec2(x - outline_offset, y + h + outline_offset), outline_color, thickness);
    draw->AddLine(ImVec2(x - outline_offset, y + h + outline_offset), ImVec2(x + (w / 3) - outline_offset, y + h + outline_offset), outline_color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3) + outline_offset, y + h + outline_offset), ImVec2(x + w + outline_offset, y + h + outline_offset), outline_color, thickness);
    draw->AddLine(ImVec2(x + w + outline_offset, y + h - (h / 3) + outline_offset), ImVec2(x + w + outline_offset, y + h + outline_offset), outline_color, thickness);

    draw->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
    draw->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
    draw->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
    draw->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
    draw->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
    draw->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
    draw->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}


inline void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

inline void draw_line(Vector2 target, const ImColor color)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_y, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

inline void draw_distance(Vector2 location, float distance, const ImColor color)
{
    char dist[64];
    sprintf_s(dist, "[%.fm]", distance);
    ImVec2 text_size = ImGui::CalcTextSize(dist);

    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    ImVec2 pos = ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2);

    ImU32 outline_color = IM_COL32(0, 0, 0, 255);
    draw_list->AddText(ImVec2(pos.x - 1, pos.y), outline_color, dist);
    draw_list->AddText(ImVec2(pos.x + 1, pos.y), outline_color, dist);
    draw_list->AddText(ImVec2(pos.x, pos.y - 1), outline_color, dist);
    draw_list->AddText(ImVec2(pos.x, pos.y + 1), outline_color, dist);
    draw_list->AddText(pos, color, dist);
}




struct RGBA {
    float R, G, B, A;
};

void DrawBox(float X, float Y, float W, float H, RGBA* color)
{
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    drawList->AddRect(
        ImVec2(X - 1, Y - 1),
        ImVec2(X + W + 1, Y + H + 1),
        IM_COL32(0, 0, 0, 255),
        0.0f, 0, 1.0f
    );

    drawList->AddRect(
        ImVec2(X, Y),
        ImVec2(X + W, Y + H),
        ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0f, color->G / 255.0f, color->B / 255.0f, color->A / 255.0f)),
        0.0f, 0, 1.0f
    );
}


inline uintptr_t get_current_actor()
{
    return cache::local_pawn;
}

inline uint64_t decryptUWorld(uint64_t encrypted) {
    return std::rotl(encrypted, 56) - offsets::UWorldXorKey;
}

inline void Loop()
{
    uintptr_t encryptedUWorld = read<uintptr_t>(Base + offsets::UWorld);
    uintptr_t decryptedUWorld = decryptUWorld(encryptedUWorld);

    cache::uworld = decryptedUWorld;

    cache::game_instance = read<uintptr_t>(cache::uworld + offsets::OwningGameInstance);
    cache::local_players = read<uintptr_t>(read<uintptr_t>(cache::game_instance + offsets::LocalPlayers));
    cache::player_controller = read<uintptr_t>(cache::local_players + offsets::PlayerController);
    cache::local_pawn = read<uintptr_t>(cache::player_controller + offsets::AcknowledgedPawn);
    if (cache::local_pawn != 0)
    {
        cache::root_component = read<uintptr_t>(cache::local_pawn + offsets::RootComponent);
        cache::relative_location = read<Vector3>(cache::root_component + offsets::RelativeLocation);
        cache::player_state = read<uintptr_t>(cache::local_pawn + offsets::PlayerState);
        cache::my_team_id = read<int>(cache::player_state + offsets::TeamIndex);
    }
    cache::game_state = read<uintptr_t>(cache::uworld + offsets::GameState);
    cache::player_array = read<uintptr_t>(cache::game_state + offsets::PlayerArray);
    cache::player_count = read<int>(cache::game_state + (offsets::PlayerArray + sizeof(uintptr_t)));
    cache::closest_distance = FLT_MAX;
    cache::closest_mesh = NULL;

    for (int i = 0; i < cache::player_count; i++)
    {
        uintptr_t player_state = read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
        if (!player_state) continue;
        int player_team_id = read<int>(player_state + offsets::TeamIndex);
        if (player_team_id == cache::my_team_id) continue;
        uintptr_t pawn_private = read<uintptr_t>(player_state + offsets::PawnPrivate);
        if (!pawn_private) continue;
        if (pawn_private == cache::local_pawn) continue;
        uintptr_t mesh = read<uintptr_t>(pawn_private + offsets::Mesh);
        if (!mesh) continue;
        Vector3 head3d = get_entity_bone(mesh, 67);
        Vector2 head2d = project_world_to_screen(Vector3(head3d.x, head3d.y, head3d.z + 20));
        Vector3 bottom3d = get_entity_bone(mesh, 0);
        Vector2 bottom2d = project_world_to_screen(bottom3d);
        float box_height = abs(head2d.y - bottom2d.y);
        float box_width = box_height * 0.3f;
        float WierdWidth = box_height * 0.35f;
        float distance = cache::relative_location.distance(bottom3d) / 100;

        if (visuals::enable)
        {
            if (visuals::CorneredBox)
            {
                if (is_visible(mesh))
                {
                    draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 255, 0, 250), 2);
                }
                else
                {
                    draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 255, 0, 250), 2);
                }

            }

            if (visuals::Box)
            {
            RGBA boxColor = { 255, 0, 0, 255 };
            float x = head2d.x - (box_width / 2);
            float y = head2d.y;
            float w = box_width;
            float h = box_height;
            DrawBox(x, y, w, h, &boxColor);
            }

            if (visuals::line)
            {
                ImColor line_color = is_visible(mesh) ? ImColor(250, 250, 250, 250) : ImColor(250, 0, 0, 250);
                draw_line(bottom2d, line_color);
            }

            if (visuals::distance)
            {
                Vector2 distancePos = head2d;
                if (visuals::Box) {
                    distancePos.y -= 10;
                }
                draw_distance(distancePos, distance, ImColor(0, 250, 0, 250));
            }

        }

        double dx = head2d.x - settings::screen_center_x;
        double dy = head2d.y - settings::screen_center_y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist <= Aimbot::fov && dist < cache::closest_distance)
        {
            cache::closest_distance = dist;
            cache::closest_mesh = mesh;
        }

        bool is_current_target = (mesh == cache::closest_mesh && cache::closest_mesh != NULL && cache::closest_distance <= Aimbot::fov);

        if (is_current_target)
        {
            std::string target_str = "[TARGET]";

            float standard_font_size = 18.0f;
            float boxCenterX = bottom2d.x;

            ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(standard_font_size, FLT_MAX, 0.0f, target_str.c_str(), nullptr);
            const float yOffset = 30.0f;
            ImVec2 pos(boxCenterX - text_size.x * 0.5f, bottom2d.y + yOffset);

            ImU32 textColor = IM_COL32(0, 128, 128, 250);

            ImDrawList* draw_list = ImGui::GetForegroundDrawList();
            draw_list->AddText(ImGui::GetFont(), standard_font_size, ImVec2(pos.x - 1, pos.y), IM_COL32(0, 0, 0, 255), target_str.c_str());
            draw_list->AddText(ImGui::GetFont(), standard_font_size, ImVec2(pos.x + 1, pos.y), IM_COL32(0, 0, 0, 255), target_str.c_str());
            draw_list->AddText(ImGui::GetFont(), standard_font_size, ImVec2(pos.x, pos.y - 1), IM_COL32(0, 0, 0, 255), target_str.c_str());
            draw_list->AddText(ImGui::GetFont(), standard_font_size, ImVec2(pos.x, pos.y + 1), IM_COL32(0, 0, 0, 255), target_str.c_str());
            draw_list->AddText(ImGui::GetFont(), standard_font_size, pos, textColor, target_str.c_str());
        }


    }

    if (Aimbot::enable)
    {
        if (GetAsyncKeyState(hotkeys::aimkey)) aimbot(cache::closest_mesh);
    }
}

inline void render_menu();
inline WPARAM render_loop()
{
    static RECT old_rc;
    ZeroMemory(&messager, sizeof(MSG));
    while (messager.message != WM_QUIT)
    {
        if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&messager);
            DispatchMessage(&messager);
        }

        if (Window == nullptr) exit(0);

        HWND active_wnd = GetForegroundWindow();
        if (active_wnd == Window)
        {
            HWND target_wnd = GetWindow(active_wnd, GW_HWNDPREV);
            SetWindowPos(my_wnd, target_wnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else
        {
            Window = get_process_wnd(Driver::ProcessID);
            Sleep(250);
        }

        RECT rc = { 0 };
        POINT xy = { 0 };
        rc.left = xy.x;
        rc.top = xy.y;

        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = 1.0f / 60.0f;

        POINT p;
        GetCursorPos(&p);
        io.MousePos.x = (float)(p.x - xy.x);
        io.MousePos.y = (float)(p.y - xy.y);

        if (GetAsyncKeyState(0x1))
        {
            io.MouseDown[0] = true;
            io.MouseClicked[0] = true;
            io.MouseClickedPos[0].x = io.MousePos.x;
            io.MouseClickedPos[0].y = io.MousePos.y;
        }
        else
        {
            io.MouseDown[0] = false;
        }

        if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
        {
            old_rc = rc;
            settings::width = rc.right;
            settings::height = rc.bottom;
            p_params.BackBufferWidth = settings::width;
            p_params.BackBufferHeight = settings::height;
            SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
            p_device->Reset(&p_params);
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Loop();
        render_menu();

        ImGui::EndFrame();

        p_device->SetRenderState(D3DRS_ZENABLE, false);
        p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
        p_device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

        if (p_device->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            p_device->EndScene();
        }

        HRESULT result = p_device->Present(0, 0, 0, 0);
        if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            p_device->Reset(&p_params);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (p_device) p_device->Release();
    if (p_object) p_object->Release();
    DestroyWindow(my_wnd);
    return messager.wParam;
}