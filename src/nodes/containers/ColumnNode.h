#pragma once
#include "ContainerNode.h"

class ColumnNode : public ContainerNode {
public:
    float GetWidgetWidth(const RenderContext* ctx) const override;
    ColumnNode(int id);
    const char* GetTypeName() const override { return "Column"; }
    const char* GetLayoutName() const override { return "Column"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    float m_Spacing = 8.0f;
    float m_Width = 0.0f;
};
