#include "TextNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>

float TextNode::GetWidgetWidth(const RenderContext*) const {
    return ImGui::CalcTextSize(m_Text.c_str()).x * m_FontScale;
}

TextNode::TextNode(int id)
    : Node(id, "Text")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void TextNode::Draw() {}

void TextNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("text S \"%s\"\n", m_Text.c_str());
    b.appendf("color C %.3f %.3f %.3f %.3f\n", m_Color.x, m_Color.y, m_Color.z, m_Color.w);
    b.appendf("wrapWidth F %.1f\n", m_WrapWidth);
    b.appendf("align I %d\n", m_Align);
    b.appendf("useCustomColor B %d\n", (int)m_UseCustomColor);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("fontScale F %.2f\n", m_FontScale);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BackgroundColor.x, m_BackgroundColor.y, m_BackgroundColor.z, m_BackgroundColor.w);
}
void TextNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "text") == 0) { char v[1024]; if (sscanf(line, " S \"%1023[^\"]\"", v) >= 1) m_Text = v; }
    else if (strcmp(key, "color") == 0) { sscanf(line, " C %f %f %f %f", &m_Color.x, &m_Color.y, &m_Color.z, &m_Color.w); }
    else if (strcmp(key, "wrapWidth") == 0) { sscanf(line, " F %f", &m_WrapWidth); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "useCustomColor") == 0) { int v; sscanf(line, " B %d", &v); m_UseCustomColor = v != 0; }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BackgroundColor.x, &m_BackgroundColor.y, &m_BackgroundColor.z, &m_BackgroundColor.w); }
}
void TextNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "%s", m_Text.c_str());
        if (ImGui::InputTextMultiline("Text", buf, sizeof(buf), ImVec2(0, 60)))
            m_Text = buf;
        int align = m_Align;
        ImGui::Combo("Align", &align, "Left\0Center\0Right\0\0");
        if (align != m_Align) m_Align = align;
    }
    if (Node::PropSection("Position")) {
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Settings")) {
        ImGui::Checkbox("Custom Color", &m_UseCustomColor);
        if (m_UseCustomColor)
            ImGui::ColorEdit4("Color", &m_Color.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.3f, 4.0f);
        ImGui::DragFloat("Wrap width", &m_WrapWidth, 0.5f, 0.0f, 2000.0f);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 20.0f);
            ImGui::DragFloat("Rounding", &m_FrameRounding, 0.5f, 0.0f, 20.0f);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 5.0f);
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
        ImGui::ColorEdit4("Background", &m_BackgroundColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
    }
}

void TextNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;
    if (m_CustomStyle) {
        if (m_UseCustomColor) ImGui::PushStyleColor(ImGuiCol_Text, m_Color);
        else if (wt) ImGui::PushStyleColor(ImGuiCol_Text, wt->m_TextColor);
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
        if (m_WrapWidth > 0) ImGui::PushTextWrapPos(m_WrapWidth);

        if (m_BackgroundColor.w > 0.01f) {
            ImVec2 textPos = ImGui::GetCursorScreenPos();
            ImVec2 textSize = ImGui::CalcTextSize(m_Text.c_str());
            float padH = m_FramePadding.x > 0 ? m_FramePadding.x : 8.0f;
            float padV = m_FramePadding.y > 0 ? m_FramePadding.y : 4.0f;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 bgA(textPos.x - padH, textPos.y - padV);
            ImVec2 bgB(textPos.x + textSize.x + padH, textPos.y + textSize.y + padV);
            dl->AddRectFilled(bgA, bgB, ImGui::ColorConvertFloat4ToU32(m_BackgroundColor), m_FrameRounding);
            if (m_BorderSize > 0)
                dl->AddRect(bgA, bgB, ImGui::ColorConvertFloat4ToU32(m_BorderColor), m_FrameRounding, 0, m_BorderSize);
        }

        ImGui::TextUnformatted(m_Text.c_str());
        if (m_WrapWidth > 0) ImGui::PopTextWrapPos();
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
        if (m_UseCustomColor || wt) ImGui::PopStyleColor();
    } else {
        if (m_UseCustomColor) ImGui::PushStyleColor(ImGuiCol_Text, m_Color);
        else if (wt) ImGui::PushStyleColor(ImGuiCol_Text, wt->m_TextColor);
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
        if (m_WrapWidth > 0) ImGui::PushTextWrapPos(m_WrapWidth);
        ImGui::TextUnformatted(m_Text.c_str());
        if (m_WrapWidth > 0) ImGui::PopTextWrapPos();
        if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
        if (m_UseCustomColor || wt) ImGui::PopStyleColor();
    }
}


