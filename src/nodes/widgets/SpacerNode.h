#pragma once
#include "NodeEditor/Node.h"

class SpacerNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return m_WidgetSize.x; }
    SpacerNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Spacer"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    ImVec2 m_WidgetSize = ImVec2(0, 8);
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
};
