#include "ComboBoxNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

ComboBoxNode::ComboBoxNode(int id)
    : Node(id, "ComboBox")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void ComboBoxNode::Draw() {}

void ComboBoxNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    std::string itemsDisp;
    const char* p = m_Items.c_str();
    while (*p) { itemsDisp += p; itemsDisp += '\n'; p += strlen(p) + 1; }
    b.appendf("items S \"%s\"\n", itemsDisp.c_str());
    b.appendf("selected I %d\n", m_Selected);
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
void ComboBoxNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "items") == 0) { char v[1024]; if (sscanf(line, " S \"%1023[^\"]\"", v) >= 1) { m_Items = v; for (char& c : m_Items) if (c == '\n') c = '\0'; } }
    else if (strcmp(key, "selected") == 0) { sscanf(line, " I %d", &m_Selected); }
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
void ComboBoxNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
        char itemsBuf[1024];
        {
            std::string display;
            const char* p = m_Items.c_str();
            while (*p) { display += p; display += '\n'; p += strlen(p) + 1; }
            snprintf(itemsBuf, sizeof(itemsBuf), "%s", display.c_str());
        }
        if (ImGui::InputTextMultiline("Items", itemsBuf, sizeof(itemsBuf), ImVec2(0, 60))) {
            m_Items = itemsBuf;
            for (char& c : m_Items) if (c == '\n') c = '\0';
        }
        ImGui::DragInt("Selected", &m_Selected, 1, 0, 100);
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

