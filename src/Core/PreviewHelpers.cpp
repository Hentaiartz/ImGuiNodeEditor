#include "PreviewHelpers.h"
#include "NodeEditor/Node.h"
#include "style/ThemeNode.h"
#include <imgui.h>

namespace RenderHelpers {

std::vector<Node*> GetChildren(Node* container, const std::vector<Link>& links, const std::vector<std::unique_ptr<Node>>& nodes) {
    std::vector<Node*> children;
    for (auto& pin : container->m_Inputs) {
        if (pin.type != PinType::Container) continue;
        for (auto& link : links) {
            if (link.toPinId != pin.id) continue;
            for (auto& n : nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == link.fromPinId) children.push_back(n.get());
        }
    }
    return children;
}

ThemeNode* FindNodeTheme(Node* node, const std::vector<Link>& links, const std::vector<std::unique_ptr<Node>>& nodes) {
    for (auto& pin : node->m_Inputs) {
        if (pin.type != PinType::Theme) continue;
        for (auto& link : links) {
            if (link.toPinId != pin.id) continue;
            for (auto& n : nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == link.fromPinId) return dynamic_cast<ThemeNode*>(n.get());
        }
    }
    return nullptr;
}

ThemeNode& DefaultTheme() {
    static ThemeNode dt(-1);
    dt.m_Primary      = ImVec4(0.22f, 0.35f, 0.55f, 1.0f);
    dt.m_PrimaryHover = ImVec4(0.28f, 0.42f, 0.65f, 1.0f);
    dt.m_BgColor      = ImVec4(0.12f, 0.14f, 0.18f, 1.0f);
    dt.m_TitleBg      = ImVec4(0.09f, 0.10f, 0.14f, 1.0f);
    dt.m_TextColor    = ImVec4(0.82f, 0.84f, 0.88f, 1.0f);
    dt.m_BorderColor  = ImVec4(0.25f, 0.28f, 0.38f, 1.0f);
    dt.m_FontSize     = 14.0f;
    dt.m_Rounding     = 6.0f;
    dt.m_Spacing      = 8.0f;
    dt.m_BorderWidth  = 1.5f;
    return dt;
}

void PushWidgetStyle(ThemeNode* t) {
    if (!t) return;
    ImGui::PushStyleColor(ImGuiCol_Button,        t->m_Primary);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  t->m_PrimaryHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,   t->m_PrimaryActive);
    ImGui::PushStyleColor(ImGuiCol_Text,           t->m_TextColor);
    ImGui::PushStyleColor(ImGuiCol_CheckMark,      t->m_CheckMark);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,     t->m_SliderGrab);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, t->m_SliderGrabActive);
    ImGui::PushStyleColor(ImGuiCol_Separator,      t->m_Separator);
    ImGui::PushStyleColor(ImGuiCol_Border,          t->m_BorderColor);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, t->m_Rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  ImVec2(t->m_Spacing*0.5f, t->m_Spacing*0.3f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, t->m_BorderWidth);
}

void PushContainerStyle(ThemeNode* t) {
    if (!t) return;
    ImGui::PushStyleColor(ImGuiCol_WindowBg,       t->m_WindowBg);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,         t->m_BgColor);
    ImGui::PushStyleColor(ImGuiCol_TitleBg,         t->m_TitleBg);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive,   t->m_TitleBg);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg,       t->m_TitleBg);
    ImGui::PushStyleColor(ImGuiCol_Text,            t->m_TextColor);
    ImGui::PushStyleColor(ImGuiCol_Tab,             t->m_Tab);
    ImGui::PushStyleColor(ImGuiCol_TabActive,       t->m_TabActive);
    ImGui::PushStyleColor(ImGuiCol_TabHovered,      t->m_TabHovered);
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,     t->m_ScrollbarBg);
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,   t->m_ScrollbarGrab);
    ImGui::PushStyleColor(ImGuiCol_PopupBg,         t->m_PopupBg);
    ImGui::PushStyleColor(ImGuiCol_TextDisabled,    t->m_DisabledText);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, t->m_Rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, t->m_Rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(t->m_Spacing, t->m_Spacing));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(t->m_Spacing*0.5f, t->m_Spacing*0.3f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, t->m_BorderWidth);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, t->m_ScrollbarSize);
}

void PushTextStyle(ThemeNode* t) {
    if (!t) return;
    ImGui::PushStyleColor(ImGuiCol_Text, t->m_TextColor);
}

void PopStyle(const ThemeNode* t, int vars, int colors) {
    if (!t) return;
    if (vars > 0) ImGui::PopStyleVar(vars);
    if (colors > 0) ImGui::PopStyleColor(colors);
}

float GetWidgetWidth(Node* node, const RenderContext* ctx) {
    return node->GetWidgetWidth(ctx);
}

void ApplyWidgetAlign(int align, float width, ImVec2 offset) {
    float avail = ImGui::GetContentRegionAvail().x;
    if (align == 1 && avail > width + 10)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - width) * 0.5f);
    else if (align == 2 && avail > width + 10)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - width);
    if (offset.x != 0 || offset.y != 0)
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offset.x, ImGui::GetCursorPosY() + offset.y));
}

} // namespace RenderHelpers
