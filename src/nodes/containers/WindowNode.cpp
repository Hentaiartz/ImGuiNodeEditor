#include "WindowNode.h"
#include "widgets/MenuBarNode.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>

WindowNode::WindowNode(int id)
    : ContainerNode(id, "Window")
{
    m_Size = ImVec2(NODE_WIDTH, 120.0f);
}

void WindowNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("title S \"%s\"\n", m_Title.c_str());
    b.appendf("windowSize V %.1f %.1f\n", m_WindowSize.x, m_WindowSize.y);
    b.appendf("flags I %d\n", m_Flags);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
}
void WindowNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "title") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Title = v; }
    else if (strcmp(key, "windowSize") == 0) { sscanf(line, " V %f %f", &m_WindowSize.x, &m_WindowSize.y); }
    else if (strcmp(key, "flags") == 0) { sscanf(line, " I %d", &m_Flags); }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
}
void WindowNode::DrawProperties() {
    ContainerNode::DrawProperties();

    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Title.c_str());
        if (ImGui::InputText("Title", buf, sizeof(buf)))
            m_Title = buf;

        ImGui::DragFloat2("Size", &m_WindowSize.x, 1.0f, 50.0f, 2000.0f);
    }
    if (Node::PropSection("Layout")) {
        ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
    }
    if (Node::PropSection("Settings")) {
        bool noTitleBar = (m_Flags & 1) != 0;
        bool noResize = (m_Flags & 2) != 0;
        bool noMove = (m_Flags & 4) != 0;
        bool noScrollbar = (m_Flags & 8) != 0;
        bool noCollapse = (m_Flags & 16) != 0;

        if (ImGui::Checkbox("No TitleBar", &noTitleBar)) m_Flags ^= 1;
        if (ImGui::Checkbox("No Resize", &noResize)) m_Flags ^= 2;
        if (ImGui::Checkbox("No Move", &noMove)) m_Flags ^= 4;
        if (ImGui::Checkbox("No Scrollbar", &noScrollbar)) m_Flags ^= 8;
        if (ImGui::Checkbox("No Collapse", &noCollapse)) m_Flags ^= 16;
    }
}

void WindowNode::RenderPreview(const RenderContext& ctx) {
    ThemeNode* wt = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!wt) wt = ctx.theme;
    RenderHelpers::PushContainerStyle(wt);
    char wndId[64]; snprintf(wndId, sizeof(wndId), "%s##wnd%d", m_Title.c_str(), m_Id);
    ImGuiWindowFlags wf = ImGuiWindowFlags_None;
    if (m_Flags & 1) wf |= ImGuiWindowFlags_NoTitleBar;
    if (m_Flags & 2) wf |= ImGuiWindowFlags_NoResize;
    if (m_Flags & 4) wf |= ImGuiWindowFlags_NoMove;
    if (m_Flags & 8) wf |= ImGuiWindowFlags_NoScrollbar;
    if (m_Flags & 16) wf |= ImGuiWindowFlags_NoCollapse;
    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
    for (auto* c : ch) if (dynamic_cast<MenuBarNode*>(c)) { wf |= ImGuiWindowFlags_MenuBar; break; }
    if (m_FramePadding.x > 0 || m_FramePadding.y > 0) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_FramePadding);
    if (wt) ImGui::PushStyleColor(ImGuiCol_Text, wt->m_TitleText); // title bar text
    ImGui::SetNextWindowSize(m_WindowSize, ImGuiCond_Appearing);
    bool beginResult = ImGui::Begin(wndId, nullptr, wf);
    if (wt) ImGui::PopStyleColor(); // pop TitleText
    if (beginResult) {
        if (wt) ImGui::PushStyleColor(ImGuiCol_Text, wt->m_TextColor); // content text
        auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
        for (auto* c : ch) c->RenderPreview(ctx);
        if (wt) ImGui::PopStyleColor(); // pop content text
    }
    ImGui::End();
    if (m_FramePadding.x > 0 || m_FramePadding.y > 0) ImGui::PopStyleVar();
    RenderHelpers::PopStyle(wt, 6, 13);
}

