#pragma once
#include "NodeEditor/Node.h"

class BadgeNode : public Node {
public:
    BadgeNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Badge"; }
    const char* GetDisplayLabel() const override { return m_Text.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Text = "3";
    ImVec4 m_Color = ImVec4(0.49f, 0.42f, 1.0f, 1.0f);
    float m_FontScale = 0.7f;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    bool m_PulseEnabled = true;
    bool m_CustomStyle = false;
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 m_BorderColor = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);
    float m_BorderSize = 0.0f;
    float m_FrameRounding = 10.0f;
    ImVec2 m_FramePadding = ImVec2(0, 0);
    double m_AnimStart = -1.0;
    bool m_Initialized = false;
};
