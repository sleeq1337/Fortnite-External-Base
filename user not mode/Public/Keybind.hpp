#pragma once
#include <string>
#include <string.h>
#include <processthreadsapi.h>
#include "../imgui/imgui.h"
#include <WinUser.h>

#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace hotkeys
{
    int aimkey;
    int airstuckey;
}

static int keystatus = 0;
static int realkey = 0;

bool GetKey(int key)
{
    realkey = key;
    return true;
}
void ChangeKey(void* blank)
{
    keystatus = 1;
    while (true)
    {
        for (int i = 0; i < 0x87; i++)
        {
            if (GetKeyState(i) & 0x8000)
            {
                hotkeys::aimkey = i;
                keystatus = 0;
                return;
            }
        }
    }
}


static const char* keyNames[256] =
{
    "No Key Selected",       // 0x00
    "Left Mouse",            // 0x01
    "Right Mouse",           // 0x02
    "Control-break Processing", // 0x03
    "Middle Mouse",          // 0x04
    "Mouse Button 4",        // 0x05
    "Mouse Button 5",        // 0x06
    "",                      // 0x07 - undefined
    "Backspace",             // 0x08
    "Tab",                   // 0x09
    "", "",                  // 0x0A–0x0B
    "Clear",                 // 0x0C
    "Enter",                 // 0x0D
    "", "",                  // 0x0E–0x0F
    "Shift",                 // 0x10
    "Control",               // 0x11
    "Alt",                   // 0x12
    "Pause",                 // 0x13
    "Caps Lock",             // 0x14
    "", "", "", "", "", "",  // 0x15–0x1A
    "Escape",                // 0x1B
    "", "", "",               // 0x1C–0x1F
    "Space",                 // 0x20
    "Page Up",               // 0x21
    "Page Down",             // 0x22
    "End",                   // 0x23
    "Home",                  // 0x24
    "Left Arrow",            // 0x25
    "Up Arrow",              // 0x26
    "Right Arrow",           // 0x27
    "Down Arrow",            // 0x28
    "Select",                // 0x29
    "Print",                 // 0x2A
    "Execute",               // 0x2B
    "Print Screen",          // 0x2C
    "Insert",                // 0x2D
    "Delete",                // 0x2E
    "Help",                  // 0x2F
    "0", "1", "2", "3", "4", // 0x30–0x34
    "5", "6", "7", "8", "9", // 0x35–0x39
    "", "", "", "", "", "",  // 0x3A–0x40
    "A", "B", "C", "D", "E", // 0x41–0x45
    "F", "G", "H", "I", "J", // 0x46–0x4A
    "K", "L", "M", "N", "O", // 0x4B–0x4F
    "P", "Q", "R", "S", "T", // 0x50–0x54
    "U", "V", "W", "X", "Y", "Z", // 0x55–0x5A
    "Left Windows",          // 0x5B
    "Right Windows",         // 0x5C
    "Applications",          // 0x5D
    "",                      // 0x5E - reserved
    "Sleep",                 // 0x5F
    "Num 0", "Num 1", "Num 2", "Num 3", "Num 4", // 0x60–0x64
    "Num 5", "Num 6", "Num 7", "Num 8", "Num 9", // 0x65–0x69
    "Num *",                 // 0x6A
    "Num +",                 // 0x6B
    "",                      // 0x6C - separator
    "Num -",                 // 0x6D
    "Num .",                 // 0x6E
    "Num /",                 // 0x6F
    "F1", "F2", "F3", "F4", "F5", // 0x70–0x74
    "F6", "F7", "F8", "F9", "F10", // 0x75–0x79
    "F11", "F12", "F13", "F14", "F15", // 0x7A–0x7E
    "F16", "F17", "F18", "F19", "F20", // 0x7F–0x83
    "F21", "F22", "F23", "F24",       // 0x84–0x87
    "", "", "", "", "", "", "", "",   // 0x88–0x8F - unassigned
    "Num Lock",              // 0x90
    "Scroll Lock",           // 0x91
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", // 0x92–0x9F
    "Left Shift",            // 0xA0
    "Right Shift",           // 0xA1
    "Left Control",          // 0xA2
    "Right Control",         // 0xA3
    "Left Menu",             // 0xA4
    "Right Menu",            // 0xA5
    "Browser Back",          // 0xA6
    "Browser Forward",       // 0xA7
    "Browser Refresh",       // 0xA8
    "Browser Stop",          // 0xA9
    "Browser Search",        // 0xAA
    "Browser Favorites",     // 0xAB
    "Browser Home",          // 0xAC
    "Volume Mute",           // 0xAD
    "Volume Down",           // 0xAE
    "Volume Up",             // 0xAF
    "Media Next Track",      // 0xB0
    "Media Prev Track",      // 0xB1
    "Media Stop",            // 0xB2
    "Media Play/Pause",      // 0xB3
    "Launch Mail",           // 0xB4
    "Launch Media Select",   // 0xB5
    "Launch App1",           // 0xB6
    "Launch App2",           // 0xB7
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", // 0xB8–0xC7
    "OEM 1 (;:)",            // 0xBA
    "OEM Plus (+)",          // 0xBB
    "OEM Comma (,)",         // 0xBC
    "OEM Minus (-)",         // 0xBD
    "OEM Period (.)",        // 0xBE
    "OEM 2 (/?)",            // 0xBF
    "OEM 3 (`~)",            // 0xC0
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", // 0xC1–0xCF
    "OEM 4 ([{)",            // 0xDB
    "OEM 5 (\\|)",           // 0xDC
    "OEM 6 (]})",            // 0xDD
    "OEM 7 ('\")",           // 0xDE
    "OEM 8",                 // 0xDF
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", // 0xE0–0xEF
    "OEM 102 (< >)",         // 0xE2
};


static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}
void HotkeyButton(int aimkey, void* changekey, int status)
{
    const char* preview_value = NULL;
    if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
        Items_ArrayGetter(keyNames, aimkey, &preview_value);

    std::string aimkeys;
    if (status == 1)
        aimkeys = "Press the key";
    else if (preview_value == NULL)
        aimkeys = "Select Key";
    else
        aimkeys = preview_value;

    ImVec2 size = ImVec2(140, 28);

    float oldRounding = ImGui::GetStyle().FrameRounding;
    ImGui::GetStyle().FrameRounding = 0.0f; 

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.12f, 1.0f)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.20f, 0.20f, 1.0f)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f)); 

    if (ImGui::Button(aimkeys.c_str(), size))
    {
        if (status == 0)
        {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::GetStyle().FrameRounding = oldRounding;
}