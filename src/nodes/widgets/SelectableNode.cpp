#include "SelectableNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

SelectableNode::SelectableNode(int id)
    : Node(id, "Selectable")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void SelectableNode::Draw() {}

void SelectableNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("selected B %d\n", (int)m_Selected);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("width F %.1f\n", m_Width);
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
void SelectableNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "selected") == 0) { int v; sscanf(line, " B %d", &v); m_Selected = v != 0; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "width") == 0) { sscanf(line, " F %f", &m_Width); }
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
void SelectableNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
        ImGui::DragFloat("Width (0=auto)", &m_Width, 1.0f, 0.0f, 2000.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Settings")) {
        ImGui::Checkbox("Selected", &m_Selected);
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

void SelectableNode::RenderPreview(const RenderContext& ctx) {
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
        m_PrevSelected = m_Selected;
        m_LastFrameSelected = m_Selected;
    }

    // Edge detection
    if (m_Selected != m_LastFrameSelected) {
        m_PrevSelected = m_LastFrameSelected;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameSelected = m_Selected;
    }

    // Animation progress
    float dur = animNode ? animNode->m_Duration : 0.2f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float from = m_PrevSelected ? 1.0f : 0.0f;
    float to   = m_Selected ? 1.0f : 0.0f;
    float animNorm = from + (to - from) * t; // 0 = not selected, 1 = selected

    // Colors
    ImVec4 bgCol       = m_CustomStyle ? m_BgColor       : (theme ? theme->m_Primary    : ImVec4(0.26f,0.42f,0.70f,1.0f));
    ImVec4 bgHoverCol  = m_CustomStyle ? m_BgHoverColor  : (theme ? theme->m_PrimaryHover : ImVec4(0.30f,0.48f,0.78f,1.0f));
    ImVec4 bgActiveCol = m_CustomStyle ? m_BgActiveColor : (theme ? theme->m_Primary    : ImVec4(0.22f,0.35f,0.60f,1.0f));
    ImVec4 textCol     = m_CustomStyle ? m_TextColor     : (theme ? theme->m_TextColor  : ImVec4(0.85f,0.85f,0.92f,1.0f));
    ImVec4 borderCol   = m_CustomStyle ? m_BorderColor   : (theme ? ImVec4(0.5f,0.5f,0.6f,1.0f) : ImVec4(0.5f,0.5f,0.6f,1.0f));
    float rounding     = m_CustomStyle ? m_FrameRounding : (theme ? theme->m_Rounding   : 4.0f);
    float borderSize   = m_CustomStyle ? m_BorderSize    : 0.0f;

    // Determine selectable rect
    float itemWidth = m_Width > 0 ? m_Width : ImGui::GetContentRegionAvail().x;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float textHeight = ImGui::GetTextLineHeight();
    ImVec2 padding = (m_CustomStyle && (m_FramePadding.x > 0 || m_FramePadding.y > 0)) ? m_FramePadding : ImVec2(8.0f, 4.0f);
    ImVec2 rectMin = pos;
    ImVec2 rectMax = ImVec2(pos.x + itemWidth, pos.y + textHeight + padding.y * 2.0f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    auto& io = ImGui::GetIO();
    bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax);
    bool active = hovered && io.MouseDown[0];

    // Hover bg (subtle)
    if (hovered && !m_Selected) {
        dl->AddRectFilled(rectMin, rectMax, IM_COL32((int)(bgHoverCol.x*255),(int)(bgHoverCol.y*255),(int)(bgHoverCol.z*255),40), rounding);
    }

    // Selected bg (animated)
    if (animNorm > 0.01f) {
        int alpha = (int)(bgCol.w * 255 * animNorm);
        ImU32 selCol = IM_COL32((int)(bgCol.x*255),(int)(bgCol.y*255),(int)(bgCol.z*255), alpha);
        dl->AddRectFilled(rectMin, rectMax, selCol, rounding);
    }

    // Active press feedback
    if (active) {
        dl->AddRectFilled(rectMin, rectMax, IM_COL32((int)(bgActiveCol.x*255),(int)(bgActiveCol.y*255),(int)(bgActiveCol.z*255),60), rounding);
    }

    // Border
    if (borderSize > 0) {
        ImVec4 brd = active ? (m_CustomStyle ? m_BorderActiveColor : ImVec4(0.4f,0.4f,0.5f,1.0f))
                   : hovered ? (m_CustomStyle ? m_BorderHoverColor : ImVec4(0.7f,0.7f,0.8f,1.0f))
                   : borderCol;
        dl->AddRect(rectMin, rectMax, IM_COL32((int)(brd.x*255),(int)(brd.y*255),(int)(brd.z*255),(int)(brd.w*255)), rounding, 0, borderSize);
    }

    // Text
    ImVec2 textPos = ImVec2(rectMin.x + padding.x, rectMin.y + padding.y);
    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
    dl->AddText(textPos, IM_COL32((int)(textCol.x*255),(int)(textCol.y*255),(int)(textCol.z*255),(int)(textCol.w*255)), m_Label.c_str());
    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);

    // Interaction
    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(m_Label.c_str(), ImVec2(itemWidth, rectMax.y - rectMin.y));
    if (ImGui::IsItemClicked()) m_Selected = !m_Selected;
}


