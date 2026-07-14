#pragma once
#include "NodeEditor/Node.h"

class TextNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override;
    TextNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Text"; }
    const char* GetDisplayLabel() const override { return m_Text.empty() ? "Text" : m_Text.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Text = "Text";
    ImVec4 m_Color = ImVec4(0.85f, 0.87f, 0.90f, 1.0f);
    float m_WrapWidth = 0.0f;
    int m_Align = 0; // 0=left, 1=center, 2=right
    ImVec2 m_Offset = ImVec2(0, 0);
    bool m_UseCustomColor = false;
    float m_FontScale = 1.0f;
    bool m_CustomStyle = false;
    ImVec2 m_FramePadding = ImVec2(0, 0);
    float m_FrameRounding = 0.0f;
    float m_BorderSize = 0.0f;
    ImVec4 m_BorderColor = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    ImVec4 m_BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
};
