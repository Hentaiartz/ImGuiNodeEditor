#include "SliderNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

SliderNode::SliderNode(int id)
    : Node(id, "Slider")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void SliderNode::Draw() {}

void SliderNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("value F %.4f\n", m_Value);
    b.appendf("min F %.4f\n", m_Min);
    b.appendf("max F %.4f\n", m_Max);
    b.appendf("format S \"%s\"\n", m_Format.c_str());
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("width F %.1f\n", m_Width);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BgColor.x, m_BgColor.y, m_BgColor.z, m_BgColor.w);
    b.appendf("bgHoverColor C %.3f %.3f %.3f %.3f\n", m_BgHoverColor.x, m_BgHoverColor.y, m_BgHoverColor.z, m_BgHoverColor.w);
    b.appendf("bgActiveColor C %.3f %.3f %.3f %.3f\n", m_BgActiveColor.x, m_BgActiveColor.y, m_BgActiveColor.z, m_BgActiveColor.w);
    b.appendf("borderHoverColor C %.3f %.3f %.3f %.3f\n", m_BorderHoverColor.x, m_BorderHoverColor.y, m_BorderHoverColor.z, m_BorderHoverColor.w);
    b.appendf("borderActiveColor C %.3f %.3f %.3f %.3f\n", m_BorderActiveColor.x, m_BorderActiveColor.y, m_BorderActiveColor.z, m_BorderActiveColor.w);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("fontScale F %.2f\n", m_FontScale);
}
void SliderNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "value") == 0) { sscanf(line, " F %f", &m_Value); }
    else if (strcmp(key, "min") == 0) { sscanf(line, " F %f", &m_Min); }
    else if (strcmp(key, "max") == 0) { sscanf(line, " F %f", &m_Max); }
    else if (strcmp(key, "format") == 0) { char v[32]; if (sscanf(line, " S \"%31[^\"]\"", v) >= 1) m_Format = v; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "width") == 0) { sscanf(line, " F %f", &m_Width); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgColor.x, &m_BgColor.y, &m_BgColor.z, &m_BgColor.w); }
    else if (strcmp(key, "bgHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgHoverColor.x, &m_BgHoverColor.y, &m_BgHoverColor.z, &m_BgHoverColor.w); }
    else if (strcmp(key, "bgActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgActiveColor.x, &m_BgActiveColor.y, &m_BgActiveColor.z, &m_BgActiveColor.w); }
    else if (strcmp(key, "borderHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderHoverColor.x, &m_BorderHoverColor.y, &m_BorderHoverColor.z, &m_BorderHoverColor.w); }
    else if (strcmp(key, "borderActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderActiveColor.x, &m_BorderActiveColor.y, &m_BorderActiveColor.z, &m_BorderActiveColor.w); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
}
void SliderNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
        ImGui::DragFloat("Min", &m_Min, 0.1f, -10000.0f, m_Max);
        ImGui::DragFloat("Max", &m_Max, 0.1f, m_Min, 10000.0f);
        char fmtBuf[32];
        snprintf(fmtBuf, sizeof(fmtBuf), "%s", m_Format.c_str());
        if (ImGui::InputText("Format", fmtBuf, sizeof(fmtBuf)))
            m_Format = fmtBuf;
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
        ImGui::DragFloat("Width (0=auto)", &m_Width, 1.0f, 0.0f, 2000.0f);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Bg Color", &m_BgColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Hover", &m_BgHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Active", &m_BgActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Hover", &m_BorderHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Active", &m_BorderActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 10.0f);
            ImGui::DragFloat("Rounding", &m_FrameRounding, 0.5f, 0.0f, 20.0f);
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void SliderNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;

    // Find connected AnimationNode
    static auto findAnimNode = [](const Node* self, const RenderContext& ctx) -> AnimationNode* {
        int animPinId = -1;
        for (auto& p : self->m_Inputs)
            if (p.type == PinType::Animation) { animPinId = p.id; break; }
        if (animPinId < 0) return nullptr;
        for (auto& l : ctx.links) {
            if (l.toPinId != animPinId) continue;
            for (auto& n : ctx.nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == l.fromPinId && op.type == PinType::Animation)
                        return dynamic_cast<AnimationNode*>(n.get());
        }
        return nullptr;
    };
    AnimationNode* animNode = findAnimNode(this, ctx);

    // Init animation state
    if (!m_Initialized) {
        m_Initialized = true;
        m_PrevValue = m_Value;
        m_LastFrameValue = m_Value;
    }

    // Edge detection: value changed
    if (fabs(m_Value - m_LastFrameValue) > 1e-6f) {
        m_PrevValue = m_LastFrameValue;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameValue = m_Value;
    }

    float availW = ImGui::GetContentRegionAvail().x;
    if (m_Width > 0) availW = m_Width;
    float trackH = 6.0f;
    float rounding = m_CustomStyle ? m_FrameRounding : (wt ? wt->m_Rounding : 4.0f);
    if (rounding < 0.5f) rounding = 4.0f;

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Normalized current value (for display)
    float curNorm = (m_Max > m_Min) ? (m_Value - m_Min) / (m_Max - m_Min) : 0.5f;
    if (curNorm < 0.0f) curNorm = 0.0f;
    if (curNorm > 1.0f) curNorm = 1.0f;

    // Animated normalized value
    float prevNorm = (m_Max > m_Min) ? (m_PrevValue - m_Min) / (m_Max - m_Min) : 0.5f;
    if (prevNorm < 0.0f) prevNorm = 0.0f;
    if (prevNorm > 1.0f) prevNorm = 1.0f;

    // Animation t
    float dur = animNode ? animNode->m_Duration : 0.2f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float animNorm = prevNorm + (curNorm - prevNorm) * t;

    // Format value text (show current value, not animated)
    char valBuf[64];
    snprintf(valBuf, sizeof(valBuf), m_Format.c_str(), m_Value);

    // Interaction first — get hover/active state
    float textH = ImGui::GetTextLineHeight();
    float pad = m_CustomStyle ? m_FramePadding.x : 0.0f;
    ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2(pad, pad);
    float trackY = pos.y + textH + 6.0f;
    float totalH = (trackY - pos.y) + trackH + pad;
    ImGui::SetCursorScreenPos(ImVec2(pos.x - pad, pos.y - pad));
    ImGui::InvisibleButton(("##slider" + std::to_string(m_Id)).c_str(), ImVec2(availW + pad * 2, totalH));
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    if (active && ImGui::IsMouseDragging(0)) {
        ImVec2 trackA(pos.x, trackY);
        ImVec2 trackB(pos.x + availW, trackY + trackH);
        float mx = ImGui::GetIO().MousePos.x - trackA.x;
        float mn = mx / (trackB.x - trackA.x);
        if (mn < 0.0f) mn = 0.0f;
        if (mn > 1.0f) mn = 1.0f;
        m_Value = m_Min + mn * (m_Max - m_Min);
    }

    // Colors with state-aware custom style
    ImVec4 txtCol, fillCol, bgCol, borderCol, handleCol;
    if (m_CustomStyle) {
        txtCol    = m_TextColor;
        fillCol   = active ? m_BgActiveColor : (hovered ? m_BgHoverColor : m_BgColor);
        bgCol     = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
        borderCol = active ? m_BorderActiveColor : (hovered ? m_BorderHoverColor : m_BorderColor);
        handleCol = m_TextColor;
    } else {
        txtCol    = wt ? wt->m_TextColor : ImVec4(0.95f, 0.95f, 0.96f, 1.0f);
        fillCol   = wt ? wt->m_Primary : ImVec4(0.49f, 0.44f, 1.0f, 1.0f);
        bgCol     = wt ? ImVec4(wt->m_BgColor.x*1.2f, wt->m_BgColor.y*1.2f, wt->m_BgColor.z*1.2f, 1.0f) : ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
        borderCol = ImVec4(0,0,0,0);
        handleCol = wt ? wt->m_SliderGrab : ImVec4(1,1,1,1);
    }

    float valW = ImGui::CalcTextSize(valBuf).x;
    if (m_CustomStyle && m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
    if (!m_Label.empty() && m_Label[0] != '#')
        dl->AddText(pos, ImGui::ColorConvertFloat4ToU32(txtCol), m_Label.c_str());
    dl->AddText(ImVec2(pos.x + availW - valW, pos.y), ImGui::ColorConvertFloat4ToU32(fillCol), valBuf);
    if (m_CustomStyle && m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);

    // Track position
    ImVec2 trackA(pos.x, trackY);
    ImVec2 trackB(pos.x + availW, trackY + trackH);

    // Normalized value
    float norm = (m_Max > m_Min) ? (m_Value - m_Min) / (m_Max - m_Min) : 0.5f;
    if (norm < 0.0f) norm = 0.0f;
    if (norm > 1.0f) norm = 1.0f;

    // Background track
    ImU32 trackBg = ImGui::ColorConvertFloat4ToU32(bgCol);
    dl->AddRectFilled(trackA, trackB, trackBg, rounding);
    if (m_CustomStyle && m_BorderSize > 0)
        dl->AddRect(trackA, trackB, ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, m_BorderSize);

    // Filled track (animated)
    ImVec2 fillB(trackA.x + (trackB.x - trackA.x) * animNorm, trackB.y);
    if (fillB.x > trackA.x + 2.0f)
        dl->AddRectFilled(trackA, fillB, ImGui::ColorConvertFloat4ToU32(fillCol), rounding);

    // Handle (animated)
    float handleR = 6.0f;
    float handleX = trackA.x + (trackB.x - trackA.x) * animNorm;
    float handleY = trackY + trackH * 0.5f;
    dl->AddCircleFilled(ImVec2(handleX, handleY), handleR, ImGui::ColorConvertFloat4ToU32(handleCol));

    // Advance cursor past the slider
    ImVec2 finalPos(pos.x, trackY + trackH + pad);
    ImGui::SetCursorScreenPos(finalPos);
    ImGui::Dummy(ImVec2(availW, 1));
}



