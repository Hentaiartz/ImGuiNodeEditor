#include "ToggleSwitchNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <cmath>

ToggleSwitchNode::ToggleSwitchNode(int id)
    : Node(id, "ToggleSwitch")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void ToggleSwitchNode::Draw() {}

void ToggleSwitchNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("on B %d\n", (int)m_On);
    b.appendf("disabled B %d\n", (int)m_Disabled);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("width F %.1f\n", m_Width);
    b.appendf("scale F %.2f\n", m_Scale);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("accent C %.3f %.3f %.3f %.3f\n", m_Accent.x, m_Accent.y, m_Accent.z, m_Accent.w);
    b.appendf("knobColor C %.3f %.3f %.3f %.3f\n", m_KnobColor.x, m_KnobColor.y, m_KnobColor.z, m_KnobColor.w);
    b.appendf("knobColorOff C %.3f %.3f %.3f %.3f\n", m_KnobColorOff.x, m_KnobColorOff.y, m_KnobColorOff.z, m_KnobColorOff.w);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BgColor.x, m_BgColor.y, m_BgColor.z, m_BgColor.w);
    b.appendf("bgHoverColor C %.3f %.3f %.3f %.3f\n", m_BgHoverColor.x, m_BgHoverColor.y, m_BgHoverColor.z, m_BgHoverColor.w);
    b.appendf("bgActiveColor C %.3f %.3f %.3f %.3f\n", m_BgActiveColor.x, m_BgActiveColor.y, m_BgActiveColor.z, m_BgActiveColor.w);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("borderHoverColor C %.3f %.3f %.3f %.3f\n", m_BorderHoverColor.x, m_BorderHoverColor.y, m_BorderHoverColor.z, m_BorderHoverColor.w);
    b.appendf("borderActiveColor C %.3f %.3f %.3f %.3f\n", m_BorderActiveColor.x, m_BorderActiveColor.y, m_BorderActiveColor.z, m_BorderActiveColor.w);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("fontScale F %.2f\n", m_FontScale);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
}
void ToggleSwitchNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "on") == 0) { int v; sscanf(line, " B %d", &v); m_On = v != 0; }
    else if (strcmp(key, "disabled") == 0) { int v; sscanf(line, " B %d", &v); m_Disabled = v != 0; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "width") == 0) { sscanf(line, " F %f", &m_Width); }
    else if (strcmp(key, "scale") == 0) { sscanf(line, " F %f", &m_Scale); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "accent") == 0) { sscanf(line, " C %f %f %f %f", &m_Accent.x, &m_Accent.y, &m_Accent.z, &m_Accent.w); }
    else if (strcmp(key, "knobColor") == 0) { sscanf(line, " C %f %f %f %f", &m_KnobColor.x, &m_KnobColor.y, &m_KnobColor.z, &m_KnobColor.w); }
    else if (strcmp(key, "knobColorOff") == 0) { sscanf(line, " C %f %f %f %f", &m_KnobColorOff.x, &m_KnobColorOff.y, &m_KnobColorOff.z, &m_KnobColorOff.w); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgColor.x, &m_BgColor.y, &m_BgColor.z, &m_BgColor.w); }
    else if (strcmp(key, "bgHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgHoverColor.x, &m_BgHoverColor.y, &m_BgHoverColor.z, &m_BgHoverColor.w); }
    else if (strcmp(key, "bgActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgActiveColor.x, &m_BgActiveColor.y, &m_BgActiveColor.z, &m_BgActiveColor.w); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "borderHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderHoverColor.x, &m_BorderHoverColor.y, &m_BorderHoverColor.z, &m_BorderHoverColor.w); }
    else if (strcmp(key, "borderActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderActiveColor.x, &m_BorderActiveColor.y, &m_BorderActiveColor.z, &m_BorderActiveColor.w); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
}
void ToggleSwitchNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
        ImGui::Checkbox("On", &m_On);
        ImGui::Checkbox("Disabled", &m_Disabled);
        ImGui::DragFloat("Scale", &m_Scale, 0.05f, 0.2f, 5.0f);
        ImGui::DragFloat("Width (0=auto)", &m_Width, 1.0f, 0.0f, 2000.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Accent (ON)", &m_Accent.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Knob ON", &m_KnobColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Knob OFF", &m_KnobColorOff.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Color (OFF)", &m_BgColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Hover", &m_BgHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Active", &m_BgActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Hover", &m_BorderHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Active", &m_BorderActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 10.0f);
            ImGui::DragFloat("Rounding", &m_FrameRounding, 0.5f, 0.0f, 40.0f);
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void ToggleSwitchNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    // Find theme
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

    float s = m_Scale;
    float trackW = 44.0f * s, trackH = 24.0f * s;
    float knobD = 20.0f * s;
    float roundAmt = m_CustomStyle ? m_FrameRounding : trackH * 0.5f;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    // First frame sync
    if (!m_Initialized) {
        m_Initialized = true;
        m_PrevOn = m_On;
        m_LastFrameOn = m_On;
        m_LastFrameDisabled = m_Disabled;
    }

    // Edge detection: On/Off toggle
    if (m_On != m_LastFrameOn) {
        m_PrevOn = m_LastFrameOn;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameOn = m_On;
    }

    // Edge detection: Disabled toggle
    if (m_Disabled != m_LastFrameDisabled) {
        m_PrevOn = m_On;
        m_AnimStart = ImGui::GetTime() - 10.0;
        m_LastFrameDisabled = m_Disabled;
    }

    // Animation progress (from connected AnimationNode or built-in)
    float dur = m_Disabled ? 0.0f : (animNode ? animNode->m_Duration : 0.25f);
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);

    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float from = m_PrevOn ? 1.0f : 0.0f;
    float to   = m_On ? 1.0f : 0.0f;
    float knobNorm = from + (to - from) * t;

    // Knob center
    float cx = pos.x + 2.0f * s + knobD * 0.5f;
    float cw = trackW - 4.0f * s - knobD;
    float knobCenterX = cx + cw * knobNorm;
    float knobCenterY = pos.y + trackH * 0.5f;

    // Colors: from theme unless CustomStyle
    float rounding = (m_CustomStyle || !theme) ? m_FrameRounding : theme->m_Rounding;
    ImVec4 accentOn  = (m_CustomStyle || !theme) ? m_Accent          : theme->m_Primary;
    ImVec4 accentOff = (m_CustomStyle || !theme) ? m_BgColor         : theme->m_BgColor;
    ImVec4 knobOn    = (m_CustomStyle || !theme) ? m_KnobColor       : ImVec4(1,1,1,1);
    ImVec4 knobOff   = (m_CustomStyle || !theme) ? m_KnobColorOff    : ImVec4(0.5f,0.5f,0.55f,1.0f);
    ImVec4 textCol   = (m_CustomStyle || !theme) ? m_TextColor       : theme->m_TextColor;

    if (m_Disabled) {
        ImVec4 disText = (theme && !m_CustomStyle) ? theme->m_DisabledText : ImVec4(0.5f,0.5f,0.55f,0.5f);
        ImVec4 disBg   = (theme && !m_CustomStyle) ? theme->m_DisabledBg   : ImVec4(0.25f,0.25f,0.25f,0.4f);
        accentOn  = disBg;
        accentOff = disBg;
        knobOn    = disText;
        knobOff   = disText;
        textCol   = disText;
    }

    float klr = accentOff.x + (accentOn.x - accentOff.x) * knobNorm;
    float klg = accentOff.y + (accentOn.y - accentOff.y) * knobNorm;
    float klb = accentOff.z + (accentOn.z - accentOff.z) * knobNorm;
    float kla = accentOff.w + (accentOn.w - accentOff.w) * knobNorm;
    float kr = knobOff.x + (knobOn.x - knobOff.x) * knobNorm;
    float kg = knobOff.y + (knobOn.y - knobOff.y) * knobNorm;
    float kb = knobOff.z + (knobOn.z - knobOff.z) * knobNorm;
    float ka = knobOff.w + (knobOn.w - knobOff.w) * knobNorm;

    // Draw
    dl->AddRectFilled(ImVec2(pos.x, pos.y+1*s), ImVec2(pos.x+trackW, pos.y+trackH+1*s), IM_COL32(0,0,0,40), rounding);
    dl->AddRectFilled(pos, ImVec2(pos.x+trackW, pos.y+trackH), IM_COL32((int)(klr*255),(int)(klg*255),(int)(klb*255),(int)(kla*255)), rounding);
    dl->AddCircleFilled(ImVec2(knobCenterX+0.5f*s, knobCenterY+0.5f*s), knobD*0.5f, IM_COL32(0,0,0,30));
    dl->AddCircleFilled(ImVec2(knobCenterX, knobCenterY), knobD*0.5f, IM_COL32((int)(kr*255),(int)(kg*255),(int)(kb*255),(int)(ka*255)));

    // Interaction
    ImGui::SetCursorScreenPos(pos);
    if (!m_Disabled && ImGui::InvisibleButton(m_Label.c_str(), ImVec2(trackW, trackH))) m_On = !m_On;
    else if (m_Disabled) ImGui::Dummy(ImVec2(trackW, trackH));

    // Label
    if (!m_Label.empty() && m_Label[0] != '#') {
        ImGui::SameLine(0, 10.0f*s);
        if (m_CustomStyle && m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
        ImGui::PushStyleColor(ImGuiCol_Text, textCol);
        ImGui::TextUnformatted(m_Label.c_str());
        ImGui::PopStyleColor();
        if (m_CustomStyle && m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
    }
}



