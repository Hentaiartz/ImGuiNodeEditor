#define GLFW_INCLUDE_NONE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "Core/PreviewRenderer.h"
#include "NodeEditor/NodeEditor.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <cstdio>
#include <cstring>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// â”€â”€â”€ File Dialogs â”€â”€â”€

bool Application::OpenFileDialog(std::string& outPath) {
    char path[4096] = {0};
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Node Graph (*.ng)\0*.ng\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = sizeof(path);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if (GetOpenFileNameA(&ofn)) {
        outPath = path;
        return true;
    }
    return false;
}

bool Application::SaveFileDialog(std::string& outPath) {
    char path[4096] = {0};
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Node Graph (*.ng)\0*.ng\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = sizeof(path);
    ofn.lpstrDefExt = "ng";
    ofn.Flags = OFN_OVERWRITEPROMPT;
    if (GetSaveFileNameA(&ofn)) {
        outPath = path;
        return true;
    }
    return false;
}

// â”€â”€â”€ Config â”€â”€â”€

Config Application::s_Config;
std::string Application::s_ConfigPath;

static const char* KeyChordToString(ImGuiKeyChord chord) {
    // Simple helper: formats a chord to a static buffer
    static char buf[64];
    buf[0] = '\0';
    if (chord & ImGuiMod_Ctrl)    strcat(buf, "Ctrl+");
    if (chord & ImGuiMod_Shift)   strcat(buf, "Shift+");
    if (chord & ImGuiMod_Alt)     strcat(buf, "Alt+");
    if (chord & ImGuiMod_Super)   strcat(buf, "Super+");
    ImGuiKey key = (ImGuiKey)(chord & ~ImGuiMod_Mask_);
    const char* keyName = ImGui::GetKeyName(key);
    if (keyName && keyName[0]) strcat(buf, keyName);
    else if (key == ImGuiKey_Space) strcat(buf, "Space");
    else if (key == ImGuiKey_Delete) strcat(buf, "Del");
    else if (key == ImGuiKey_Escape) strcat(buf, "Esc");
    else if (key == ImGuiKey_Enter) strcat(buf, "Enter");
    else if (key == ImGuiKey_Tab) strcat(buf, "Tab");
    return buf;
}

static ImGuiKeyChord ImGuiKeyChordFromString(const char* s) {
    ImGuiKeyChord chord = ImGuiMod_None;
    const char* p = s;
    // Check modifiers
    if (strncmp(p, "Ctrl+", 5) == 0) { chord |= ImGuiMod_Ctrl; p += 5; }
    else if (strncmp(p, "Shift+", 6) == 0) { chord |= ImGuiMod_Shift; p += 6; }
    else if (strncmp(p, "Alt+", 4) == 0) { chord |= ImGuiMod_Alt; p += 4; }
    if (strncmp(p, "Ctrl+", 5) == 0) { chord |= ImGuiMod_Ctrl; p += 5; }
    if (strncmp(p, "Shift+", 6) == 0) { chord |= ImGuiMod_Shift; p += 6; }
    if (strncmp(p, "Alt+", 4) == 0) { chord |= ImGuiMod_Alt; p += 4; }
    // Map common names
    if (strcmp(p, "Space") == 0) return chord | ImGuiKey_Space;
    if (strcmp(p, "Del") == 0 || strcmp(p, "Delete") == 0) return chord | ImGuiKey_Delete;
    if (strcmp(p, "Esc") == 0) return chord | ImGuiKey_Escape;
    if (strcmp(p, "Enter") == 0) return chord | ImGuiKey_Enter;
    if (strcmp(p, "Tab") == 0) return chord | ImGuiKey_Tab;
    if (strcmp(p, "Backspace") == 0) return chord | ImGuiKey_Backspace;
    // Single letter/number
    if (p[0] && !p[1]) {
        if (p[0] >= 'A' && p[0] <= 'Z') return chord | (ImGuiKey_A + (p[0] - 'A'));
        if (p[0] >= '0' && p[0] <= '9') return chord | (ImGuiKey_0 + (p[0] - '0'));
    }
    // F-keys
    if (p[0] == 'F' && p[1] >= '1' && p[1] <= '9' && !p[2])
        return chord | (ImGuiKey_F1 + (p[1] - '1'));
    if (p[0] == 'F' && p[1] == '1' && p[2] >= '0' && p[2] <= '2' && !p[3])
        return chord | (ImGuiKey_F10 + (p[2] - '0'));
    return ImGuiKey_None;
}

