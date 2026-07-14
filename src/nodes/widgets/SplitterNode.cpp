#include "SplitterNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>

SplitterNode::SplitterNode(int id)
    : Node(id, "Splitter")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 2, "Side A", PinKind::Input, PinType::Container, id });
    m_Inputs.push_back({ id * 10000 + 3, "Side B", PinKind::Input, PinType::Container, id });
    m_Outputs.push_back({ id * 10000 + 100, "Container", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 80.0f);
}

void SplitterNode::Draw() {}

void SplitterNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("vertical B %d\n", (int)m_Vertical);
    b.appendf("ratio F %.3f\n", m_InitialRatio);
    b.appendf("thickness F %.1f\n", m_Thickness);
    b.appendf("minPane F %.1f\n", m_MinPane);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
}
void SplitterNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "vertical") == 0) { int v; sscanf(line, " B %d", &v); m_Vertical = v != 0; }
    else if (strcmp(key, "ratio") == 0) { sscanf(line, " F %f", &m_InitialRatio); }
    else if (strcmp(key, "thickness") == 0) { sscanf(line, " F %f", &m_Thickness); }
    else if (strcmp(key, "minPane") == 0) { sscanf(line, " F %f", &m_MinPane); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
}
void SplitterNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        ImGui::Checkbox("Vertical Split", &m_Vertical);
        ImGui::DragFloat("Ratio", &m_InitialRatio, 0.01f, 0.05f, 0.95f);
        ImGui::DragFloat("Thickness", &m_Thickness, 0.5f, 1.0f, 20.0f);
        ImGui::DragFloat("Min Pane", &m_MinPane, 1.0f, 10.0f, 500.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
}

void SplitterNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;

    ImU32 handleHoveredCol, handleNormalCol;
    if (theme) {
        handleHoveredCol = IM_COL32((int)(theme->m_Primary.x*255), (int)(theme->m_Primary.y*255), (int)(theme->m_Primary.z*255), 150);
        handleNormalCol  = IM_COL32((int)(theme->m_Primary.x*255*0.5f), (int)(theme->m_Primary.y*255*0.5f), (int)(theme->m_Primary.z*255*0.5f), 100);
    } else {
        handleHoveredCol = IM_COL32(120, 140, 200, 150);
        handleNormalCol  = IM_COL32(60, 70, 100, 100);
    }

    // Assign by input pin IDs (Side A = first container input, Side B = second)
    int sideAPin = m_Inputs.size() >= 2 ? m_Inputs[1].id : -1;
    int sideBPin = m_Inputs.size() >= 3 ? m_Inputs[2].id : -1;
    Node* a = nullptr, * b = nullptr;
    for (auto& l : ctx.links) {
        if (l.toPinId == sideAPin)
            for (auto& n : ctx.nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == l.fromPinId) { a = n.get(); break; }
        if (l.toPinId == sideBPin)
            for (auto& n : ctx.nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == l.fromPinId) { b = n.get(); break; }
    }
    if (!a && !b) {
        ImGui::TextDisabled("Splitter: connect children to Side A / Side B");
        return;
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::PushID(m_Id);
    float& ratio = m_InitialRatio; // we modify ratio directly for the drag

    if (m_Vertical) {
        float totalH = avail.y;
        float paneH = totalH * ratio;
        if (paneH < m_MinPane) paneH = m_MinPane;
        if (totalH - paneH - m_Thickness < m_MinPane) paneH = totalH - m_MinPane - m_Thickness;
        if (a) {
            ImGui::BeginChild("##splitA", ImVec2(avail.x, paneH), false);
            a->RenderPreview(ctx);
            ImGui::EndChild();
        }
        // Splitter handle
        ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
        ImGui::InvisibleButton("##splitH", ImVec2(avail.x, m_Thickness));
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            ratio += ImGui::GetIO().MouseDelta.y / totalH;
            if (ratio < 0.05f) ratio = 0.05f;
            if (ratio > 0.95f) ratio = 0.95f;
        }
        ImU32 handleCol = ImGui::IsItemHovered() ? handleHoveredCol : handleNormalCol;
        ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), handleCol);
        if (b) {
            float remaining = totalH - paneH - m_Thickness;
            if (remaining > 0) {
                ImGui::BeginChild("##splitB", ImVec2(avail.x, remaining), false);
                b->RenderPreview(ctx);
                ImGui::EndChild();
            }
        }
    } else {
        float totalW = avail.x;
        float paneW = totalW * ratio;
        if (paneW < m_MinPane) paneW = m_MinPane;
        if (totalW - paneW - m_Thickness < m_MinPane) paneW = totalW - m_MinPane - m_Thickness;
        if (a) {
            ImGui::BeginChild("##splitA", ImVec2(paneW, 0), false);
            a->RenderPreview(ctx);
            ImGui::EndChild();
            ImGui::SameLine();
        }
        ImGui::InvisibleButton("##splitH", ImVec2(m_Thickness, avail.y));
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            ratio += ImGui::GetIO().MouseDelta.x / totalW;
            if (ratio < 0.05f) ratio = 0.05f;
            if (ratio > 0.95f) ratio = 0.95f;
        }
        ImU32 handleCol = ImGui::IsItemHovered() ? handleHoveredCol : handleNormalCol;
        ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), handleCol);
        if (b) {
            float remaining = totalW - paneW - m_Thickness;
            if (remaining > 0) {
                ImGui::SameLine();
                ImGui::BeginChild("##splitB", ImVec2(remaining, 0), false);
                b->RenderPreview(ctx);
                ImGui::EndChild();
            }
        }
    }
    ImGui::PopID();
}

