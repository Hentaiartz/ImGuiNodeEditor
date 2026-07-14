#pragma once
#include "NodeEditor/Node.h"

class ButtonNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return m_ButtonSize.x; }
    ButtonNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Button"; }
    const char* GetDisplayLabel() const override { return m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "Button";
    ImVec2 m_ButtonSize = ImVec2(120, 30);
    bool m_Disabled = false;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    // Custom style overrides
    bool m_CustomStyle = false;
    ImVec4 m_BgColor      = ImVec4(0.26f, 0.42f, 0.70f, 1.0f);
    ImVec4 m_BgHoverColor = ImVec4(0.30f, 0.48f, 0.78f, 1.0f);
    ImVec4 m_BgActiveColor= ImVec4(0.22f, 0.35f, 0.60f, 1.0f);
    ImVec4 m_BorderColor  = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);
    ImVec4 m_BorderHoverColor = ImVec4(0.7f, 0.7f, 0.8f, 1.0f);
    ImVec4 m_BorderActiveColor = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);
    float m_BorderSize    = 0.0f;
    float m_FrameRounding = 4.0f;
    ImVec4 m_TextColor    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float m_FontScale     = 1.0f;
    ImVec2 m_FramePadding = ImVec2(0, 0); // 0=default
};
