#pragma once
#include "NodeEditor/Node.h"

class SeparatorNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return 0; }
    SeparatorNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Separator"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    float m_Thickness = 1.0f;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    ImVec4 m_Color = ImVec4(1, 1, 1, 1);
    bool m_UseCustomColor = false;
    bool m_CustomStyle = false;
    ImVec2 m_FramePadding = ImVec2(0, 0);
    float m_FrameRounding = 0.0f;
    float m_BorderSize = 0.0f;
    ImVec4 m_BorderColor = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
};
