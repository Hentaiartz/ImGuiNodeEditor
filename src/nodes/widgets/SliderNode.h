#pragma once
#include "NodeEditor/Node.h"

class SliderNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return m_Width > 0 ? m_Width : 200; }
    SliderNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Slider"; }
    const char* GetDisplayLabel() const override { return m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "Slider";
    float m_Value = 0.5f;
    float m_Min = 0.0f;
    float m_Max = 1.0f;
    std::string m_Format = "%.2f";
    float m_Width = 0.0f;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    bool m_CustomStyle = false;
    ImVec2 m_FramePadding = ImVec2(0, 0);
    float m_FrameRounding = 0.0f;
    float m_BorderSize = 0.0f;
    ImVec4 m_BorderColor = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    ImVec4 m_BgColor = ImVec4(0.26f, 0.42f, 0.70f, 1.0f);
    ImVec4 m_BgHoverColor = ImVec4(0.30f, 0.48f, 0.78f, 1.0f);
    ImVec4 m_BgActiveColor = ImVec4(0.22f, 0.35f, 0.60f, 1.0f);
    ImVec4 m_BorderHoverColor = ImVec4(0.7f, 0.7f, 0.8f, 1.0f);
    ImVec4 m_BorderActiveColor = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float m_FontScale = 1.0f;

    // Animation
    bool m_Initialized = false;
    double m_AnimStart = 0.0;
    float m_PrevValue = 0.5f;
    float m_LastFrameValue = 0.5f;
};
