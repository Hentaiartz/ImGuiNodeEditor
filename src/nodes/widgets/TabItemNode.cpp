#include "TabItemNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>

TabItemNode::TabItemNode(int id)
    : ContainerNode(id, "Tab")
{
}

void TabItemNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("align I %d\n", m_Align);
}
void TabItemNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
}
void TabItemNode::DrawProperties() {
    ContainerNode::DrawProperties();
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf)))
            m_Label = buf;
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
    }
}

void TabItemNode::RenderPreview(const RenderContext& ctx) {
    if (ImGui::GetCurrentTabBar()) {
        if (ImGui::BeginTabItem(m_Label.c_str())) {
            auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
            for (auto* c : ch) c->RenderPreview(ctx);
            ImGui::EndTabItem();
        }
    } else {
        auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
        for (auto* c : ch) c->RenderPreview(ctx);
    }
}


