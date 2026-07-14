#include "GridNode.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>

GridNode::GridNode(int id)
    : Node(id, "Grid")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 2, "Children", PinKind::Input, PinType::Container, id });
    m_Outputs.push_back({ id * 10000 + 100, "Container", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 100.0f);
    m_Weights = { 1.0f, 1.0f };
}

void GridNode::Draw() {}

void GridNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("columns I %d\n", m_Columns);
    b.appendf("spacing F %.1f\n", m_Spacing);
    b.appendf("padding F %.1f\n", m_Padding);
    b.appendf("weights");
    for (float w : m_Weights) b.appendf(" %.2f", w);
    b.appendf("\n");
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
}

void GridNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "columns") == 0) { sscanf(line, " I %d", &m_Columns); m_Columns = std::max(1, m_Columns); }
    else if (strcmp(key, "spacing") == 0) { sscanf(line, " F %f", &m_Spacing); }
    else if (strcmp(key, "padding") == 0) { sscanf(line, " F %f", &m_Padding); }
    else if (strcmp(key, "weights") == 0) {
        m_Weights.clear();
        const char* p = line; float w;
        while (*p) { if (sscanf(p, " %f", &w) == 1) { m_Weights.push_back(w); while (*p && *p != ' ') p++; while (*p == ' ') p++; } else break; }
    }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
}

void GridNode::DrawProperties() {
    if (Node::PropSection("Grid")) {
        ImGui::DragInt("Columns", &m_Columns, 1, 1, 8);
        ImGui::DragFloat("Spacing", &m_Spacing, 0.5f, 0.0f, 40.0f);
        ImGui::DragFloat("Padding", &m_Padding, 0.5f, 0.0f, 40.0f);
        if (ImGui::CollapsingHeader("Column Weights", ImGuiTreeNodeFlags_DefaultOpen)) {
            m_Weights.resize(m_Columns, 1.0f);
            for (int i = 0; i < m_Columns; i++) {
                char lbl[32]; snprintf(lbl, sizeof(lbl), "Col %d", i + 1);
                ImGui::DragFloat(lbl, &m_Weights[i], 0.1f, 0.1f, 5.0f);
            }
        }
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 10.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void GridNode::RenderPreview(const RenderContext& ctx) {
    m_Weights.resize(std::max(1, m_Columns), 1.0f);

    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;

    // Find all children connected to the single Children pin
    std::vector<Node*> children;
    int childPinId = -1;
    for (auto& p : m_Inputs)
        if (p.type == PinType::Container) { childPinId = p.id; break; }
    if (childPinId >= 0) {
        for (auto& link : ctx.links) {
            if (link.toPinId != childPinId) continue;
            for (auto& n : ctx.nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == link.fromPinId) { children.push_back(n.get()); break; }
        }
    }

    if (children.empty()) return;

    ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchProp;
    if (m_CustomStyle) {
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
        if (m_CustomStyle) ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, m_BorderColor);
        if (m_BorderSize > 0) {
            flags |= ImGuiTableFlags_Borders;
        }
    } else if (wt) {
        ImGui::PushStyleColor(ImGuiCol_Text, wt->m_TextColor);
    }
    if (ImGui::BeginTable("##grid", m_Columns, flags)) {
        for (int col = 0; col < m_Columns; col++)
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, m_Weights[col]);
        ImGui::TableNextRow(0, 0);
        for (int col = 0; col < m_Columns; col++) {
            ImGui::TableSetColumnIndex(col);
            if (m_Padding > 0) ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(m_Padding, m_Padding));
            for (int ci = col; ci < (int)children.size(); ci += m_Columns) {
                children[ci]->RenderPreview(ctx);
            }
            if (m_Padding > 0) ImGui::PopStyleVar();
        }
        ImGui::EndTable();
    }
    if (m_CustomStyle) {
        ImGui::PopStyleColor(m_BorderSize > 0 ? 2 : 1);
    } else if (wt) {
        ImGui::PopStyleColor();
    }
}

