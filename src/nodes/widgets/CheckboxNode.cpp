#include "CheckboxNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

float CheckboxNode::GetWidgetWidth(const RenderContext*) const {
    return ImGui::CalcTextSize(m_Label.c_str()).x + ImGui::GetFontSize() * 0.8f * m_Scale + 10;
}

CheckboxNode::CheckboxNode(int id)
    : Node(id, "Checkbox")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void CheckboxNode::Draw() {}

void CheckboxNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("checked B %d\n", (int)m_Checked);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("scale F %.2f\n", m_Scale);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("borderHoverColor C %.3f %.3f %.3f %.3f\n", m_BorderHoverColor.x, m_BorderHoverColor.y, m_BorderHoverColor.z, m_BorderHoverColor.w);
    b.appendf("borderActiveColor C %.3f %.3f %.3f %.3f\n", m_BorderActiveColor.x, m_BorderActiveColor.y, m_BorderActiveColor.z, m_BorderActiveColor.w);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BgColor.x, m_BgColor.y, m_BgColor.z, m_BgColor.w);
    b.appendf("bgHoverColor C %.3f %.3f %.3f %.3f\n", m_BgHoverColor.x, m_BgHoverColor.y, m_BgHoverColor.z, m_BgHoverColor.w);
    b.appendf("bgActiveColor C %.3f %.3f %.3f %.3f\n", m_BgActiveColor.x, m_BgActiveColor.y, m_BgActiveColor.z, m_BgActiveColor.w);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("fontScale F %.2f\n", m_FontScale);
}
void CheckboxNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "checked") == 0) { int v; sscanf(line, " B %d", &v); m_Checked = v != 0; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "scale") == 0) { sscanf(line, " F %f", &m_Scale); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "borderHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderHoverColor.x, &m_BorderHoverColor.y, &m_BorderHoverColor.z, &m_BorderHoverColor.w); }
    else if (strcmp(key, "borderActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderActiveColor.x, &m_BorderActiveColor.y, &m_BorderActiveColor.z, &m_BorderActiveColor.w); }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgColor.x, &m_BgColor.y, &m_BgColor.z, &m_BgColor.w); }
    else if (strcmp(key, "bgHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgHoverColor.x, &m_BgHoverColor.y, &m_BgHoverColor.z, &m_BgHoverColor.w); }
    else if (strcmp(key, "bgActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgActiveColor.x, &m_BgActiveColor.y, &m_BgActiveColor.z, &m_BgActiveColor.w); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
}
void CheckboxNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Settings")) {
        ImGui::Checkbox("Checked", &m_Checked);
        ImGui::DragFloat("Width", &m_Scale, 0.1f, 0.5f, 4.0f, "%.1fx");
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
            ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void CheckboxNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;

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

    if (!m_Initialized) {
        m_Initialized = true;
        m_PrevChecked = m_Checked;
        m_LastFrameChecked = m_Checked;
    }

    // Edge detection
    if (m_Checked != m_LastFrameChecked) {
        m_PrevChecked = m_LastFrameChecked;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameChecked = m_Checked;
    }

    // Animation progress
    float dur = animNode ? animNode->m_Duration : 0.2f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float from = m_PrevChecked ? 1.0f : 0.0f;
    float to   = m_Checked ? 1.0f : 0.0f;
    float animNorm = from + (to - from) * t; // 0 = unchecked, 1 = checked

    float fs = ImGui::GetFontSize();
    float checkSize = fs * 0.8f * m_Scale;
    float pad = m_CustomStyle ? m_FramePadding.x : 0.0f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 cbMin = pos + ImVec2(pad, pad);
    ImVec2 cbMax = cbMin + ImVec2(checkSize, checkSize);

    // Interaction first — get hover/active state for drawing
    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(m_Label.c_str(), ImVec2(checkSize + pad * 2, checkSize + pad * 2));
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    if (ImGui::IsItemClicked()) m_Checked = !m_Checked;

    // Colors with state-aware custom style
    ImVec4 bgCol, borderCol, checkMark, textCol;
    float rounding;
    if (m_CustomStyle) {
        bgCol     = active ? m_BgActiveColor : (hovered ? m_BgHoverColor : m_BgColor);
        borderCol = active ? m_BorderActiveColor : (hovered ? m_BorderHoverColor : m_BorderColor);
        checkMark = m_TextColor;
        textCol   = m_TextColor;
        rounding  = m_FrameRounding;
    } else {
        bgCol     = theme ? ImVec4(theme->m_BgColor.x, theme->m_BgColor.y, theme->m_BgColor.z, 1.0f) : ImVec4(0.16f,0.16f,0.20f,1.0f);
        borderCol = ImVec4(0,0,0,0);
        checkMark = theme ? theme->m_CheckMark : ImVec4(0.85f,0.85f,0.92f,1.0f);
        textCol   = theme ? theme->m_TextColor : ImVec4(0.85f,0.85f,0.92f,1.0f);
        rounding  = theme ? theme->m_Rounding : 4.0f;
    }

    // Checkbox square
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(cbMin, cbMax, IM_COL32(0,0,0,30), rounding);
    dl->AddRectFilled(cbMin, cbMax, IM_COL32((int)(bgCol.x*255),(int)(bgCol.y*255),(int)(bgCol.z*255),(int)(bgCol.w*255)), rounding);
    if (m_CustomStyle && m_BorderSize > 0)
        dl->AddRect(cbMin, cbMax, IM_COL32((int)(borderCol.x*255),(int)(borderCol.y*255),(int)(borderCol.z*255),(int)(borderCol.w*255)), rounding, 0, m_BorderSize);

    // Animated checkmark ✓
    if (animNorm > 0.01f) {
        ImU32 cmCol = IM_COL32((int)(checkMark.x*255),(int)(checkMark.y*255),(int)(checkMark.z*255),(int)(checkMark.w*255*animNorm));
        float w = cbMax.x - cbMin.x;
        float h = cbMax.y - cbMin.y;
        float cx = cbMin.x, cy = cbMin.y;
        // Checkmark control points
        ImVec2 p1 = ImVec2(cx + w*0.22f, cy + h*0.52f);
        ImVec2 p2 = ImVec2(cx + w*0.40f, cy + h*0.72f);
        ImVec2 p3 = ImVec2(cx + w*0.78f, cy + h*0.28f);
        // Animate: draw progressive segments
        float len12 = sqrtf((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y));
        float len23 = sqrtf((p3.x-p2.x)*(p3.x-p2.x)+(p3.y-p2.y)*(p3.y-p2.y));
        float total = len12 + len23;
        float drawLen = total * animNorm;
        if (drawLen <= len12) {
            float frac = drawLen / len12;
            ImVec2 mp = ImVec2(p1.x + (p2.x-p1.x)*frac, p1.y + (p2.y-p1.y)*frac);
            dl->AddLine(p1, mp, cmCol, 2.0f);
        } else {
            dl->AddLine(p1, p2, cmCol, 2.0f);
            float frac = (drawLen - len12) / len23;
            ImVec2 mp = ImVec2(p2.x + (p3.x-p2.x)*frac, p2.y + (p3.y-p2.y)*frac);
            dl->AddLine(p2, mp, cmCol, 2.0f);
        }
    }

    // Label
    if (!m_Label.empty() && m_Label[0] != '#') {
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        float textScale = 1.0f + (m_Scale - 1.0f) * 0.5f;
        if (m_CustomStyle) textScale *= m_FontScale;
        if (textScale != 1.0f) ImGui::SetWindowFontScale(textScale);
        ImGui::PushStyleColor(ImGuiCol_Text, textCol);
        ImGui::TextUnformatted(m_Label.c_str());
        ImGui::PopStyleColor();
        if (textScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
    }
}



