#include "ColumnNode.h"
#include "style/ThemeNode.h"
#include "Core/PreviewHelpers.h"
#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>

float ColumnNode::GetWidgetWidth(const RenderContext* ctx) const {
    if (m_Width > 0) return m_Width;
    if (!ctx) return 0;
    float maxW = 0;
    for (auto* c : RenderHelpers::GetChildren(const_cast<ColumnNode*>(this), ctx->links, ctx->nodes)) {
        float w = c->GetWidgetWidth(ctx);
        if (w > maxW) maxW = w;
    }
    return maxW;
}

ColumnNode::ColumnNode(int id)
    : ContainerNode(id, "Column")
{
    m_Size = ImVec2(NODE_WIDTH, 100.0f);
}

void ColumnNode::SaveExtra(ImGuiTextBuffer& b) const {
    SaveBoxExtra(b);
    b.appendf("spacing F %.1f\n", m_Spacing);
    b.appendf("width F %.1f\n", m_Width);
}
void ColumnNode::LoadExtra(const char* key, const char* line) {
    if (LoadBoxProperty(key, line)) return;
    if (strcmp(key, "spacing") == 0) { sscanf(line, " F %f", &m_Spacing); }
    else if (strcmp(key, "width") == 0) { sscanf(line, " F %f", &m_Width); }
}
void ColumnNode::DrawProperties() {
    ContainerNode::DrawProperties();
    if (Node::PropSection("Basic")) {
        ImGui::DragFloat("Spacing", &m_Spacing, 0.5f, 0.0f, 50.0f);
        ImGui::DragFloat("Width (0=auto)", &m_Width, 1.0f, 0.0f, 2000.0f);
    }
}

void ColumnNode::RenderPreview(const RenderContext& ctx) {
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;
    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
    bool useChild = m_Width > 0;
    if (useChild) ImGui::BeginChild("##col", ImVec2(m_Width, m_HasBackground ? -1 : 0), false, ImGuiWindowFlags_NoScrollbar);
    if (!ch.empty()) {
        bool hasBg = m_HasBackground;
        bool useSplit = hasBg && ctx.channelSplitDepth == 0;
        RenderContext childCtx = ctx;
        if (useSplit) childCtx.channelSplitDepth = 1;

        ImDrawList* dl = nullptr; ImVec2 bgMin; float bgAvail = 0;
        float rr = wt ? wt->m_Rounding : 6.0f;
        ImVec4 cardBg = wt ? wt->m_BgColor : ImVec4(0.10f,0.10f,0.12f,1.0f);
        cardBg.x = std::min(1.0f, cardBg.x + 0.03f);
        cardBg.y = std::min(1.0f, cardBg.y + 0.03f);
        cardBg.z = std::min(1.0f, cardBg.z + 0.03f);
        ImU32 bgCol = ImGui::ColorConvertFloat4ToU32(cardBg);
        if (hasBg) {
            dl = ImGui::GetWindowDrawList();
            bgMin = ImGui::GetCursorScreenPos();
            bgAvail = ImGui::GetContentRegionAvail().x;
            if (useSplit) { dl->ChannelsSplit(2); dl->ChannelsSetCurrent(1); }
        }
        if (m_Padding > 0) { ImGui::Dummy(ImVec2(0, m_Padding)); ImGui::Indent(m_Padding); }
        for (size_t i = 0; i < ch.size(); i++) { if (i > 0) ImGui::Dummy(ImVec2(0, m_Spacing)); ch[i]->RenderPreview(childCtx); }
        if (hasBg) {
            ImVec2 bgMax = ImGui::GetItemRectMax();
            bgMax.x = bgMin.x + (useChild ? m_Width : bgAvail);
            bgMax.y += m_Padding;
            if (useChild) bgMax.y = bgMin.y + ImGui::GetWindowContentRegionMax().y;
            if (useSplit) dl->ChannelsSetCurrent(0);
            dl->AddRectFilled(bgMin, bgMax, bgCol, rr);
            if (useSplit) dl->ChannelsMerge();
        }
        if (m_Padding > 0) ImGui::Dummy(ImVec2(0, m_Padding));
    }
    if (useChild) ImGui::EndChild();
}
