#include "BadgeNode.h"
#include "AnimationNode.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>

BadgeNode::BadgeNode(int id)
    : Node(id, "Badge")
{
    m_Inputs.push_back({ id * 10000 + 1, "Theme", PinKind::Input, PinType::Theme, id });
    m_Inputs.push_back({ id * 10000 + 3, "Anim", PinKind::Input, PinType::Animation, id });
    m_Outputs.push_back({ id * 10000 + 2, "Widget", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void BadgeNode::Draw() {}

void BadgeNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("text S \"%s\"\n", m_Text.c_str());
    b.appendf("color C %.3f %.3f %.3f %.3f\n", m_Color.x, m_Color.y, m_Color.z, m_Color.w);
    b.appendf("fontScale F %.1f\n", m_FontScale);
    b.appendf("align I %d\n", m_Align);
    b.appendf("offset V %.1f %.1f\n", m_Offset.x, m_Offset.y);
    b.appendf("pulseEnabled B %d\n", (int)m_PulseEnabled);
    b.appendf("customStyle B %d\n", (int)m_CustomStyle);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("borderSize F %.1f\n", m_BorderSize);
    b.appendf("frameRounding F %.1f\n", m_FrameRounding);
    b.appendf("framePadding V %.1f %.1f\n", m_FramePadding.x, m_FramePadding.y);
}
void BadgeNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "text") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Text = v; }
    else if (strcmp(key, "color") == 0) { sscanf(line, " C %f %f %f %f", &m_Color.x, &m_Color.y, &m_Color.z, &m_Color.w); }
    else if (strcmp(key, "fontScale") == 0) { sscanf(line, " F %f", &m_FontScale); }
    else if (strcmp(key, "align") == 0) { sscanf(line, " I %d", &m_Align); }
    else if (strcmp(key, "offset") == 0) { sscanf(line, " V %f %f", &m_Offset.x, &m_Offset.y); }
    else if (strcmp(key, "pulseEnabled") == 0) { int v; sscanf(line, " B %d", &v); m_PulseEnabled = v != 0; }
    else if (strcmp(key, "customStyle") == 0) { int v; sscanf(line, " B %d", &v); m_CustomStyle = v != 0; }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "borderSize") == 0) { sscanf(line, " F %f", &m_BorderSize); }
    else if (strcmp(key, "frameRounding") == 0) { sscanf(line, " F %f", &m_FrameRounding); }
    else if (strcmp(key, "framePadding") == 0) { sscanf(line, " V %f %f", &m_FramePadding.x, &m_FramePadding.y); }
}
void BadgeNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Text.c_str());
        if (ImGui::InputText("Text", buf, sizeof(buf)))
            m_Text = buf;
    }
    if (Node::PropSection("Position")) {
        ImGui::Combo("Align", &m_Align, "Left\0Center\0Right\0\0");
        ImGui::DragFloat2("Offset (px)", &m_Offset.x, 1.0f, -10000.0f, 10000.0f);
    }
    if (Node::PropSection("Style")) {
        ImGui::Checkbox("Pulse Animation", &m_PulseEnabled);
        ImGui::Checkbox("Custom Style", &m_CustomStyle);
        if (m_CustomStyle) {
            ImGui::ColorEdit4("Color", &m_Color.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Border Color", &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
            ImGui::DragFloat("Border Size", &m_BorderSize, 0.5f, 0.0f, 10.0f);
            ImGui::DragFloat("Rounding", &m_FrameRounding, 0.5f, 0.0f, 20.0f);
            ImGui::DragFloat("Font Scale", &m_FontScale, 0.05f, 0.5f, 3.0f);
            ImGui::DragFloat2("Frame Padding", &m_FramePadding.x, 1.0f, 0.0f, 40.0f);
            ImGui::ColorEdit4("Text Color", &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        }
    }
}

void BadgeNode::RenderPreview(const RenderContext& ctx) {
    RenderHelpers::ApplyWidgetAlign(m_Align, RenderHelpers::GetWidgetWidth(this), m_Offset);

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
    ThemeNode* theme = RenderHelpers::FindNodeTheme(this, ctx.links, ctx.nodes);
    if (!theme) theme = ctx.theme;

    float dur = animNode ? animNode->m_Duration : 0.8f;
    if (!m_Initialized) {
        m_Initialized = true;
        m_AnimStart = ImGui::GetTime();
    }

    // Continuous pulse: oscillate 0→1→0 as a smooth wave
    double elapsed = ImGui::GetTime() - m_AnimStart;
    float pulseT = (float)fmod(elapsed, (double)dur) / dur; // 0→1 loop
    // Sine wave for smooth pulse
    float pulse = sinf(pulseT * 3.14159265f); // 0→1→0

    ImVec4 bgColor   = m_CustomStyle ? m_Color   : (theme ? theme->m_Primary   : ImVec4(0.49f,0.42f,1.0f,1.0f));
    ImVec4 textColor = m_CustomStyle ? m_TextColor : (theme ? theme->m_TextColor : ImVec4(1,1,1,1));
    float rounding = m_CustomStyle && m_FrameRounding > 0 ? m_FrameRounding : (theme ? theme->m_Rounding : 10.0f);
    float pad = m_CustomStyle && m_FramePadding.x > 0 ? m_FramePadding.x : (theme ? theme->m_Spacing * 0.5f : 4.0f);
    float padY = m_CustomStyle && m_FramePadding.y > 0 ? m_FramePadding.y : pad;

    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(m_FontScale);
    ImVec2 textSize = ImGui::CalcTextSize(m_Text.c_str());
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImVec2 rectMin = p - ImVec2(pad, padY);
    ImVec2 rectMax = p + textSize + ImVec2(pad * 2, padY);

    // Glow ring (pulses outward)
    if (m_PulseEnabled) {
        float glowExpand = pulse * 4.0f;
        float glowAlpha = pulse * 0.35f;
        int gx = (int)(bgColor.x * 255 * 1.2f);
        int gy = (int)(bgColor.y * 255 * 1.2f);
        int gz = (int)(bgColor.z * 255 * 1.2f);
        if (gx > 255) gx = 255;
        if (gy > 255) gy = 255;
        if (gz > 255) gz = 255;
        ImU32 glowCol = IM_COL32(gx, gy, gz, (int)(glowAlpha * 255));
        dl->AddRectFilled(rectMin - ImVec2(glowExpand, glowExpand),
                          rectMax + ImVec2(glowExpand, glowExpand),
                          glowCol, rounding + glowExpand);
    }

    // Main bg
    ImU32 bgCol = IM_COL32((int)(bgColor.x*255), (int)(bgColor.y*255), (int)(bgColor.z*255), (int)(bgColor.w*255));
    dl->AddRectFilled(rectMin, rectMax, bgCol, rounding);

    // Border
    if (m_CustomStyle && m_BorderSize > 0) {
        dl->AddRect(rectMin, rectMax, ImColor(m_BorderColor), rounding, 0, m_BorderSize);
    }

    // Text
    ImGui::SetCursorScreenPos(p);
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::TextUnformatted(m_Text.c_str());
    ImGui::PopStyleColor();

    if (m_FontScale != 1.0f) ImGui::SetWindowFontScale(1.0f);
}


