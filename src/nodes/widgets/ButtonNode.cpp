#include "ButtonNode.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>

ButtonNode::ButtonNode(int id)
    : Node(id, "Button")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void ButtonNode::Draw() {}

void ButtonNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("buttonSize V %.1f %.1f\n", m_ButtonSize.x, m_ButtonSize.y);
    b.appendf("disabled B %d\n", (int)m_Disabled);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BgColor.x, m_BgColor.y, m_BgColor.z, m_BgColor.w);
    b.appendf("bgHoverColor C %.3f %.3f %.3f %.3f\n", m_BgHoverColor.x, m_BgHoverColor.y, m_BgHoverColor.z, m_BgHoverColor.w);
    b.appendf("bgActiveColor C %.3f %.3f %.3f %.3f\n", m_BgActiveColor.x, m_BgActiveColor.y, m_BgActiveColor.z, m_BgActiveColor.w);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("borderHoverColor C %.3f %.3f %.3f %.3f\n", m_BorderHoverColor.x, m_BorderHoverColor.y, m_BorderHoverColor.z, m_BorderHoverColor.w);
    b.appendf("borderActiveColor C %.3f %.3f %.3f %.3f\n", m_BorderActiveColor.x, m_BorderActiveColor.y, m_BorderActiveColor.z, m_BorderActiveColor.w);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("fontScale F %.2f\n", m_FontScale);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
}
void ButtonNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "buttonSize") == 0) { sscanf(line, " V %f %f", &m_ButtonSize.x, &m_ButtonSize.y); }
    else if (strcmp(key, "disabled") == 0) { int v; sscanf(line, " B %d", &v); m_Disabled = v != 0; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgColor.x, &m_BgColor.y, &m_BgColor.z, &m_BgColor.w); }
    else if (strcmp(key, "bgHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgHoverColor.x, &m_BgHoverColor.y, &m_BgHoverColor.z, &m_BgHoverColor.w); }
    else if (strcmp(key, "bgActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgActiveColor.x, &m_BgActiveColor.y, &m_BgActiveColor.z, &m_BgActiveColor.w); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "borderHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderHoverColor.x, &m_BorderHoverColor.y, &m_BorderHoverColor.z, &m_BorderHoverColor.w); }
    else if (strcmp(key, "borderActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderActiveColor.x, &m_BorderActiveColor.y, &m_BorderActiveColor.z, &m_BorderActiveColor.w); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
}
void ButtonNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
        ImGui::DragFloat2("Size", &m_ButtonSize.x, 1.0f, 20.0f, 800.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Settings")) {
        ImGui::Checkbox("Disabled", &m_Disabled);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Bg Color",      &m_BgColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Hover",      &m_BgHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Active",     &m_BgActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Color",  &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Hover",  &m_BorderHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Active", &m_BorderActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Border Size",    &m_BorderSize, 0.5f, 0.0f, 10.0f);
            ImGui::DragFloat("Rounding",       &m_FrameRounding, 0.5f, 0.0f, 20.0f);
            ImGui::DragFloat("Font Scale",      &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::DragFloat2("Frame Padding",   &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
            ImGui::ColorEdit4("Text Color",    &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void ButtonNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;
    if (m_CustomStyle) {
        ScopedStyle s;
        s.col(ImGuiCol_Button, m_BgColor);
        s.col(ImGuiCol_ButtonHovered, m_BgHoverColor);
        s.col(ImGuiCol_ButtonActive, m_BgActiveColor);
        s.col(ImGuiCol_Text, m_TextColor);
        s.var(ImGuiStyleVar_FrameRounding, m_FrameRounding);
        s.var(ImGuiStyleVar_FrameBorderSize, m_BorderSize);
        if (m_FramePadding.x > 0 || m_FramePadding.y > 0) s.var(ImGuiStyleVar_FramePadding, m_FramePadding);
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
        if (m_BorderSize > 0) {
            auto& io = ImGui::GetIO();
            ImVec2 bp = ImGui::GetCursorScreenPos();
            ImRect br(bp, bp + m_ButtonSize);
            bool bh = br.Contains(io.MousePos), ba = bh && io.MouseDown[0];
            s.col(ImGuiCol_Border, ba ? m_BorderActiveColor : bh ? m_BorderHoverColor : m_BorderColor);
        }
        if (m_Disabled) ImGui::BeginDisabled();
        ImGui::Button(m_Label.empty() ? "##btn" : m_Label.c_str(), m_ButtonSize);
        if (m_Disabled) ImGui::EndDisabled();
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
    } else {
        RenderHelpers::PushWidgetStyle(wt);
        if (m_Disabled) ImGui::BeginDisabled();
        ImGui::Button(m_Label.empty() ? "##btn" : m_Label.c_str(), m_ButtonSize);
        if (m_Disabled) ImGui::EndDisabled();
        RenderHelpers::PopStyle(wt, 3, 9);
    }
}


