#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <imgui.h>

class NodeEditor;

// ─── Config ───

enum class HotkeyAction {
    Save, SaveAs, Open, NewProject, Export,
    Copy, Cut, Paste, SelectAll, Delete, Search,
    Undo, Redo, Find,
    Count
};

struct HotkeyBinding {
    const char* name;
    ImGuiKeyChord chord;
};

struct Config {
    // Theme colors (all stored as float[4] for easy editing)
    ImVec4 windowBg       = ImVec4(0.141f, 0.141f, 0.157f, 1.0f);
    ImVec4 childBg        = ImVec4(0.090f, 0.086f, 0.110f, 1.0f);
    ImVec4 text           = ImVec4(0.941f, 0.941f, 0.949f, 1.0f);
    ImVec4 textDisabled   = ImVec4(0.557f, 0.557f, 0.588f, 1.0f);
    ImVec4 border         = ImVec4(0.180f, 0.184f, 0.208f, 1.0f);
    ImVec4 frameBg        = ImVec4(0.161f, 0.165f, 0.188f, 1.0f);
    ImVec4 titleBg        = ImVec4(0.110f, 0.110f, 0.125f, 1.0f);
    ImVec4 menuBarBg      = ImVec4(0.161f, 0.165f, 0.188f, 1.0f);
    ImVec4 scrollbarBg    = ImVec4(0.071f, 0.071f, 0.082f, 1.0f);
    ImVec4 button         = ImVec4(0.200f, 0.204f, 0.231f, 1.0f);
    ImVec4 buttonHovered  = ImVec4(0.271f, 0.275f, 0.310f, 1.0f);
    ImVec4 buttonActive   = ImVec4(0.329f, 0.333f, 0.376f, 1.0f);
    ImVec4 header         = ImVec4(0.392f, 0.361f, 0.702f, 1.0f);
    ImVec4 headerHovered  = ImVec4(0.486f, 0.459f, 0.773f, 1.0f);
    ImVec4 headerActive   = ImVec4(0.545f, 0.522f, 0.804f, 1.0f);
    ImVec4 tabActive      = ImVec4(0.392f, 0.361f, 0.702f, 1.0f);
    ImVec4 selection      = ImVec4(0.545f, 0.486f, 1.000f, 1.0f);
    ImVec4 selectionGlow  = ImVec4(0.545f, 0.486f, 1.000f, 0.30f);
    ImVec4 gridFine       = ImVec4(1.0f, 1.0f, 1.0f, 0.04f);
    ImVec4 gridMajor      = ImVec4(1.0f, 1.0f, 1.0f, 0.08f);
    // Node colors (ImU32 stored as float[4] for editing)
    ImVec4 nodeGlow       = ImVec4(139/255.f, 124/255.f, 255/255.f, 45/255.f);
    ImVec4 nodeGlowInner  = ImVec4(139/255.f, 124/255.f, 255/255.f, 20/255.f);
    ImVec4 nodeBorderSel  = ImVec4(90/255.f, 80/255.f, 170/255.f, 220/255.f);
    ImVec4 nodeBorder     = ImVec4(60/255.f, 62/255.f, 70/255.f, 255/255.f);
    ImVec4 nodeBody       = ImVec4(32/255.f, 33/255.f, 38/255.f, 245/255.f);
    // Headers
    ImVec4 headerInput    = ImVec4(76/255.f, 139/255.f, 210/255.f, 1.0f); // blue
    ImVec4 headerOutput   = ImVec4(217/255.f, 87/255.f, 74/255.f, 1.0f);  // coral
    ImVec4 headerShader   = ImVec4(79/255.f, 168/255.f, 108/255.f, 1.0f); // emerald
    ImVec4 headerConverter= ImVec4(208/255.f, 148/255.f, 59/255.f, 1.0f); // amber
    ImVec4 headerVector   = ImVec4(138/255.f, 127/255.f, 217/255.f, 1.0f);// lavender
    ImVec4 headerTexture  = ImVec4(181/255.f, 102/255.f, 209/255.f, 1.0f);// purple
    ImVec4 headerScript   = ImVec4(114/255.f, 114/255.f, 122/255.f, 1.0f);// gray
    ImVec4 headerContainer= ImVec4(61/255.f, 184/255.f, 179/255.f, 1.0f); // teal
    // Pins
    ImVec4 pinContainer   = ImVec4(55/255.f, 100/255.f, 200/255.f, 1.0f);
    ImVec4 pinTheme       = ImVec4(160/255.f, 60/255.f, 180/255.f, 1.0f);
    ImVec4 pinWidget      = ImVec4(200/255.f, 120/255.f, 40/255.f, 1.0f);
    // Links
    ImVec4 linkDefault    = ImVec4(173/255.f, 173/255.f, 181/255.f, 200/255.f);
    ImVec4 linkValid      = ImVec4(100/255.f, 220/255.f, 100/255.f, 180/255.f);
    ImVec4 linkInvalid    = ImVec4(220/255.f, 80/255.f, 80/255.f, 180/255.f);

    // Hotkeys
    HotkeyBinding hotkeys[(int)HotkeyAction::Count] = {
        {"Save",          ImGuiMod_Ctrl | ImGuiKey_S},
        {"Save As",       ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S},
        {"Open",          ImGuiMod_Ctrl | ImGuiKey_O},
        {"New Project",   ImGuiMod_Ctrl | ImGuiKey_N},
        {"Export Code",   ImGuiMod_Ctrl | ImGuiKey_E},
        {"Copy",          ImGuiMod_Ctrl | ImGuiKey_C},
        {"Cut",           ImGuiMod_Ctrl | ImGuiKey_X},
        {"Paste",         ImGuiMod_Ctrl | ImGuiKey_V},
        {"Select All",    ImGuiMod_Ctrl | ImGuiKey_A},
        {"Delete",        ImGuiKey_Delete},
        {"Search",        ImGuiKey_Space},
        {"Undo",          ImGuiMod_Ctrl | ImGuiKey_Z},
        {"Redo",          ImGuiMod_Ctrl | ImGuiKey_Y},
        {"Find",          ImGuiMod_Ctrl | ImGuiKey_F},
    };

    // Saved palette colors
    std::vector<ImVec4> savedPalette;
};

// ─── Application ───

class Application {
public:
    Application();
    ~Application();
    void Run();
    static Config& GetConfig() { return s_Config; }
    static ImU32 Vec4ToU32(const ImVec4& v) {
        return IM_COL32((int)(v.x*255), (int)(v.y*255), (int)(v.z*255), (int)(v.w*255));
    }
    static bool IsHotkeyPressed(HotkeyAction action);

private:
    GLFWwindow* m_Window;
    NodeEditor* m_NodeEditor;
    std::string m_CurrentFilePath;
    bool m_ShowNodeEditor = true;
    bool m_ShowProperties = true;
    bool m_ShowPreferences = false;
    Config m_PendingConfig;
    int m_RecordingHotkey = -1; // index being recorded, -1 = none

    void InitWindow();
    void InitImGui();
    void ShutdownImGui();
    void RenderFrame();
    void SetupDockspace();
    void RenderPreferences();
    void ApplyConfig(const Config& cfg);
    void SaveConfig(const Config& cfg);
    void LoadConfig(Config& cfg);
    bool OpenFileDialog(std::string& outPath);
    bool SaveFileDialog(std::string& outPath);

    static Config s_Config;
    static std::string s_ConfigPath;
};