static void WriteVec4(FILE* f, const char* key, const ImVec4& v) {
    fprintf(f, "%s=%.4f %.4f %.4f %.4f\n", key, v.x, v.y, v.z, v.w);
}
static ImVec4 ReadVec4(const char* line) {
    ImVec4 v(0,0,0,1);
    sscanf(line, "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);
    return v;
}

void Application::SaveConfig(const Config& cfg) {
    FILE* f = fopen(s_ConfigPath.c_str(), "w");
    if (!f) return;
    fprintf(f, "[theme]\n");
    #define SV(n) WriteVec4(f, #n, cfg.n)
    SV(windowBg); SV(childBg); SV(text); SV(textDisabled); SV(border);
    SV(frameBg); SV(titleBg); SV(menuBarBg); SV(scrollbarBg);
    SV(button); SV(buttonHovered); SV(buttonActive);
    SV(header); SV(headerHovered); SV(headerActive);
    SV(tabActive); SV(selection); SV(selectionGlow);
    SV(gridFine); SV(gridMajor);
    SV(nodeGlow); SV(nodeGlowInner); SV(nodeBorderSel); SV(nodeBorder); SV(nodeBody);
    SV(headerInput); SV(headerOutput); SV(headerShader); SV(headerConverter);
    SV(headerVector); SV(headerTexture); SV(headerScript);
    SV(pinContainer); SV(pinTheme); SV(pinWidget);
    SV(linkDefault); SV(linkValid); SV(linkInvalid);
    #undef SV
    fprintf(f, "\n[hotkeys]\n");
    for (int i = 0; i < (int)HotkeyAction::Count; i++)
        fprintf(f, "%s=%s\n", cfg.hotkeys[i].name, KeyChordToString(cfg.hotkeys[i].chord));
    fprintf(f, "\n[palette]\n");
    for (auto& c : cfg.savedPalette)
        WriteVec4(f, "color", c);
    fclose(f);
}

void Application::LoadConfig(Config& cfg) {
    FILE* f = fopen(s_ConfigPath.c_str(), "r");
    if (!f) return;
    char line[512];
    int section = 0; // 0=none, 1=theme, 2=hotkeys, 3=palette
    while (fgets(line, sizeof(line), f)) {
        char* nl = strchr(line, '\n'); if (nl) *nl = '\0';
        if (line[0] == '[') {
            if (strstr(line, "theme")) section = 1;
            else if (strstr(line, "hotkeys")) section = 2;
            else if (strstr(line, "palette")) section = 3;
            else section = 0;
            continue;
        }
        char* eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0'; const char* key = line; const char* val = eq + 1;
        if (section == 1) {
            #define LV(n) if (strcmp(key, #n) == 0) { cfg.n = ReadVec4(val); continue; }
            LV(windowBg); LV(childBg); LV(text); LV(textDisabled); LV(border);
            LV(frameBg); LV(titleBg); LV(menuBarBg); LV(scrollbarBg);
            LV(button); LV(buttonHovered); LV(buttonActive);
            LV(header); LV(headerHovered); LV(headerActive);
            LV(tabActive); LV(selection); LV(selectionGlow);
            LV(gridFine); LV(gridMajor);
            LV(nodeGlow); LV(nodeGlowInner); LV(nodeBorderSel); LV(nodeBorder); LV(nodeBody);
            LV(headerInput); LV(headerOutput); LV(headerShader); LV(headerConverter);
            LV(headerVector); LV(headerTexture); LV(headerScript);
            LV(pinContainer); LV(pinTheme); LV(pinWidget);
            LV(linkDefault); LV(linkValid); LV(linkInvalid);
            #undef LV
        } else if (section == 2) {
            for (int i = 0; i < (int)HotkeyAction::Count; i++)
                if (strcmp(key, cfg.hotkeys[i].name) == 0)
                    cfg.hotkeys[i].chord = ImGuiKeyChordFromString(val);
        } else if (section == 3) {
            if (strcmp(key, "color") == 0)
                cfg.savedPalette.push_back(ReadVec4(val));
        }
    }
    fclose(f);
}

bool Application::IsHotkeyPressed(HotkeyAction action) {
    auto& io = ImGui::GetIO();
    if (io.WantTextInput) return false;
    ImGuiKeyChord chord = s_Config.hotkeys[(int)action].chord;
    return ImGui::IsKeyChordPressed(chord, ImGuiInputFlags_None);
}

// Forward declarations
static const char* KeyChordToString(ImGuiKeyChord chord);
static void WriteVec4(FILE* f, const char* key, const ImVec4& v);
static ImVec4 ReadVec4(const char* line);

// â”€â”€â”€ ApplyConfig â”€â”€â”€

void Application::ApplyConfig(const Config& cfg) {
    auto& t = cfg;
    ImVec4* c = ImGui::GetStyle().Colors;
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 6.0f;
    s.FrameRounding = 4.0f;
    s.GrabRounding = 4.0f;
    s.TabRounding = 4.0f;
    s.ChildRounding = 4.0f;
    s.PopupRounding = 4.0f;
    s.ScrollbarSize = 8.0f;
    s.ScrollbarRounding = 4.0f;
    s.WindowPadding = ImVec2(8, 8);
    s.FramePadding = ImVec2(6, 4);
    s.TabBorderSize = 1.0f;

    c[ImGuiCol_Text]                  = t.text;
    c[ImGuiCol_TextDisabled]          = t.textDisabled;
    c[ImGuiCol_WindowBg]              = t.windowBg;
    c[ImGuiCol_ChildBg]               = t.childBg;
    c[ImGuiCol_PopupBg]               = ImVec4(t.windowBg.x, t.windowBg.y, t.windowBg.z, 0.95f);
    c[ImGuiCol_Border]                = t.border;
    c[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    c[ImGuiCol_FrameBg]               = t.frameBg;
    c[ImGuiCol_FrameBgHovered]        = ImVec4(t.frameBg.x*1.3f, t.frameBg.y*1.3f, t.frameBg.z*1.3f, 1.0f);
    c[ImGuiCol_FrameBgActive]         = ImVec4(t.frameBg.x*1.15f, t.frameBg.y*1.15f, t.frameBg.z*1.15f, 1.0f);
    c[ImGuiCol_CheckboxSelectedBg]    = t.frameBg;
    c[ImGuiCol_TitleBg]               = t.titleBg;
    c[ImGuiCol_TitleBgActive]         = t.titleBg;
    c[ImGuiCol_TitleBgCollapsed]      = ImVec4(t.titleBg.x, t.titleBg.y, t.titleBg.z, 0.7f);
    c[ImGuiCol_MenuBarBg]             = t.menuBarBg;
    c[ImGuiCol_ScrollbarBg]           = t.scrollbarBg;
    c[ImGuiCol_ScrollbarGrab]         = t.button;
    c[ImGuiCol_ScrollbarGrabHovered]  = t.buttonHovered;
    c[ImGuiCol_ScrollbarGrabActive]   = t.buttonActive;
    c[ImGuiCol_CheckMark]             = t.selection;
    c[ImGuiCol_CheckboxSelectedBg]    = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    c[ImGuiCol_SliderGrab]            = t.selection;
    c[ImGuiCol_SliderGrabActive]      = ImVec4(t.selection.x*1.1f, t.selection.y*1.1f, t.selection.z*1.1f, 1.0f);
    c[ImGuiCol_Button]                = t.button;
    c[ImGuiCol_ButtonHovered]         = t.buttonHovered;
    c[ImGuiCol_ButtonActive]          = t.buttonActive;
    c[ImGuiCol_Header]                = t.header;
    c[ImGuiCol_HeaderHovered]         = t.headerHovered;
    c[ImGuiCol_HeaderActive]          = t.headerActive;
    c[ImGuiCol_Separator]             = t.border;
    c[ImGuiCol_SeparatorHovered]      = t.headerHovered;
    c[ImGuiCol_SeparatorActive]       = t.headerActive;
    c[ImGuiCol_ResizeGrip]            = t.button;
    c[ImGuiCol_ResizeGripHovered]     = t.buttonHovered;
    c[ImGuiCol_ResizeGripActive]      = t.buttonActive;
    c[ImGuiCol_Tab]                   = t.titleBg;
    c[ImGuiCol_TabHovered]            = t.headerHovered;
    c[ImGuiCol_TabActive]             = t.tabActive;
    c[ImGuiCol_TabUnfocused]          = t.titleBg;
    c[ImGuiCol_TabUnfocusedActive]    = t.tabActive;
    c[ImGuiCol_DockingPreview]        = t.selectionGlow;
    c[ImGuiCol_DockingEmptyBg]        = t.childBg;
    c[ImGuiCol_PlotLines]             = t.text;
    c[ImGuiCol_PlotLinesHovered]      = t.selection;
    c[ImGuiCol_PlotHistogram]         = t.selection;
    c[ImGuiCol_PlotHistogramHovered]  = ImVec4(t.selection.x*1.2f, t.selection.y*1.2f, t.selection.z*1.2f, 1.0f);
    c[ImGuiCol_TextSelectedBg]        = t.selectionGlow;
    c[ImGuiCol_DragDropTarget]        = t.selectionGlow;
    c[ImGuiCol_NavHighlight]          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.4f);
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
}

// â”€â”€â”€ Application â”€â”€â”€

Application::Application() : m_Window(nullptr), m_NodeEditor(nullptr) {
    InitWindow();
    InitImGui();
    m_NodeEditor = new NodeEditor();
}

Application::~Application() {
    delete m_NodeEditor;
    ShutdownImGui();
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Application::InitWindow() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int winW = 1400, winH = 900;
    m_Window = glfwCreateWindow(winW, winH, "Node Editor - UI Designer", nullptr, nullptr);
    if (!m_Window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(1);
    }
    // Center window on screen
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode) {
        int cx = (mode->width - winW) / 2;
        int cy = (mode->height - winH) / 2;
        glfwSetWindowPos(m_Window, cx > 0 ? cx : 0, cy > 0 ? cy : 0);
    }
    glfwMakeContextCurrent(m_Window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        exit(1);
    }
    glfwSwapInterval(1);
}

