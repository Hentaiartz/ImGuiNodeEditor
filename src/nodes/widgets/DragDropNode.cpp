#include "DragDropNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>

DragDropNode::DragDropNode(int id)
    : Node(id, "DragDrop")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void DragDropNode::Draw() {}

void DragDropNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("isSource B %d\n", (int)m_IsSource);
    b.appendf("isTarget B %d\n", (int)m_IsTarget);
    b.appendf("payload S \"%s\"\n", m_PayloadType.c_str());
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("tooltip S \"%s\"\n", m_TooltipText.c_str());
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
}
void DragDropNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "isSource") == 0) { int v; sscanf(line, " B %d", &v); m_IsSource = v != 0; }
    else if (strcmp(key, "isTarget") == 0) { int v; sscanf(line, " B %d", &v); m_IsTarget = v != 0; }
    else if (strcmp(key, "payload") == 0) { char v[128]; if (sscanf(line, " S \"%127[^\"]\"", v) >= 1) m_PayloadType = v; }
    else if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "tooltip") == 0) { char v[1024]; if (sscanf(line, " S \"%1023[^\"]\"", v) >= 1) m_TooltipText = v; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
}
void DragDropNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf))) m_Label = buf;
        ImGui::Checkbox("Is Drag Source", &m_IsSource);
        ImGui::Checkbox("Is Drop Target", &m_IsTarget);
        if (m_IsSource || m_IsTarget) {
            char pBuf[128];
            snprintf(pBuf, sizeof(pBuf), "%s", m_PayloadType.c_str());
            if (ImGui::InputText("Payload Type", pBuf, sizeof(pBuf))) m_PayloadType = pBuf;
        }
    }
    if (Node::PropSection("Tooltip")) {
        char tipBuf[1024];
        snprintf(tipBuf, sizeof(tipBuf), "%s", m_TooltipText.c_str());
        if (ImGui::InputTextMultiline("Tooltip Text", tipBuf, sizeof(tipBuf), ImVec2(0, 60)))
            m_TooltipText = tipBuf;
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
}

void DragDropNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;

    if (theme) {
        ImGui::PushStyleColor(ImGuiCol_Button, theme->m_Primary);
        ImGui::PushStyleColor(ImGuiCol_Text, theme->m_TextColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, theme->m_Rounding);
    }
    ImGui::Button(m_Label.empty() ? m_PayloadType.c_str() : m_Label.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));
    if (theme) ImGui::PopStyleColor(2);
    if (theme) ImGui::PopStyleVar(1);

    if (m_IsSource && ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload(m_PayloadType.c_str(), &m_Id, sizeof(m_Id));
        ImGui::TextUnformatted(m_Label.empty() ? m_PayloadType.c_str() : m_Label.c_str());
        if (!m_TooltipText.empty()) ImGui::TextUnformatted(m_TooltipText.c_str());
        ImGui::EndDragDropSource();
    }

    if (m_IsTarget && ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(m_PayloadType.c_str())) {
            // Payload accepted — this could trigger an action
            (void)p;
        }
        ImGui::EndDragDropTarget();
    }
}