void ComboBoxNode::RenderPreview(const RenderContext& ctx) {
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

    // Edge detection for selection change pulse
    if (m_Selected != m_LastFrameSelected) {
        m_PrevSelected = m_LastFrameSelected;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameSelected = m_Selected;
    }

    // Selection change pulse animation
    float dur = animNode ? animNode->m_Duration : 0.2f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);
    float pulse = 1.0f - t;

    // Colors
    // ── Interaction first — get hover/active state ──
    float availW = m_Width > 0 ? m_Width : ImGui::GetContentRegionAvail().x;
    float fh = ImGui::GetFrameHeight();
    float padAmt = m_CustomStyle ? m_FramePadding.x : 0.0f;
    ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2(padAmt, padAmt);
    char btnId[64]; snprintf(btnId, sizeof(btnId), "##cbtn_%d", m_Id);
    ImGui::SetCursorScreenPos(ImVec2(pos.x - padAmt, pos.y - padAmt));
    ImGui::InvisibleButton(btnId, ImVec2(availW + padAmt * 2, fh + padAmt * 2));
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    if (ImGui::IsItemClicked()) {
        if (m_DropdownClosing || !m_DropdownOpen) {
            m_DropdownOpen = true; m_DropdownClosing = false; m_DropdownAnimStart = ImGui::GetTime();
        } else {
            m_DropdownAnimStart = ImGui::GetTime(); m_DropdownClosing = true;
        }
    }

    // Colors with state-aware custom style
    ImVec4 bgCol, textCol, borderCol;
    float rounding;
    if (m_CustomStyle) {
        bgCol     = active ? m_BgActiveColor : (hovered ? m_BgHoverColor : m_BgColor);
        textCol   = m_TextColor;
        borderCol = active ? m_BorderActiveColor : (hovered ? m_BorderHoverColor : m_BorderColor);
        rounding  = m_FrameRounding;
    } else {
        bgCol     = wt ? wt->m_Primary : ImVec4(0.26f,0.42f,0.70f,1.0f);
        textCol   = wt ? wt->m_TextColor : ImVec4(1,1,1,1);
        borderCol = ImVec4(0,0,0,0);
        rounding  = wt ? wt->m_Rounding : 4.0f;
    }
    if (pulse > 0.01f && m_LastFrameSelected != m_PrevSelected) {
        bgCol.x = std::min(1.0f, bgCol.x + pulse * 0.2f);
        bgCol.y = std::min(1.0f, bgCol.y + pulse * 0.2f);
        bgCol.z = std::min(1.0f, bgCol.z + pulse * 0.2f);
    }

    // ── Draw combo button ──
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, pos + ImVec2(availW, fh), ImGui::ColorConvertFloat4ToU32(bgCol), rounding);
    if (m_CustomStyle && m_BorderSize > 0)
        dl->AddRect(pos, pos + ImVec2(availW, fh), ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, m_BorderSize);
    // Label inside button
    const char* selectedText = "(none)";
    int idx = 0; const char* sp = m_Items.c_str();
    while (*sp) { if (idx == m_Selected) { selectedText = sp; break; } sp += strlen(sp) + 1; idx++; }
    float textPad = ImGui::GetStyle().FramePadding.x;
    std::string displayText = (!m_Label.empty() && m_Label[0] != '#') ? m_Label + ": " + selectedText : selectedText;
    dl->AddText(ImVec2(pos.x + textPad, pos.y + (fh - ImGui::GetTextLineHeight()) * 0.5f),
        ImGui::ColorConvertFloat4ToU32(textCol), displayText.c_str());
    // Arrow
    float arSz = fh * 0.3f; float arX = pos.x + availW - textPad - arSz; float arY = pos.y + (fh - arSz) * 0.5f;
    dl->AddTriangleFilled(ImVec2(arX, arY), ImVec2(arX + arSz, arY), ImVec2(arX + arSz * 0.5f, arY + arSz * 0.7f),
        ImGui::ColorConvertFloat4ToU32(textCol));

    int itemCount = 0; const char* cp = m_Items.c_str(); while (*cp) { itemCount++; cp += strlen(cp) + 1; }
    float itemH = ImGui::GetTextLineHeightWithSpacing();
    float fullH = itemCount * itemH + ImGui::GetStyle().WindowPadding.y * 2;

    // Bidirectional animation
    float ddDur = animNode ? animNode->m_Duration : 0.15f;
    double ddElapsed = ImGui::GetTime() - m_DropdownAnimStart;
    float ddT = (float)std::min(1.0, ddElapsed / ddDur);

    // Apply easing
    ddT = AnimationNode::ApplyEasing(ddT, animNode ? animNode->m_Easing : "", animNode);

    // The animation progress (0=hidden, 1=fully open)
    float progress = m_DropdownClosing ? (1.0f - ddT) : ddT;
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    float animH = fullH * progress;

    // Close when animation finishes after closing
    if (m_DropdownClosing && ddT >= 1.0f) {
        m_DropdownOpen = false;
        m_DropdownClosing = false;
    }

    // Draw dropdown overlay (manually, not using ImGui popup)
    if (m_DropdownOpen && animH > 0.5f) {
        ImVec2 dPos(pos.x, pos.y + fh);
        ImVec2 dSize(availW, animH);
        ImU32 bgCol32 = IM_COL32(20, 20, 28, (int)(250 * progress));
        ImU32 borderCol32 = IM_COL32(120, 140, 200, (int)(230 * progress));
        float drr = 4.0f;

        dl->AddRectFilled(dPos, dPos + dSize, bgCol32, drr);
        dl->AddRect(dPos, dPos + dSize, borderCol32, drr, 0, 1.5f);

        // Items
        ImGui::SetCursorScreenPos(dPos + ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, progress);
        // Create a child region so Selectable works within it
        ImGui::BeginChild("##ddList", ImVec2(availW, animH - 2), false, ImGuiWindowFlags_NoScrollWithMouse);
        int itemIdx = 0; const char* sip = m_Items.c_str();
        while (*sip) {
            ImVec2 itemPos = ImGui::GetCursorScreenPos();
            bool isSel = (itemIdx == m_Selected);
            if (ImGui::Selectable(sip, isSel)) {
                m_Selected = itemIdx;
                m_DropdownAnimStart = ImGui::GetTime();
                m_DropdownClosing = true;
            }
            // Highlight selected item
            if (isSel) {
                dl->AddRectFilled(itemPos, itemPos + ImVec2(availW, ImGui::GetTextLineHeightWithSpacing()),
                    IM_COL32(50, 60, 120, (int)(180 * progress)));
            }
            sip += strlen(sip) + 1;
            itemIdx++;
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    // Click outside to close
    if (m_DropdownOpen && !m_DropdownClosing) {
        ImVec2 dPos(pos.x, pos.y + fh);
        ImVec2 dSize(availW, animH);
        bool inButton = ImGui::IsMouseHoveringRect(pos, pos + ImVec2(availW, fh));
        bool inList = ImGui::IsMouseHoveringRect(dPos, dPos + dSize);
        if ((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && !inButton && !inList) {
            m_DropdownAnimStart = ImGui::GetTime();
            m_DropdownClosing = true;
        }
    }

    ImGui::Dummy(ImVec2(availW, 1));
}

