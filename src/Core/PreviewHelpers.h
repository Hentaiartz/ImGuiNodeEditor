#pragma once
#include <vector>
#include <memory>
#include <imgui.h>
#include "style/ThemeNode.h"

class Node;
struct Link;

struct RenderContext {
    ThemeNode* theme;
    const std::vector<Link>& links;
    const std::vector<std::unique_ptr<Node>>& nodes;
    int channelSplitDepth = 0; // tracks nested ChannelsSplit to prevent crash
};

// RAII helper for ImGui style push/pop — pops automatically on destruction
struct ScopedStyle {
    int vars = 0, colors = 0;
    ~ScopedStyle() { if (vars) ImGui::PopStyleVar(vars); if (colors) ImGui::PopStyleColor(colors); }
    void col(ImGuiCol idx, ImVec4 v) { ImGui::PushStyleColor(idx, v); colors++; }
    void var(ImGuiStyleVar idx, float v) { ImGui::PushStyleVar(idx, v); vars++; }
    void var(ImGuiStyleVar idx, ImVec2 v) { ImGui::PushStyleVar(idx, v); vars++; }
};

namespace RenderHelpers {
    std::vector<Node*> GetChildren(Node* container, const std::vector<Link>& links, const std::vector<std::unique_ptr<Node>>& nodes);
    ThemeNode* FindNodeTheme(Node* node, const std::vector<Link>& links, const std::vector<std::unique_ptr<Node>>& nodes);
    ThemeNode& DefaultTheme();
    void PushWidgetStyle(ThemeNode* t);
    void PushContainerStyle(ThemeNode* t);
    void PushTextStyle(ThemeNode* t);
    void PopStyle(const ThemeNode* t, int vars, int colors);
    float GetWidgetWidth(Node* node, const RenderContext* ctx = nullptr);
    void ApplyWidgetAlign(int align, float width, ImVec2 offset);
}
