#include "SeparatorNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>

SeparatorNode::SeparatorNode(int id)
    : Node(id, "Separator")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 50.0f);
}

void SeparatorNode::Draw() {}

void SeparatorNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("thickness F %.1f\n", m_Thickness);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("separatorColor C %.3f %.3f %.3f %.3f\n", m_Color.x, m_Color.y, m_Color.z, m_Color.w);
    b.appendf("useCustomSeparatorColor B %d\n", (int)m_UseCustomColor);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
}
void SeparatorNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "thickness") == 0) { sscanf(line, " F %f", &m_Thickness); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "separatorColor") == 0) { sscanf(line, " C %f %f %f %f", &m_Color.x, &m_Color.y, &m_Color.z, &m_Color.w); }
    else if (strcmp(key, "useCustomSeparatorColor") == 0) { int v; sscanf(line, " B %d", &v); m_UseCustomColor = v != 0; }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
}
void SeparatorNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        ImGui::DragFloat("Thickness", &m_Thickness, 0.1f, 0.0f, 20.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Settings")) {
        ImGui::Checkbox("Custom Color", &m_UseCustomColor);
        if (m_UseCustomColor)
            ImGui::ColorEdit4("Color", &m_Color.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 20.0f);
            ImGui::DragFloat("Rounding", &m_FrameRounding, 0.5f, 0.0f, 20.0f);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 5.0f);
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void SeparatorNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;
    if (m_CustomStyle) {
        ScopedStyle s;
        if (m_FramePadding.x > 0 || m_FramePadding.y > 0) s.var(ImGuiStyleVar_FramePadding, m_FramePadding);
        if (m_FrameRounding > 0) s.var(ImGuiStyleVar_FrameRounding, m_FrameRounding);
        if (m_UseCustomColor) s.col(ImGuiCol_Separator, m_Color);
        if (m_BorderSize > 0) {
            s.col(ImGuiCol_Border, m_BorderColor);
            s.var(ImGuiStyleVar_FrameBorderSize, m_BorderSize);
        }
        ImGui::Separator();
    } else {
        if (m_UseCustomColor) ImGui::PushStyleColor(ImGuiCol_Separator, m_Color);
        else ImGui::PushStyleColor(ImGuiCol_Separator, theme ? theme->m_Separator : ImVec4(0.18f,0.19f,0.23f,1.0f));
        ImGui::Separator();
        ImGui::PopStyleColor();
    }
}

