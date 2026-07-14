#pragma once
#include "NodeEditor/Node.h"

class SameLineNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return 0; }
    SameLineNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "SameLine"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    float m_Spacing = 0.0f;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
};
