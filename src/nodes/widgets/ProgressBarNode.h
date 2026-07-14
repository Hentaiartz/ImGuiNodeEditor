#pragma once
#include "NodeEditor/Node.h"

class ProgressBarNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return m_BarSize.x > 0 ? m_BarSize.x : 200; }
    ProgressBarNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "ProgressBar"; }
    const char* GetDisplayLabel() const override { return "ProgressBar"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    float m_Value = 0.5f;
    ImVec2 m_BarSize = ImVec2(200, 20);
    char m_Overlay[256] = "";
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    bool m_CustomStyle = false;
    ImVec4 m_BgColor = ImVec4(0.20f, 0.22f, 0.27f, 1.0f);
    ImVec4 m_BgHoverColor = ImVec4(0.49f, 0.42f, 1.0f, 1.0f);
    ImVec4 m_BorderColor = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);
    ImVec4 m_BorderHoverColor = ImVec4(0.7f, 0.7f, 0.8f, 1.0f);
    ImVec4 m_BorderActiveColor = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);
    float m_BorderSize = 0.0f;
    float m_FrameRounding = 4.0f;
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float m_FontScale = 1.0f;
    double m_AnimStart = -1.0;
    float m_PrevValue = 0.5f;
    float m_LastFrameValue = 0.5f;
    bool m_Initialized = false;
};
