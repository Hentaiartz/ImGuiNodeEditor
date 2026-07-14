#pragma once
#include "NodeEditor/Node.h"

class GridNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return 0; }
    GridNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Grid"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    int m_Columns = 2;
    float m_Spacing = 14.0f;
    float m_Padding = 0.0f;
    std::vector<float> m_Weights;
    bool m_CustomStyle = false;
    ImVec4 m_BorderColor = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    float m_BorderSize = 0.0f;
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
};