void Application::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Config path: next to the executable
    char exePath[1024];
    GetModuleFileNameA(NULL, exePath, sizeof(exePath));
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash) *lastSlash = '\0';
    s_ConfigPath = std::string(exePath) + "\\config.cfg";
    LoadConfig(s_Config);
    ApplyConfig(s_Config);

    // Load Inter + Font Awesome merged (TTF only - stb_truetype doesn't support OTF)
    const char* assetDir = ASSETS_DIR;
    char interPath[1024], faPath[1024], faRegPath[1024];
    snprintf(interPath, sizeof(interPath), "%s/fonts/Inter/Inter-VariableFont_opsz,wght.ttf", assetDir);
    snprintf(faPath, sizeof(faPath), "%s/fonts/fa-solid-900.ttf", assetDir);
    snprintf(faRegPath, sizeof(faRegPath), "%s/fonts/fa-regular-400.ttf", assetDir);
    ImFont* mainFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(interPath, 15.0f);
    if (!mainFont)
        mainFont = ImGui::GetIO().Fonts->AddFontDefault();
    // Merge Font Awesome Solid (PUA range)
    static const ImWchar faRange[] = { 0xe000, 0xf8ff, 0 };
    ImFontConfig mergeCfg;
    mergeCfg.MergeMode = true;
    mergeCfg.GlyphMinAdvanceX = 13.0f;
    ImFont* faSolid = ImGui::GetIO().Fonts->AddFontFromFileTTF(faPath, 13.0f, &mergeCfg, faRange);
    // Merge Font Awesome Regular for remaining glyphs
    mergeCfg.MergeMode = true;
    ImFont* faReg = ImGui::GetIO().Fonts->AddFontFromFileTTF(faRegPath, 13.0f, &mergeCfg, faRange);
    if (!faSolid && !faReg) {
        // No icon font loaded, will use plain text
    }

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::ShutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

