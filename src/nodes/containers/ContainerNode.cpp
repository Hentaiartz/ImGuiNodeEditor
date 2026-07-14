#include "ContainerNode.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>

ContainerNode::ContainerNode(int id, const std::string& name)
    : Node(id, name)
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 2, "Children", PinKind::Input, PinType::Container, id });
    m_Outputs.push_back({ id * 10000 + 100, "Container", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 100.0f);
}

void ContainerNode::Draw() {}

void ContainerNode::DrawProperties() {
    DrawBoxProperties();
}

static const char* kSizeModeNames[] = { "Auto", "Fill", "Fixed" };
static const char* kJustifyNames[]  = { "Start", "Center", "End", "Space Between" };
static const char* kAlignNames[]    = { "Start", "Center", "End", "Stretch" };

void ContainerNode::DrawBoxProperties() {
    if (Node::PropSection("Box")) {
        ImGui::Checkbox("Background", &m_HasBackground);
        ImGui::DragFloat("Padding", &m_Padding, 0.5f, 0.0f, 60.0f);

        int wm = (int)m_WidthMode;
        if (ImGui::Combo("Width", &wm, kSizeModeNames, 3)) m_WidthMode = (BoxSizeMode)wm;
        if (m_WidthMode == BoxSizeMode::Fixed)
            ImGui::DragFloat("##fw", &m_FixedSize.x, 1.0f, 10.0f, 2000.0f);

        int hm = (int)m_HeightMode;
        if (ImGui::Combo("Height", &hm, kSizeModeNames, 3)) m_HeightMode = (BoxSizeMode)hm;
        if (m_HeightMode == BoxSizeMode::Fixed)
            ImGui::DragFloat("##fh", &m_FixedSize.y, 1.0f, 10.0f, 2000.0f);

        int j = (int)m_Justify;
        if (ImGui::Combo("Justify", &j, kJustifyNames, 4)) m_Justify = (BoxJustify)j;

        int a = (int)m_Align;
        if (ImGui::Combo("Align", &a, kAlignNames, 4)) m_Align = (BoxAlign)a;
    }
}

void ContainerNode::SaveBoxExtra(ImGuiTextBuffer& b) const {
    b.appendf("hasBg I %d\n", (int)m_HasBackground);
    b.appendf("padding F %.1f\n", m_Padding);
    b.appendf("widthMode I %d\n", (int)m_WidthMode);
    b.appendf("heightMode I %d\n", (int)m_HeightMode);
    b.appendf("fixedSize V %.1f %.1f\n", m_FixedSize.x, m_FixedSize.y);
    b.appendf("justify I %d\n", (int)m_Justify);
    b.appendf("align I %d\n", (int)m_Align);
}

bool ContainerNode::LoadBoxProperty(const char* key, const char* line) {
    if (strcmp(key, "hasBg") == 0) { int v; if (sscanf(line, " I %d", &v) >= 1) m_HasBackground = v != 0; return true; }
    if (strcmp(key, "padding") == 0) { sscanf(line, " F %f", &m_Padding); return true; }
    if (strcmp(key, "widthMode") == 0) { int v; if (sscanf(line, " I %d", &v) >= 1) m_WidthMode = (BoxSizeMode)v; return true; }
    if (strcmp(key, "heightMode") == 0) { int v; if (sscanf(line, " I %d", &v) >= 1) m_HeightMode = (BoxSizeMode)v; return true; }
    if (strcmp(key, "fixedSize") == 0) { sscanf(line, " V %f %f", &m_FixedSize.x, &m_FixedSize.y); return true; }
    if (strcmp(key, "justify") == 0) { int v; if (sscanf(line, " I %d", &v) >= 1) m_Justify = (BoxJustify)v; return true; }
    if (strcmp(key, "align") == 0) { int v; if (sscanf(line, " I %d", &v) >= 1) m_Align = (BoxAlign)v; return true; }
    return false;
}

void ContainerNode::RenderPreview(const RenderContext& ctx) {
    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
    for (auto* c : ch) c->RenderPreview(ctx);
}

