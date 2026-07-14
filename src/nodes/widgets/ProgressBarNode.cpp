#include "ProgressBarNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

ProgressBarNode::ProgressBarNode(int id)
    : Node(id, "ProgressBar")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void ProgressBarNode::Draw() {}

void ProgressBarNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("value F %.4f\n", m_Value);
    b.appendf("barSize V %.1f %.1f\n", m_BarSize.x, m_BarSize.y);
    b.appendf("overlay S \"%s\"\n", m_Overlay);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BgColor.x, m_BgColor.y, m_BgColor.z, m_BgColor.w);
    b.appendf("bgHoverColor C %.3f %.3f %.3f %.3f\n", m_BgHoverColor.x, m_BgHoverColor.y, m_BgHoverColor.z, m_BgHoverColor.w);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("borderHoverColor C %.3f %.3f %.3f %.3f\n", m_BorderHoverColor.x, m_BorderHoverColor.y, m_BorderHoverColor.z, m_BorderHoverColor.w);
    b.appendf("borderActiveColor C %.3f %.3f %.3f %.3f\n", m_BorderActiveColor.x, m_BorderActiveColor.y, m_BorderActiveColor.z, m_BorderActiveColor.w);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("fontScale F %.2f\n", m_FontScale);
}
void ProgressBarNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "value") == 0) { sscanf(line, " F %f", &m_Value); }
    else if (strcmp(key, "barSize") == 0) { sscanf(line, " V %f %f", &m_BarSize.x, &m_BarSize.y); }
    else if (strcmp(key, "overlay") == 0) { sscanf(line, " S \"%255[^\"]\"", m_Overlay); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgColor.x, &m_BgColor.y, &m_BgColor.z, &m_BgColor.w); }
    else if (strcmp(key, "bgHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgHoverColor.x, &m_BgHoverColor.y, &m_BgHoverColor.z, &m_BgHoverColor.w); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "borderHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderHoverColor.x, &m_BorderHoverColor.y, &m_BorderHoverColor.z, &m_BorderHoverColor.w); }
    else if (strcmp(key, "borderActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderActiveColor.x, &m_BorderActiveColor.y, &m_BorderActiveColor.z, &m_BorderActiveColor.w); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
}
void ProgressBarNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        ImGui::SliderFloat("Value", &m_Value, 0.0f, 1.0f);
        ImGui::DragFloat2("Size", &m_BarSize.x, 1.0f, 1.0f, 2000.0f);
        if (ImGui::InputText("Overlay", m_Overlay, sizeof(m_Overlay)))
            ;
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Bg Color", &m_BgColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Fill Color", &m_BgHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Hover", &m_BorderHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Active", &m_BorderActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 10.0f);
            ImGui::DragFloat("Rounding", &m_FrameRounding, 0.5f, 0.0f, 20.0f);
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void ProgressBarNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;

    // Find connected AnimationNode
    static auto findAnimNode = [](const Node* self, const RenderContext& cr) -> AnimationNode* {
        int animPinId = -1;
        for (auto& p : self->m_Inputs)
            if (p.type == PinType::Animation) { animPinId = p.id; break; }
        if (animPinId < 0) return nullptr;
        for (auto& l : cr.links) {
            if (l.toPinId != animPinId) continue;
            for (auto& n : cr.nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == l.fromPinId && op.type == PinType::Animation)
                        return dynamic_cast<AnimationNode*>(n.get());
        }
        return nullptr;
    };
    AnimationNode* animNode = findAnimNode(this, ctx);

    if (!m_Initialized) {
        m_Initialized = true;
        m_PrevValue = m_Value;
        m_LastFrameValue = m_Value;
    }

    // Edge detection
    if (fabs(m_Value - m_LastFrameValue) > 0.001f) {
        m_PrevValue = m_LastFrameValue;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameValue = m_Value;
    }

    // Animation progress
    float dur = animNode ? animNode->m_Duration : 0.25f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float from = m_PrevValue;
    float to   = m_Value;
    float animVal = from + (to - from) * t;

    // Colors
    ImVec4 frameBg      = m_CustomStyle ? m_BgColor       : (theme ? theme->m_BgColor  : ImVec4(0.20f,0.22f,0.27f,1.0f));
    ImVec4 fillCol      = m_CustomStyle ? m_BgHoverColor  : (theme ? theme->m_Primary : ImVec4(0.49f,0.42f,1.0f,1.0f));
    ImVec4 textCol      = m_CustomStyle ? m_TextColor     : (theme ? theme->m_TextColor : ImVec4(1,1,1,1));
    float rounding      = m_CustomStyle ? m_FrameRounding : (theme ? theme->m_Rounding : 4.0f);
    float borderSize    = m_CustomStyle ? m_BorderSize    : 0.0f;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 barMin = pos;
    ImVec2 barMax = pos + m_BarSize;

    // Background
    dl->AddRectFilled(barMin, barMax, IM_COL32((int)(frameBg.x*255),(int)(frameBg.y*255),(int)(frameBg.z*255),(int)(frameBg.w*255)), rounding);

    // Fill (animated)
    float fillW = (barMax.x - barMin.x) * ImClamp(animVal, 0.0f, 1.0f);
    if (fillW > 0.0f) {
        ImRect fillRect(barMin, ImVec2(barMin.x + fillW, barMax.y));
        dl->AddRectFilled(fillRect.Min, fillRect.Max, IM_COL32((int)(fillCol.x*255),(int)(fillCol.y*255),(int)(fillCol.z*255),(int)(fillCol.w*255)), rounding);
        // Square off the right side of fill if rounding > 0
        if (rounding > 0 && fillW < m_BarSize.x) {
            dl->AddRectFilled(ImVec2(fillRect.Max.x - rounding, fillRect.Min.y), fillRect.Max, IM_COL32((int)(fillCol.x*255),(int)(fillCol.y*255),(int)(fillCol.z*255),(int)(fillCol.w*255)));
        }
    }

    // Border
    if (borderSize > 0) {
        auto& io = ImGui::GetIO();
        ImRect br(barMin, barMax);
        bool bh = br.Contains(io.MousePos), ba = bh && io.MouseDown[0];
        ImVec4 brd = ba ? (m_CustomStyle ? m_BorderActiveColor : ImVec4(0.4f,0.4f,0.5f,1.0f))
                  : bh ? (m_CustomStyle ? m_BorderHoverColor : ImVec4(0.7f,0.7f,0.8f,1.0f))
                  : (m_CustomStyle ? m_BorderColor : ImVec4(0.5f,0.5f,0.6f,1.0f));
        dl->AddRect(barMin, barMax, IM_COL32((int)(brd.x*255),(int)(brd.y*255),(int)(brd.z*255),(int)(brd.w*255)), rounding, 0, borderSize);
    }

    // Overlay text
    const char* overlay = m_Overlay[0] ? m_Overlay : nullptr;
    if (!overlay) {
        char pctBuf[16];
        snprintf(pctBuf, sizeof(pctBuf), "%d%%", (int)(animVal * 100.0f + 0.5f));
        overlay = pctBuf;
    }
    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
    ImVec2 textSize = ImGui::CalcTextSize(overlay);
    ImVec2 textPos = ImVec2(barMin.x + (m_BarSize.x - textSize.x) * 0.5f, barMin.y + (m_BarSize.y - textSize.y) * 0.5f);
    dl->AddText(textPos, IM_COL32((int)(textCol.x*255),(int)(textCol.y*255),(int)(textCol.z*255),(int)(textCol.w*255)), overlay);
    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);

    // Reserve space
    ImGui::SetCursorScreenPos(pos);
    ImGui::Dummy(m_BarSize);
}

