#pragma once
#include "ContainerNode.h"

class RowNode : public ContainerNode {
public:
    float GetWidgetWidth(const RenderContext*) const override { return 0; }
    RowNode(int id);
    const char* GetTypeName() const override { return "Row"; }
    const char* GetLayoutName() const override { return "Row"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    float m_Spacing = 8.0f;
};