static void HideDockMenuButtonRecursive(ImGuiDockNode* node) {
    if (!node) return;
    node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
    for (int i = 0; i < 2; i++)
        if (node->ChildNodes[i])
            HideDockMenuButtonRecursive(node->ChildNodes[i]);
}

void Application::SetupDockspace() {
    ImGuiID id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    static bool once = true;
    if (once) {
        once = false;
        ImGui::DockBuilderRemoveNode(id);
        ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton);
        ImGui::DockBuilderSetNodeSize(id, ImGui::GetMainViewport()->Size);

        ImGuiID dockRight, dockLeft, dockMain;
        ImGuiID dockBottom;
        ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.22f, &dockRight, &dockLeft);
        ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.30f, &dockBottom, &dockMain);

        ImGui::DockBuilderDockWindow("Node Editor", dockMain);
        ImGui::DockBuilderDockWindow("Properties", dockRight);
        ImGui::DockBuilderFinish(id);

        // Remove the "Hide Tab Bar" arrow from all dock nodes
        ImGuiDockNode* root = ImGui::DockBuilderGetNode(id);
        HideDockMenuButtonRecursive(root);
    }
}

void Application::RenderFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // â”€â”€â”€ Menubar â”€â”€â”€
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("  New Project", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::NewProject].chord))) {
                m_NodeEditor->NewProject();
                m_CurrentFilePath.clear();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("  Open...", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Open].chord))) {
                std::string path;
                if (OpenFileDialog(path)) {
                    m_NodeEditor->LoadFromFile(path.c_str());
                    m_CurrentFilePath = path;
                }
            }
            if (ImGui::MenuItem("  Save", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Save].chord), false, !m_CurrentFilePath.empty())) {
                m_NodeEditor->SaveToFile(m_CurrentFilePath.c_str());
            }
            if (ImGui::MenuItem("  Save As...", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::SaveAs].chord))) {
                std::string path;
                if (SaveFileDialog(path)) {
                    m_NodeEditor->SaveToFile(path.c_str());
                    m_CurrentFilePath = path;
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("  \u2699  Preferences...")) {
                m_ShowPreferences = true;
                m_PendingConfig = s_Config;
                m_RecordingHotkey = -1;
            }
            if (ImGui::MenuItem("  \u2716  Exit")) {
                glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("  Undo", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Undo].chord)))
                m_NodeEditor->Undo();
            if (ImGui::MenuItem("  Redo", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Redo].chord)))
                m_NodeEditor->Redo();
            ImGui::Separator();
            if (ImGui::MenuItem("  Cut", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Cut].chord)))
                m_NodeEditor->CutSelected();
            if (ImGui::MenuItem("  Copy", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Copy].chord)))
                m_NodeEditor->CopySelected();
            if (ImGui::MenuItem("  Paste", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Paste].chord)))
                m_NodeEditor->PasteClipboard(m_NodeEditor->ScreenToCanvas(ImGui::GetIO().MousePos));
            ImGui::Separator();
            if (ImGui::MenuItem("  Find...", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::Find].chord)))
                m_NodeEditor->OpenSearch();
            if (ImGui::MenuItem("  Select All", KeyChordToString(s_Config.hotkeys[(int)HotkeyAction::SelectAll].chord)))
                m_NodeEditor->SelectAll();
            ImGui::Separator();
            if (ImGui::MenuItem("  Re-Arrange Selected", "Ctrl+R")) m_NodeEditor->AutoArrangeSelected();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("Node Editor", nullptr, &m_ShowNodeEditor);
            ImGui::MenuItem("Properties", nullptr, &m_ShowProperties);
            ImGui::Separator();
            bool snap = m_NodeEditor->SnapToGrid();
            if (ImGui::MenuItem("Snap to Grid", nullptr, &snap))
                m_NodeEditor->SnapToGrid() = snap;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    SetupDockspace();

    if (m_ShowNodeEditor) m_NodeEditor->Draw();

    if (m_ShowProperties) {
        ImGui::Begin("Properties");
        int selId = m_NodeEditor->GetSelectedNodeId();
        static int prevSelId = -1;
        if (selId != prevSelId) { if (prevSelId >= 0) m_NodeEditor->PushUndo(); prevSelId = selId; }
        if (selId >= 0) {
            Node* node = m_NodeEditor->GetNodeById(selId);
            if (node) {
                ImGui::TextColored(ImVec4(0.35f, 0.55f, 0.85f, 1.0f),
                    "%s (%s)", node->GetDisplayLabel(), node->GetTypeName());
                ImGui::Separator();
                node->SetEditor(m_NodeEditor);
                node->DrawProperties();

                int childrenPinId = -1;
                for (auto& pin : node->m_Inputs)
                    if (pin.type == PinType::Container) { childrenPinId = pin.id; break; }
                if (childrenPinId >= 0) {
                    auto& links = m_NodeEditor->GetLinks();
                    struct ChildEntry { Node* n; int linkIdx; };
                    std::vector<ChildEntry> children;
                    for (int i = 0; i < (int)links.size(); i++) {
                        if (links[i].toPinId != childrenPinId) continue;
                        for (auto& nn : m_NodeEditor->GetNodes())
                            for (auto& op : nn->m_Outputs)
                                if (op.id == links[i].fromPinId) {
                                    children.push_back({nn.get(), i}); break;
                                }
                    }
                    if (!children.empty() && ImGui::CollapsingHeader("Children", ImGuiTreeNodeFlags_DefaultOpen)) {
                        for (int i = 0; i < (int)children.size(); i++) {
                            ImGui::PushID(i);
                            bool canUp = (i > 0), canDown = (i < (int)children.size() - 1);
                            if (!canUp) ImGui::BeginDisabled();
                            if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
                                std::swap(links[children[i].linkIdx], links[children[i-1].linkIdx]);
                                std::swap(children[i].linkIdx, children[i-1].linkIdx);
                            }
                            if (!canUp) ImGui::EndDisabled();
                            ImGui::SameLine();
                            if (!canDown) ImGui::BeginDisabled();
                            if (ImGui::ArrowButton("##down", ImGuiDir_Down)) {
                                std::swap(links[children[i].linkIdx], links[children[i+1].linkIdx]);
                                std::swap(children[i].linkIdx, children[i+1].linkIdx);
                            }
                            if (!canDown) ImGui::EndDisabled();
                            ImGui::SameLine();
                            ImGui::Text("%s (%s)", children[i].n->GetDisplayLabel(), children[i].n->GetTypeName());
                            ImGui::PopID();
                        }
                    }
                }
            }
        } else {
            // Check for selected region
            int rid = m_NodeEditor->GetSelectedRegionId();
            if (rid >= 0) {
                auto& regions = m_NodeEditor->GetRegions();
                for (auto& r : regions) {
                    if (r.id == rid) {
                        ImGui::TextColored(ImVec4(0.35f, 0.55f, 0.85f, 1.0f),
                            "Region: %s", r.name.c_str());
                        ImGui::Separator();
                        char buf[128];
                        snprintf(buf, sizeof(buf), "%s", r.name.c_str());
                        if (ImGui::InputText("Name", buf, sizeof(buf)))
                            r.name = buf;
                        ImGui::ColorEdit4("Color", &r.color.x, ImGuiColorEditFlags_AlphaBar);
                        ImGui::Checkbox("Collapsed", &r.collapsed);
                        break;
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(0.4f, 0.42f, 0.5f, 1.0f),
                    "Select a node to edit its properties.");
            }
        }
        ImGui::End();
    }

    // â”€â”€â”€ Live Preview (real ImGui windows) â”€â”€â”€
    // Live Preview (real ImGui windows)
    {
        float savedFontScale = ImGui::GetIO().FontGlobalScale;
        RenderLivePreview(m_NodeEditor->GetNodes(), m_NodeEditor->GetLinks());
        ImGui::GetIO().FontGlobalScale = savedFontScale;
    }
    if (m_ShowPreferences) RenderPreferences();

    // Search widget â€” drawn after all docked windows so it floats on top
    m_NodeEditor->DrawSearchWidget();

    // Force-close any ImGui fallback "Debug" window if it appeared
    // â”€â”€â”€ Status bar overlay â”€â”€â”€
    {
        ImGuiViewport* vp = ImGui::GetMainViewport();
        float barH = 24.0f;
        ImU32 bgCol = IM_COL32(22, 22, 28, 230);
        ImDrawList* dl = ImGui::GetForegroundDrawList(vp);
        ImVec2 p0(vp->Pos.x, vp->Pos.y + vp->Size.y - barH);
        ImVec2 p1(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
        dl->AddRectFilled(p0, p1, bgCol);
        dl->AddLine(p0, ImVec2(p1.x, p0.y), IM_COL32(40, 40, 45, 255), 1.0f);

        auto& t = Application::GetConfig();
        ImU32 textCol = ImGui::ColorConvertFloat4ToU32(ImVec4(t.textDisabled.x, t.textDisabled.y, t.textDisabled.z, 0.7f));

        // Left: file name
        std::string fn = m_CurrentFilePath.empty() ? "Untitled.ng" : m_CurrentFilePath;
        size_t slash = fn.find_last_of("\\/");
        if (slash != std::string::npos) fn = fn.substr(slash + 1);
        dl->AddText(ImVec2(p0.x + 10, p0.y + 4), textCol, fn.c_str());

        // Center: node / link count
        int nodeCount = (int)m_NodeEditor->GetNodes().size();
        int linkCount = (int)m_NodeEditor->GetLinks().size();
        char buf[64];
        snprintf(buf, sizeof(buf), "%d nodes  |  %d links", nodeCount, linkCount);
        float tw = ImGui::CalcTextSize(buf).x;
        dl->AddText(ImVec2(p0.x + (p1.x - p0.x) * 0.5f - tw * 0.5f, p0.y + 4), textCol, buf);

        // Right: FPS
        float fps = ImGui::GetIO().Framerate;
        ImU32 fpsCol = (fps > 50) ? IM_COL32(100, 200, 100, 200) :
                       (fps > 25) ? IM_COL32(220, 200, 80, 200) : IM_COL32(220, 80, 80, 200);
        char fpsBuf[16];
        snprintf(fpsBuf, sizeof(fpsBuf), "%.0f FPS", fps);
        float fpsW = ImGui::CalcTextSize(fpsBuf).x;
        dl->AddText(ImVec2(p1.x - fpsW - 10, p0.y + 4), fpsCol, fpsBuf);
    }

    ImGui::Render();
    int displayW, displayH;
    glfwGetFramebufferSize(m_Window, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    glClearColor(0.08f, 0.08f, 0.10f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GLFWwindow* backup = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup);

    glfwSwapBuffers(m_Window);
}

void Application::RenderPreferences() {
    if (!m_ShowPreferences) return;
    ImGui::SetNextWindowSize(ImVec2(580, 540), ImGuiCond_Appearing);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::OpenPopup("Preferences");
    ImGui::PopStyleVar();
    if (!ImGui::BeginPopupModal("Preferences", &m_ShowPreferences, ImGuiWindowFlags_NoResize)) {
        return;
    }

    Config& p = m_PendingConfig;

    if (ImGui::BeginTabBar("PrefTabs", ImGuiTabBarFlags_FittingPolicyScroll)) {
        // â”€â”€â”€ Hotkeys Tab â”€â”€â”€
        if (ImGui::BeginTabItem("\uE801  Hotkeys")) {
            if (ImGui::BeginChild("HotkeyScroll", ImVec2(0, -36), false)) {
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
                ImGui::Columns(2, "HotkeyCols", false);
                ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.38f);
                for (int i = 0; i < (int)HotkeyAction::Count; i++) {
                    auto& hk = p.hotkeys[i];
                    ImGui::PushID(i);
                    ImGui::Text("%s", hk.name);
                    ImGui::NextColumn();
                    ImU32 btnCol = (m_RecordingHotkey == i)
                        ? IM_COL32(60, 130, 60, 255)
                        : IM_COL32(45, 48, 55, 255);
                    ImGui::PushStyleColor(ImGuiCol_Button, btnCol);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(60, 64, 75, 255));
                    ImGui::PushItemWidth(-1);
                    if (ImGui::Button(KeyChordToString(hk.chord), ImVec2(0, 0))) {
                        m_RecordingHotkey = i;
                    }
                    ImGui::PopItemWidth();
                    ImGui::PopStyleColor(2);

                    if (m_RecordingHotkey == i) {
                        ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), " Press a key...");
                        auto& io = ImGui::GetIO();
                        for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; k++) {
                            if (ImGui::IsKeyPressed((ImGuiKey)k, false)) {
                                ImGuiKeyChord mods = ImGuiMod_None;
                                if (io.KeyCtrl)  mods |= ImGuiMod_Ctrl;
                                if (io.KeyShift) mods |= ImGuiMod_Shift;
                                if (io.KeyAlt)   mods |= ImGuiMod_Alt;
                                if (io.KeySuper) mods |= ImGuiMod_Super;
                                if (k != ImGuiKey_Escape) {
                                    hk.chord = mods | (ImGuiKeyChord)k;
                                }
                                m_RecordingHotkey = -1;
                            }
                        }
                    }
                    ImGui::NextColumn();
                    ImGui::PopID();
                }
                ImGui::Columns(1);
                ImGui::PopStyleVar();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    // â”€â”€â”€ Bottom bar â”€â”€â”€
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 2));
    float bw = 100.0f;
    float winW = ImGui::GetWindowWidth();
    ImGui::SetCursorPosX(winW - bw * 2 - 12);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.45f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.55f, 0.28f, 1.0f));
    if (ImGui::Button("Save", ImVec2(bw, 26))) {
        s_Config = m_PendingConfig;
        ApplyConfig(s_Config);
        SaveConfig(s_Config);
        m_ShowPreferences = false;
    }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(bw, 26))) {
        m_ShowPreferences = false;
    }

    ImGui::EndPopup();
}


void Application::Run() {
    while (!glfwWindowShouldClose(m_Window)) {
        glfwPollEvents();
        RenderFrame();
    }
}
