#include "SameLineNode.h"
#include "Core/Application.h"
#include <imgui.h>
#include <cstdio>

SameLineNode::SameLineNode(int id)
    : Node(id, "SameLine")
{
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void SameLineNode::Draw() {}

void SameLineNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("spacing F %.1f\n", m_Spacing);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
}
void SameLineNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "spacing") == 0) { sscanf(line, " F %f", &m_Spacing); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
}
void SameLineNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        ImGui::DragFloat("Spacing", &m_Spacing, 0.1f, -100.0f, 10000.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
}

void SameLineNode::RenderPreview(const RenderContext& ctx) {
    ImGui::SameLine(0, m_Spacing);
}

