#pragma once
#include "NodeEditor/Node.h"

class ToggleSwitchNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return 50.0f; }
    ToggleSwitchNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "ToggleSwitch"; }
    const char* GetDisplayLabel() const override { return m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "Toggle";
    bool m_On = false;
    bool m_Disabled = false;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    float m_Width = 0.0f;
    float m_Scale = 1.0f;
    bool m_CustomStyle = false;
    ImVec4 m_Accent = ImVec4(0.49f, 0.42f, 1.0f, 1.0f);
    ImVec4 m_KnobColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 m_KnobColorOff = ImVec4(0.55f, 0.55f, 0.6f, 1.0f);
    float m_FrameRounding = 20.0f;
    ImVec4 m_BgColor = ImVec4(0.15f, 0.15f, 0.20f, 1.0f);
    ImVec4 m_BgHoverColor = ImVec4(0.18f, 0.18f, 0.24f, 1.0f);
    ImVec4 m_BgActiveColor = ImVec4(0.12f, 0.12f, 0.16f, 1.0f);
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 m_BorderColor = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    ImVec4 m_BorderHoverColor = ImVec4(0.5f, 0.5f, 0.55f, 1.0f);
    ImVec4 m_BorderActiveColor = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
    float m_BorderSize = 0.0f;
    float m_FontScale = 1.0f;
    ImVec2 m_FramePadding = ImVec2(0, 0);
    // Animation state (transient, not saved)
    bool m_Initialized = false;
    bool m_PrevOn = false;
    bool m_LastFrameOn = false;
    bool m_LastFrameDisabled = false;
    double m_AnimStart = 0.0;
};
