#include "PopupNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <cstdio>
#include <math.h>

PopupNode::PopupNode(int id)
    : ContainerNode(id, "Popup")
{
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Size = ImVec2(NODE_WIDTH, 80.0f);
}

void PopupNode::SaveExtra(ImGuiTextBuffer& b) const {
    SaveBoxExtra(b);
    b.appendf("title S \"%s\"\n", m_Title.c_str());
    b.appendf("modal B %d\n", (int)m_Modal);
}
void PopupNode::LoadExtra(const char* key, const char* line) {
    if (LoadBoxProperty(key, line)) return;
    if (strcmp(key, "title") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Title = v; }
    else if (strcmp(key, "modal") == 0) { int v; sscanf(line, " B %d", &v); m_Modal = v != 0; }
}
void PopupNode::DrawProperties() {
    ContainerNode::DrawProperties();
    if (Node::PropSection("Popup")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Title.c_str());
        if (ImGui::InputText("Title", buf, sizeof(buf))) m_Title = buf;
        ImGui::Checkbox("Modal", &m_Modal);
        if (ImGui::Button(m_Open ? "Close" : "Open Test")) m_Open = !m_Open;
    }
}

void PopupNode::RenderPreview(const RenderContext& ctx) {
    auto ch = RenderHelpers::GetChildren(this, ctx.links, ctx.nodes);
    char popupId[64]; snprintf(popupId, sizeof(popupId), "##popup_%d", m_Id);

    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;

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

    if (!m_Initialized) {
        m_Initialized = true;
        m_PrevOpen = m_Open;
        m_LastFrameOpen = m_Open;
        m_CloseAnimating = false;
    }

    // Edge detection
    if (m_Open != m_LastFrameOpen) {
        m_PrevOpen = m_LastFrameOpen;
        m_AnimStart = ImGui::GetTime();
        m_LastFrameOpen = m_Open;
        if (!m_Open) m_CloseAnimating = true;
    }

    // Keep popup alive during close animation
    bool keepAlive = m_Open || m_CloseAnimating;
    if (keepAlive) ImGui::OpenPopup(popupId);

    // Animation progress
    float dur = animNode ? animNode->m_Duration : 0.2f;
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float t = (dur <= 0.0f || elapsed >= dur) ? 1.0f : (float)(elapsed / dur);
    t = AnimationNode::ApplyEasing(t, animNode ? animNode->m_Easing : "", animNode);

    float animAlpha;
    if (m_CloseAnimating) {
        // Closing: fade out
        animAlpha = 1.0f - t;
        if (t >= 1.0f) {
            m_CloseAnimating = false;
            animAlpha = 0.0f;
        }
    } else {
        // Opening or static: fade in
        float from = m_PrevOpen ? 1.0f : 0.0f;
        float to   = m_Open ? 1.0f : 0.0f;
        animAlpha = from + (to - from) * t;
    }

    if (m_Modal) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        float bgAlpha = animAlpha * 0.7f;
        ImGui::SetNextWindowBgAlpha(bgAlpha);
        if (ImGui::BeginPopupModal(m_Title.c_str(), &m_Open)) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, animAlpha * ImGui::GetStyle().Alpha);
            for (auto* c : ch) c->RenderPreview(ctx);
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
    } else {
        if (ImGui::BeginPopup(popupId)) {
            if (!m_Title.empty()) {
                ImGui::TextUnformatted(m_Title.c_str());
                ImGui::Separator();
            }
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, animAlpha * ImGui::GetStyle().Alpha);
            for (auto* c : ch) c->RenderPreview(ctx);
            ImGui::PopStyleVar();
            ImGui::EndPopup();
        }
    }
}
