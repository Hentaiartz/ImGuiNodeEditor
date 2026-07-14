#include "SpacerNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>

SpacerNode::SpacerNode(int id)
    : Node(id, "Spacer")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 50.0f);
}

void SpacerNode::Draw() {}

void SpacerNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("widgetSize V %.1f %.1f\n", m_WidgetSize.x, m_WidgetSize.y);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
}
void SpacerNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "widgetSize") == 0) { sscanf(line, " V %f %f", &m_WidgetSize.x, &m_WidgetSize.y); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
}
void SpacerNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        ImGui::DragFloat2("Size", &m_WidgetSize.x, 0.5f, 0.0f, 200.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
}

void SpacerNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ImGui::Dummy(m_WidgetSize);
}

