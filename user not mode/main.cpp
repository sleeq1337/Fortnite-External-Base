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
#include "main.hpp"
#include "Public/Keybind.hpp"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
static int boxMode = 0;
const char* boxItems[] = { "Off", "Box", "Cornered Box" };

void DrawFOVCircle(ImVec2 center, float radius, float speed = 2.0f)
{
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    const int segments = (int)(2 * 3.14159f * radius); 
    float angle_per = 2 * 3.14159f / segments;
    float t = ImGui::GetTime() * speed;

    for (int i = 0; i < segments; i++)
    {
        float angle = i * angle_per;
        float angle_next = (i + 1) * angle_per;

        ImVec2 p1(center.x + cosf(angle) * radius, center.y + sinf(angle) * radius);
        ImVec2 p2(center.x + cosf(angle_next) * radius, center.y + sinf(angle_next) * radius);

        float hue = fmodf((i / (float)segments) + t, 1.0f);
        ImU32 col = ImColor::HSV(hue, 1.0f, 1.0f);

        draw_list->AddLine(p1, p2, col, 2.0f);
    }
}


void render_menu()
{
    if (Aimbot::show_fov)
        DrawFOVCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), Aimbot::fov, 3.0f);

    if (GetAsyncKeyState(VK_INSERT) & 1)
        settings::show_menu = !settings::show_menu;

    if (!settings::show_menu) return;

    ImGui::SetNextWindowSize(ImVec2(450, 550));
    ImGui::Begin("I Love EasyAnti-Cheat", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    if (ImGui::Button("Aimbot", ImVec2(130, 40))) settings::tab = 0;
    ImGui::SameLine();
    if (ImGui::Button("Visuals", ImVec2(130, 40))) settings::tab = 1;
    ImGui::SameLine();
    if (ImGui::Button("Misc", ImVec2(130, 40))) settings::tab = 2;
    ImGui::PopStyleColor(3);
    ImGui::Separator();

    switch (settings::tab)
    {
    case 0:
        ImGui::Checkbox("Memory Aimbot", &Aimbot::enable);
        ImGui::Checkbox("Show FOV", &Aimbot::show_fov);
        ImGui::SliderFloat("FOV", &Aimbot::fov, 25, 1336, "%.0f");
        ImGui::SliderFloat("Smoothness", &Aimbot::smoothness, 1, 10, "%.0f");
        ImGui::Text("Aimkey:");
        HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);
        break;

    case 1:
        ImGui::Checkbox("Enable Visuals", &visuals::enable);
        if (ImGui::Combo("Box Type", &boxMode, boxItems, IM_ARRAYSIZE(boxItems)))
        {
            visuals::Box = (boxMode == 1);
            visuals::CorneredBox = (boxMode == 2);
        }

        ImGui::Checkbox("Line", &visuals::line);
        ImGui::Checkbox("Distance", &visuals::distance);
        break;

    case 2:
        if (ImGui::Button("Unload", ImVec2(150, 40))) exit(0);
        break;
    }

    ImGui::End();
}

int main()
{
    printf("Waiting for Fortnite...");

    while (Window == 0)
    {
        Sleep(1);
        Driver::ProcessID = Driver::FindProcess(L"FortniteClient-Win64-Shipping.exe");
        Window = get_process_wnd(Driver::ProcessID);
        Sleep(1);
    }

    system("cls");

    if (!input::init())
    {
        printf("The input was not initialized :(");
        Sleep(3000);
        return 0;
    }

    if (!gui::init())
    {
        printf("The gui was not initialized :(");
        Sleep(3000);
        return 0;
    }
    if (!Driver::Init())
    {
        printf("The driver was not initialized :(");
        Sleep(3000);
        return 0;
    }

    Base = Driver::GetBase();
    auto cr3 = Driver::CR3();

    if (!Driver::DriverHandle)
    {
        printf("The driver couldn't get the base address");
        Sleep(3000);
        return 0;
    }

    create_overlay();
    DX_Init();
    render_loop();

    return 0;
}
