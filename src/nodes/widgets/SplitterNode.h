#pragma once
#include "NodeEditor/Node.h"

class SplitterNode : public Node {
public:
    SplitterNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Splitter"; }
    const char* GetDisplayLabel() const override { return "Splitter"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    bool m_Vertical = false;    // false = horizontal split
    float m_InitialRatio = 0.5f;
    float m_Thickness = 4.0f;
    float m_MinPane = 30.0f;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
};
