#include "RadioButtonNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

RadioButtonNode::RadioButtonNode(int id)
    : Node(id, "RadioButton")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void RadioButtonNode::Draw() {}

void RadioButtonNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("value I %d\n", m_Value);
    b.appendf("groupId I %d\n", m_GroupId);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("scale F %.2f\n", m_Scale);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
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
void RadioButtonNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "value") == 0) { sscanf(line, " I %d", &m_Value); }
    else if (strcmp(key, "groupId") == 0) { sscanf(line, " I %d", &m_GroupId); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "scale") == 0) { sscanf(line, " F %f", &m_Scale); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
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
void RadioButtonNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
        ImGui::DragInt("Value", &m_Value, 1, -10000, 10000);
        ImGui::DragInt("Group ID", &m_GroupId, 1, 0, 10000);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Settings")) {
        ImGui::DragFloat("Scale", &m_Scale, 0.1f, 0.5f, 4.0f, "%.1fx");
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
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void RadioButtonNode::RenderPreview(const RenderContext& ctx) {
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

    bool selected = (m_Value == m_GroupId);

    if (!m_Initialized) {
        m_Initialized = true;
        m_PrevSelected = selected;
        m_LastFrameSelected = selected;
    }

    // Edge detection
    if (selected != m_LastFrameSelected) {
        m_PrevSelected = m_LastFrameSelected;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameSelected = selected;
    }

    // Animation progress
    float dur = animNode ? animNode->m_Duration : 0.2f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float from = m_PrevSelected ? 1.0f : 0.0f;
    float to   = selected ? 1.0f : 0.0f;
    float animNorm = from + (to - from) * t; // 0 = not selected, 1 = selected

    // Colors
    ImVec4 bgCol      = m_CustomStyle ? m_BgColor       : (theme ? ImVec4(theme->m_BgColor.x, theme->m_BgColor.y, theme->m_BgColor.z, 1.0f) : ImVec4(0.16f,0.16f,0.20f,1.0f));
    ImVec4 bgHovCol   = m_CustomStyle ? m_BgHoverColor  : (theme ? theme->m_PrimaryHover : ImVec4(0.30f,0.48f,0.78f,1.0f));
    ImVec4 dotCol     = m_CustomStyle ? m_TextColor     : (theme ? theme->m_CheckMark : ImVec4(0.85f,0.85f,0.92f,1.0f));
    ImVec4 textCol    = m_CustomStyle ? m_TextColor     : (theme ? theme->m_TextColor : ImVec4(0.85f,0.85f,0.92f,1.0f));
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float fs = ImGui::GetFontSize();
    float radioSize = fs * 0.8f * m_Scale;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 center = ImVec2(pos.x + radioSize * 0.5f, pos.y + radioSize * 0.5f);
    float outerR = radioSize * 0.5f;
    float innerR = outerR * 0.35f * animNorm;

    auto& io = ImGui::GetIO();
    bool hovered = ImGui::IsMouseHoveringRect(pos, pos + ImVec2(radioSize, radioSize));
    bool active = hovered && io.MouseDown[0];

    // Shadow
    dl->AddCircleFilled(center, outerR + 1.0f, IM_COL32(0,0,0,40));

    // Outer circle
    ImU32 outerCol = active ? IM_COL32((int)(bgHovCol.x*255),(int)(bgHovCol.y*255),(int)(bgHovCol.z*255),(int)(bgHovCol.w*255))
                  : hovered ? IM_COL32((int)(bgHovCol.x*255),(int)(bgHovCol.y*255),(int)(bgHovCol.z*255),(int)(bgHovCol.w*255*0.7f))
                  : IM_COL32((int)(bgCol.x*255),(int)(bgCol.y*255),(int)(bgCol.z*255),(int)(bgCol.w*255));
    dl->AddCircleFilled(center, outerR, outerCol);
    dl->AddCircle(center, outerR, IM_COL32(255,255,255,30), 0, 1.5f);

    // Inner dot (animated)
    if (animNorm > 0.01f) {
        ImU32 dotCol32 = IM_COL32((int)(dotCol.x*255),(int)(dotCol.y*255),(int)(dotCol.z*255),(int)(dotCol.w*255*animNorm));
        dl->AddCircleFilled(center, innerR, dotCol32);
    }

    // Click
    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(m_Label.c_str(), ImVec2(radioSize, radioSize));
    if (ImGui::IsItemClicked()) { m_Value = m_GroupId; }

    // Label
    if (!m_Label.empty() && m_Label[0] != '#') {
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        float textScale = 1.0f + (m_Scale - 1.0f) * 0.5f;
        if (textScale != 1.0f) ImGui::SetWindowFontScale(textScale);
        ImGui::PushStyleColor(ImGuiCol_Text, textCol);
        ImGui::TextUnformatted(m_Label.c_str());
        ImGui::PopStyleColor();
        if (textScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
    }
}


