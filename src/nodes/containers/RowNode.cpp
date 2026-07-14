#include "RowNode.h"
#include "containers/ColumnNode.h"
#include "style/ThemeNode.h"
#include "Core/PreviewHelpers.h"
#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>

RowNode::RowNode(int id)
    : ContainerNode(id, "Row")
{
    m_Size = ImVec2(NODE_WIDTH, 100.0f);
}

void RowNode::SaveExtra(ImGuiTextBuffer& b) const {
    SaveBoxExtra(b);
    b.appendf("spacing F %.1f\n", m_Spacing);
}
void RowNode::LoadExtra(const char* key, const char* line) {
    if (LoadBoxProperty(key, line)) return;
    if (strcmp(key, "spacing") == 0) { sscanf(line, " F %f", &m_Spacing); }
}
void RowNode::DrawProperties() {
    ContainerNode::DrawProperties();
    if (Node::PropSection("Basic")) {
        ImGui::DragFloat("Spacing", &m_Spacing, 0.5f, 0.0f, 50.0f);
    }
}

// Helper: check if a node uses BeginChild (constrained width)
static bool UsesBeginChild(Node* n) {
    if (auto* c = dynamic_cast<ColumnNode*>(n))
        return c->m_Width > 0;
    return false;
}

void RowNode::RenderPreview(const RenderContext& ctx) {
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;
    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
    if (ch.empty()) return;

    bool hasBg = m_HasBackground;
    bool useSplit = hasBg && ctx.channelSplitDepth == 0;
    RenderContext childCtx = ctx;
    if (useSplit) childCtx.channelSplitDepth = 1;

    // Check if any child uses BeginChild → use flex layout instead of SameLine
    bool hasFlexChild = false;
    for (auto* c : ch) if (UsesBeginChild(c)) { hasFlexChild = true; break; }

    if (hasFlexChild) {
        // ── Flex layout: each child gets its own BeginChild ──
        float availW = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
        float cursorX = ImGui::GetCursorPosX();
        float startX = ImGui::GetCursorScreenPos().x;
        float startY = ImGui::GetCursorScreenPos().y;

        // Calculate widths: fixed children get m_Width, others share remaining
        float totalSpacing = m_Spacing * (ch.size() - 1);
        float fixedW = 0; int flexCount = 0;
        for (auto* c : ch) {
            if (auto* col = dynamic_cast<ColumnNode*>(c)) {
                if (col->m_Width > 0) fixedW += col->m_Width;
                else flexCount++;
            } else flexCount++;
        }
        float remainingW = availW - cursorX - fixedW - totalSpacing;
        float flexW = flexCount > 0 ? remainingW / (float)flexCount : 0;
        if (flexW < 50.0f) flexW = 50.0f;

        ImDrawList* dl = nullptr; ImVec2 bgMin;
        float rr = wt ? wt->m_Rounding : 6.0f;
        ImVec4 cardBg = wt ? wt->m_BgColor : ImVec4(0.10f,0.10f,0.12f,1.0f);
        ImU32 bgCol = ImGui::ColorConvertFloat4ToU32(cardBg);
        if (hasBg) { dl = ImGui::GetWindowDrawList(); bgMin = ImVec2(startX + cursorX, startY); }

        float curX = startX + cursorX;
        for (size_t i = 0; i < ch.size(); i++) {
            float childW = flexW;
            if (auto* col = dynamic_cast<ColumnNode*>(ch[i])) {
                if (col->m_Width > 0) childW = col->m_Width;
            }
            ImGui::SetCursorScreenPos(ImVec2(curX, startY));
            char childId[32]; snprintf(childId, 32, "##row%d_%d", m_Id, (int)i);
            ImGui::BeginChild(childId, ImVec2(childW, -1), false, ImGuiWindowFlags_NoScrollbar);
            ch[i]->RenderPreview(childCtx);
            ImGui::EndChild();
            curX += childW + m_Spacing;
        }

        if (hasBg && dl) {
            ImVec2 bgMax(curX - m_Spacing, ImGui::GetItemRectMax().y + m_Padding);
            bgMin.x -= m_Padding;
            bgMax.x += m_Padding;
            dl->AddRectFilled(bgMin, bgMax, bgCol, rr);
        }
    } else {
        // ── Normal layout: SameLine ──
        ImDrawList* dl = nullptr; ImVec2 bgMin; float bgAvailX = 0;
        float rr = wt ? wt->m_Rounding : 6.0f;
        ImVec4 cardBg = wt ? wt->m_BgColor : ImVec4(0.10f,0.10f,0.12f,1.0f);
        ImU32 bgCol = ImGui::ColorConvertFloat4ToU32(cardBg);
        if (hasBg) {
            dl = ImGui::GetWindowDrawList();
            bgMin = ImGui::GetCursorScreenPos();
            bgAvailX = ImGui::GetContentRegionAvail().x;
            if (useSplit) { dl->ChannelsSplit(2); dl->ChannelsSetCurrent(1); }
            else { dl->AddRectFilled(bgMin, bgMin + ImVec2(bgAvailX, 100), bgCol, rr); }
        }
        if (m_Padding > 0) { ImGui::Dummy(ImVec2(m_Padding, 0)); ImGui::SameLine(); }
        float spacing = m_Spacing;
        if (m_Justify != BoxJustify::Start) {
            float tw = 0; for (auto* c : ch) tw += RenderHelpers::GetWidgetWidth(c, &ctx);
            if (ch.size() > 1 && m_Justify != BoxJustify::SpaceBetween) tw += m_Spacing * (ch.size() - 1);
            float contentW = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
            float cursorX = ImGui::GetCursorPosX();
            if (m_Justify == BoxJustify::SpaceBetween && ch.size() > 1) {
                float avail = contentW - cursorX;
                spacing = (avail - tw) / (float)(ch.size() - 1);
                if (spacing < 0) spacing = m_Spacing;
            } else {
                float off = (m_Justify == BoxJustify::Center) ? (contentW - tw) * 0.5f - cursorX : contentW - tw - cursorX;
                if (off > 0) { ImGui::Dummy(ImVec2(off, 0)); ImGui::SameLine(); }
            }
        }
        for (size_t i = 0; i < ch.size(); i++) {
            if (i > 0) ImGui::SameLine(0, spacing);
            ch[i]->RenderPreview(childCtx);
        }
        ImVec2 bgMax(0,0);
        if (hasBg) bgMax = ImGui::GetItemRectMax();
        if (m_Padding > 0) { ImGui::SameLine(0, spacing); ImGui::Dummy(ImVec2(m_Padding, 0)); }
        if (hasBg && dl) {
            bgMax.x = bgMin.x + bgAvailX;
            if (useSplit) { dl->ChannelsSetCurrent(0); dl->AddRectFilled(bgMin, bgMax, bgCol, rr); dl->ChannelsMerge(); }
        }
    }
}
