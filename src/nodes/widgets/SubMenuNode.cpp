#include "SubMenuNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

SubMenuNode::SubMenuNode(int id)
    : ContainerNode(id, "SubMenu")
{
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Size = ImVec2(NODE_WIDTH, 100.0f);
}

void SubMenuNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("label S \"%s\"\n", m_Label.c_str());
    b.appendf("align I %d\n", m_Align);
}
void SubMenuNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "label") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Label = v; }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
}
void SubMenuNode::DrawProperties() {
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

void SubMenuNode::RenderPreview(const RenderContext& ctx) {
    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);

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

    bool isOpen = ImGui::BeginMenu(m_Label.c_str());

    if (isOpen) {
        if (!m_WasOpen) {
            m_AnimStart = ImGui::GetTime();
            m_WasOpen = true;
        }

        // Fade progress
        float dur = animNode ? animNode->m_Duration : 0.15f;
        double elapsed = ImGui::GetTime() - m_AnimStart;
        float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, t * ImGui::GetStyle().Alpha);
        for (auto* c : ch) c->RenderPreview(ctx);
        ImGui::PopStyleVar();
        ImGui::EndMenu();
    } else {
        m_WasOpen = false;
    }
}


