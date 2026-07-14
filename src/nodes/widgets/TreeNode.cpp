#include "TreeNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

TreeNode::TreeNode(int id)
    : Node(id, "TreeNode")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 2, "Children", PinKind::Input, PinType::Container, id });
    m_Inputs.push_back({ id * 10000 + 4, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 100, "Container", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void TreeNode::Draw() {}

void TreeNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("defaultOpen B %d\n", (int)m_DefaultOpen);
    b.appendf("leaf B %d\n", (int)m_Leaf);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
}
void TreeNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "defaultOpen") == 0) { int v; sscanf(line, " B %d", &v); m_DefaultOpen = v != 0; }
    else if (strcmp(key, "leaf") == 0) { int v; sscanf(line, " B %d", &v); m_Leaf = v != 0; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
}
void TreeNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Label.c_str());
        if (ImGui::InputText("Label", buf, sizeof(buf))) m_Label = buf;
        ImGui::Checkbox("Default Open", &m_DefaultOpen);
        ImGui::Checkbox("Leaf", &m_Leaf);
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
}

void TreeNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);
    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;

    // Find connected AnimationNode
    static auto findAnimNode = [](const Node* self, const RenderContext& cr) -> AnimationNode* {
        int animPinId = -1;
        for (auto& p : self->m_Inputs)
            if (p.type == PinType::Animation) { animPinId = p.id; break; }
        if (animPinId < 0) return nullptr;
        for (auto& l : cr.links) {
            if (l.toPinId != animPinId) continue;
            for (auto& n : cr.nodes)
                for (auto& op : n->m_Outputs)
                    if (op.id == l.fromPinId && op.type == PinType::Animation)
                        return dynamic_cast<AnimationNode*>(n.get());
        }
        return nullptr;
    };
    AnimationNode* animNode = findAnimNode(this, ctx);

    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);

    if (!m_Initialized) {
        m_Initialized = true;
        m_Open = m_DefaultOpen;
        m_PrevOpen = m_Open;
        m_LastFrameOpen = m_Open;
    }

    // Edge detection
    if (m_Open != m_LastFrameOpen) {
        m_PrevOpen = m_LastFrameOpen;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameOpen = m_Open;
    }

    // Animation progress
    float dur = animNode ? animNode->m_Duration : 0.2f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float from = m_PrevOpen ? 1.0f : 0.0f;
    float to   = m_Open ? 1.0f : 0.0f;
    float animNorm = from + (to - from) * t; // 0 = closed → 1 = open

    // Custom tree node draw
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float fs = ImGui::GetFontSize();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float availWidth = ImGui::GetContentRegionAvail().x;

    // Full-width hit area
    float itemHeight = fs + ImGui::GetStyle().FramePadding.y * 2;
    ImVec2 rectMin = pos;
    ImVec2 rectMax = ImVec2(pos.x + availWidth, pos.y + itemHeight);

    // Hover / active
    auto& io = ImGui::GetIO();
    bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax);
    bool active = hovered && io.MouseDown[0];
    if (hovered) {
        ImU32 hoverCol = theme ? IM_COL32((int)(theme->m_PrimaryHover.x*255), (int)(theme->m_PrimaryHover.y*255), (int)(theme->m_PrimaryHover.z*255), active ? 30 : 15) : IM_COL32(255,255,255, active ? 25 : 12);
        dl->AddRectFilled(rectMin, rectMax, hoverCol);
    }

    // Arrow triangle
    float arrowSize = fs * 0.5f;
    ImVec2 arrowCenter = ImVec2(pos.x + arrowSize + 4, pos.y + itemHeight * 0.5f);
    float angle = animNorm * 3.14159265f * 0.5f; // 0 (right) → 90° (down)

    if (!m_Leaf) {
        // Draw arrow as a filled triangle (pointing right by default, rotated down)
        ImVec2 a = ImVec2(arrowCenter.x + cosf(angle) * arrowSize * 0.7f - sinf(angle) * 0,
                          arrowCenter.y + sinf(angle) * arrowSize * 0.7f + cosf(angle) * 0);
        ImVec2 b = ImVec2(arrowCenter.x + cosf(angle + 2.094f) * arrowSize * 0.7f - sinf(angle + 2.094f) * 0,
                          arrowCenter.y + sinf(angle + 2.094f) * arrowSize * 0.7f + cosf(angle + 2.094f) * 0);
        ImVec2 c = ImVec2(arrowCenter.x + cosf(angle + 4.188f) * arrowSize * 0.7f - sinf(angle + 4.188f) * 0,
                          arrowCenter.y + sinf(angle + 4.188f) * arrowSize * 0.7f + cosf(angle + 4.188f) * 0);
        ImU32 arrowCol = theme ? IM_COL32((int)(theme->m_TextColor.x*255), (int)(theme->m_TextColor.y*255), (int)(theme->m_TextColor.z*255), 255) : IM_COL32(180,180,200,255);
        dl->AddTriangleFilled(a, b, c, arrowCol);
    }

    // Label
    ImVec2 textPos = ImVec2(pos.x + arrowSize + 8 + ImGui::GetStyle().FramePadding.x, pos.y + ImGui::GetStyle().FramePadding.y);
    ImU32 labelCol = theme ? IM_COL32((int)(theme->m_TextColor.x*255), (int)(theme->m_TextColor.y*255), (int)(theme->m_TextColor.z*255), 255) : IM_COL32(200,200,215,255);
    dl->AddText(textPos, labelCol, m_Label.c_str());

    // Invisible button for click + children rendering
    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(m_Label.c_str(), ImVec2(availWidth, itemHeight));
    if (ImGui::IsItemClicked() && !m_Leaf) {
        m_Open = !m_Open;
    }

    // Render children (animated reveal)
    bool showChildren = (m_Open || animNorm > 0.01f) && !m_Leaf && !ch.empty();
    if (showChildren) {
        ImGui::Indent();
        ImVec2 startScreen = ImGui::GetCursorScreenPos();
        float clipH = m_ChildrenHeight * animNorm;
        if (clipH < 1.0f) clipH = 1.0f;
        ImGui::PushClipRect(startScreen, ImVec2(startScreen.x + 10000.0f, startScreen.y + clipH), false);

        float startY = ImGui::GetCursorPosY();
        for (auto* c : ch) c->RenderPreview(ctx);
        m_ChildrenHeight = ImGui::GetCursorPosY() - startY;

        ImGui::PopClipRect();
        ImGui::Unindent();
    }
}

