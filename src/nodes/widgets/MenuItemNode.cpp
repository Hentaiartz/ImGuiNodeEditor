#include "MenuItemNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>

MenuItemNode::MenuItemNode(int id)
    : Node(id, "MenuItem")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void MenuItemNode::Draw() {}

void MenuItemNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("shortcut S \"%s\"\n", m_Shortcut.c_str());
    b.appendf("checkable B %d\n", (int)m_Checkable);
    b.appendf("checked B %d\n", (int)m_Checked);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("fontScale F %.2f\n", m_FontScale);
}
void MenuItemNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "shortcut") == 0) { char v[64]; if (sscanf(line, " S \"%63[^\"]\"", v) >= 1) m_Shortcut = v; }
    else if (strcmp(key, "checkable") == 0) { int v; sscanf(line, " B %d", &v); m_Checkable = v != 0; }
    else if (strcmp(key, "checked") == 0) { int v; sscanf(line, " B %d", &v); m_Checked = v != 0; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
}
void MenuItemNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
        char shortcutBuf[64];
        snprintf(shortcutBuf, sizeof(shortcutBuf), "%s", m_Shortcut.c_str());
        if (ImGui::InputText("Shortcut", shortcutBuf, sizeof(shortcutBuf)))
            m_Shortcut = shortcutBuf;
        ImGui::Checkbox("Checkable", &m_Checkable);
        if (m_Checkable)
            ImGui::Checkbox("Checked", &m_Checked);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
        }
    }
}

void MenuItemNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;
    if (m_CustomStyle) {
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
        if (m_Checkable) ImGui::MenuItem(m_Label.c_str(), m_Shortcut.empty() ? nullptr : m_Shortcut.c_str(), &m_Checked);
        else ImGui::MenuItem(m_Label.c_str(), m_Shortcut.empty() ? nullptr : m_Shortcut.c_str());
        ImGui::PopStyleColor();
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
    } else {
        if (wt) ImGui::PushStyleColor(ImGuiCol_Text, wt->m_TextColor);
        if (m_Checkable) ImGui::MenuItem(m_Label.c_str(), m_Shortcut.empty() ? nullptr : m_Shortcut.c_str(), &m_Checked);
        else ImGui::MenuItem(m_Label.c_str(), m_Shortcut.empty() ? nullptr : m_Shortcut.c_str());
        if (wt) ImGui::PopStyleColor();
    }
}
