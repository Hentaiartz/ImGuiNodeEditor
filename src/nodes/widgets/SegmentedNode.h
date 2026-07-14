#pragma once
#include "NodeEditor/Node.h"

class SegmentedNode : public Node {
public:
    SegmentedNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Segmented"; }
    const char* GetDisplayLabel() const override { return "Segmented"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Items = std::string("Option 1\0Option 2\0Option 3", 27);
    int m_Selected = 0;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    bool m_CustomStyle = false;
    ImVec4 m_BgColor = ImVec4(0.31f, 0.27f, 0.78f, 1.0f);
    ImVec4 m_BgHoverColor = ImVec4(0.36f, 0.32f, 0.88f, 1.0f);
    ImVec4 m_BgActiveColor = ImVec4(0.26f, 0.22f, 0.68f, 1.0f);
    ImVec4 m_BorderColor = ImVec4(0.22f, 0.24f, 0.32f, 1.0f);
    float m_BorderSize = 1.0f;
    float m_FrameRounding = 4.0f;
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float m_FontScale = 1.0f;
    ImVec2 m_FramePadding = ImVec2(0, 0);

    // Animation
    bool m_Initialized = false;
    double m_AnimStart = 0.0;
    int m_PrevSelected = 0;
    int m_LastFrameSelected = 0;
};
