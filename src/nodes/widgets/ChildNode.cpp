#include "ChildNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>

ChildNode::ChildNode(int id)
    : ContainerNode(id, "Child")
{
}

void ChildNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("title S \"%s\"\n", m_Title.c_str());
    b.appendf("childSize V %.1f %.1f\n", m_ChildSize.x, m_ChildSize.y);
    b.appendf("border B %d\n", (int)m_Border);
    b.appendf("align I %d\n", m_Align);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
}
void ChildNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "title") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Title = v; }
    else if (strcmp(key, "childSize") == 0) { sscanf(line, " V %f %f", &m_ChildSize.x, &m_ChildSize.y); }
    else if (strcmp(key, "border") == 0) { int v; sscanf(line, " B %d", &v); m_Border = v != 0; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
}
void ChildNode::DrawProperties() {
    ContainerNode::DrawProperties();
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Title.c_str());
        if (ImGui::InputText("Title", buf, sizeof(buf)))
            m_Title = buf;
        ImGui::DragFloat2("Size", &m_ChildSize.x, 1.0f, 20.0f, 2000.0f);
        ImGui::Checkbox("Border", &m_Border);
    }
    if (Node::PropSection("Layout")) {
        ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
    }
}

void ChildNode::RenderPreview(const RenderContext& ctx) {
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;
    RenderHelpers::PushContainerStyle(wt);
    if (m_FramePadding.x > 0 || m_FramePadding.y > 0) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_FramePadding);
    ImGui::BeginChild(m_Title.c_str(), m_ChildSize, m_Border);
    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
    for (auto* c : ch) c->RenderPreview(ctx);
    ImGui::EndChild();
    if (m_FramePadding.x > 0 || m_FramePadding.y > 0) ImGui::PopStyleVar();
    RenderHelpers::PopStyle(wt, 6, 13);
}


