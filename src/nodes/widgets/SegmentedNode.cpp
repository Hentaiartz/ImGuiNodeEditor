#include "SegmentedNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

SegmentedNode::SegmentedNode(int id)
    : Node(id, "Segmented")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 50.0f);
}

void SegmentedNode::Draw() {}

void SegmentedNode::SaveExtra(ImGuiTextBuffer& b) const {
    std::string itemsDisp;
    const char* p = m_Items.c_str();
    while (*p) { itemsDisp += p; itemsDisp += '\n'; p += strlen(p) + 1; }
    b.appendf("items S \"%s\"\n", itemsDisp.c_str());
    b.appendf("selected I %d\n", m_Selected);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BgColor.x, m_BgColor.y, m_BgColor.z, m_BgColor.w);
    b.appendf("bgHoverColor C %.3f %.3f %.3f %.3f\n", m_BgHoverColor.x, m_BgHoverColor.y, m_BgHoverColor.z, m_BgHoverColor.w);
    b.appendf("bgActiveColor C %.3f %.3f %.3f %.3f\n", m_BgActiveColor.x, m_BgActiveColor.y, m_BgActiveColor.z, m_BgActiveColor.w);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("fontScale F %.2f\n", m_FontScale);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
}

void SegmentedNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "items") == 0) { char v[1024]; if (sscanf(line, " S \"%1023[^\"]\"", v) >= 1) m_Items = v; }
    else if (strcmp(key, "selected") == 0) { sscanf(line, " I %d", &m_Selected); }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgColor.x, &m_BgColor.y, &m_BgColor.z, &m_BgColor.w); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "bgHoverColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgHoverColor.x, &m_BgHoverColor.y, &m_BgHoverColor.z, &m_BgHoverColor.w); }
    else if (strcmp(key, "bgActiveColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgActiveColor.x, &m_BgActiveColor.y, &m_BgActiveColor.z, &m_BgActiveColor.w); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
}

void SegmentedNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[1024];
        {
            std::string display;
            const char* p = m_Items.c_str();
            while (*p) { display += p; display += '\n'; p += strlen(p) + 1; }
            snprintf(buf, sizeof(buf), "%s", display.c_str());
        }
        if (ImGui::InputTextMultiline("Items", buf, sizeof(buf), ImVec2(0, 60))) {
            m_Items = buf;
            for (char& c : m_Items) if (c == '\n') c = '\0';
        }
        ImGui::DragInt("Selected", &m_Selected, 1, 0, 100);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Bg Color", &m_BgColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Hover", &m_BgHoverColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Bg Active", &m_BgActiveColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 10.0f);
            ImGui::DragFloat("Rounding", &m_FrameRounding, 0.5f, 0.0f, 20.0f);
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void SegmentedNode::RenderPreview(const RenderContext& ctx) {
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

    if (!m_Initialized) {
        m_Initialized = true;
        m_PrevSelected = m_Selected;
        m_LastFrameSelected = m_Selected;
    }
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

    const char* p = m_Items.c_str();
    int totalSegs = (int)std::count(m_Items.begin(), m_Items.end(), '\0');
    if (totalSegs < 1) return;

    float availW = ImGui::GetContentRegionAvail().x;
    float segW = availW / (float)totalSegs;
    if (segW < 5.0f) segW = 5.0f;
    float h = ImGui::GetFrameHeight();
    float rounding = m_CustomStyle ? m_FrameRounding : (wt ? wt->m_Rounding : 4.0f);
    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 basePos = ImGui::GetCursorScreenPos();

    // Theme colors
    ImVec4 selBg = m_CustomStyle ? m_BgColor : (wt ? wt->m_Primary : ImVec4(0.31f, 0.27f, 0.78f, 1.0f));
    ImVec4 selText = m_CustomStyle ? m_TextColor : ImVec4(1, 1, 1, 1);
    ImVec4 unselBg = m_CustomStyle ? ImVec4(0.14f,0.14f,0.18f,1.0f) : ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
    ImVec4 unselText = m_CustomStyle ? m_TextColor : (wt ? wt->m_TextColor : ImVec4(0.7f, 0.7f, 0.74f, 1.0f));

    // Animated highlight rect position
    float fromX = basePos.x + m_PrevSelected * segW;
    float toX   = basePos.x + m_Selected * segW;
    float hlX = fromX + (toX - fromX) * t;

    // Draw each segment (unselected background + text)
    int idx = 0;
    while (*p) {
        std::string lbl(p); p += lbl.size() + 1;
        ImVec2 pos = ImVec2(basePos.x + idx * segW, basePos.y);

        ImDrawFlags flags = ImDrawFlags_RoundCornersNone;
        if (totalSegs == 1) flags = ImDrawFlags_RoundCornersAll;
        else if (idx == 0) flags = ImDrawFlags_RoundCornersLeft;
        else if (idx == totalSegs - 1) flags = ImDrawFlags_RoundCornersRight;

        dl->AddRectFilled(pos, pos + ImVec2(segW, h), ImGui::ColorConvertFloat4ToU32(unselBg), rounding, flags);

        if (m_BorderSize > 0) {
            ImU32 bc = ImGui::ColorConvertFloat4ToU32(m_CustomStyle ? m_BorderColor : (wt ? wt->m_BorderColor : ImVec4(0.22f, 0.24f, 0.32f, 1.0f)));
            dl->AddRect(pos, pos + ImVec2(segW, h), bc, rounding, flags, m_BorderSize);
        }

        // Draw unselected text (will be overdrawn by selected text + highlight later)
        ImVec2 ts = ImGui::CalcTextSize(lbl.c_str());
        dl->AddText(pos + ImVec2((segW - ts.x) * 0.5f, (h - ts.y) * 0.5f),
            ImGui::ColorConvertFloat4ToU32(unselText), lbl.c_str());

        ImGui::SetCursorScreenPos(pos);
        if (ImGui::InvisibleButton(lbl.c_str(), ImVec2(segW, h))) m_Selected = idx;
        if (m_CustomStyle && ImGui::IsItemHovered()) {
            ImU32 hoverOverlay = ImGui::IsItemActive()
                ? IM_COL32((int)(m_BgActiveColor.x*255),(int)(m_BgActiveColor.y*255),(int)(m_BgActiveColor.z*255),80)
                : IM_COL32((int)(m_BgHoverColor.x*255),(int)(m_BgHoverColor.y*255),(int)(m_BgHoverColor.z*255),60);
            dl->AddRectFilled(pos, pos + ImVec2(segW, h), hoverOverlay, rounding, flags);
        }
        idx++;
    }

    // Draw animated highlight as an overlay
    {
        float hlL = ImMax(hlX, basePos.x);
        float hlR = ImMin(hlX + segW, basePos.x + totalSegs * segW);
        if (hlR > hlL) {
            // Determine rounding flags for the animated rect
            bool atLeft = (hlL <= basePos.x + segW * 0.1f);
            bool atRight = (hlR >= basePos.x + segW * (totalSegs - 0.9f));
            ImDrawFlags hlFlags = ImDrawFlags_RoundCornersNone;
            if (totalSegs == 1) hlFlags = ImDrawFlags_RoundCornersAll;
            else {
                if (atLeft) hlFlags |= ImDrawFlags_RoundCornersLeft;
                if (atRight) hlFlags |= ImDrawFlags_RoundCornersRight;
            }
            dl->AddRectFilled(ImVec2(hlL, basePos.y), ImVec2(hlR, basePos.y + h),
                ImGui::ColorConvertFloat4ToU32(selBg), rounding, hlFlags);

            // Redraw text with selected color for the highlighted area
            ImGui::PushClipRect(ImVec2(hlL, basePos.y), ImVec2(hlR, basePos.y + h), true);
            idx = 0; p = m_Items.c_str();
            while (*p) {
                std::string lbl(p); p += lbl.size() + 1;
                float segCenter = basePos.x + idx * segW + segW * 0.5f;
                if (segCenter >= hlL && segCenter <= hlR) {
                    ImVec2 ts = ImGui::CalcTextSize(lbl.c_str());
                    dl->AddText(ImVec2(basePos.x + idx * segW + (segW - ts.x) * 0.5f, basePos.y + (h - ts.y) * 0.5f),
                        ImGui::ColorConvertFloat4ToU32(selText), lbl.c_str());
                }
                idx++;
            }
            ImGui::PopClipRect();
        }
    }

    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
    ImGui::Dummy(ImVec2(0, h));
}



