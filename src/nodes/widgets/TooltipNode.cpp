#include "TooltipNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>

TooltipNode::TooltipNode(int id)
    : Node(id, "Tooltip")
{
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void TooltipNode::Draw() {}

void TooltipNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("text S \"%s\"\n", m_Text.c_str());
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
}
void TooltipNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "text") == 0) { char v[1024]; if (sscanf(line, " S \"%1023[^\"]\"", v) >= 1) m_Text = v; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
}
void TooltipNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf))) m_Label = buf;
        char textBuf[1024];
        snprintf(textBuf, sizeof(textBuf), "%s", m_Text.c_str());
        if (ImGui::InputTextMultiline("Text", textBuf, sizeof(textBuf), ImVec2(0, 60))) m_Text = textBuf;
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
}

void TooltipNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    if (!m_Label.empty()) {
        ImGui::TextUnformatted(m_Label.c_str());
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(m_Text.c_str());
            ImGui::EndTooltip();
        }
        return;
    }
    // No label: render a small hover zone
    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight()));
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(m_Text.c_str());
        ImGui::EndTooltip();
    }
}

