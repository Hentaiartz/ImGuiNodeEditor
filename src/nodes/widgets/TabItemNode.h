#pragma once
#include "containers/ContainerNode.h"

class TabItemNode : public ContainerNode {
public:
    TabItemNode(int id);
    const char* GetTypeName() const override { return "TabItem"; }
    const char* GetLayoutName() const override { return "TabItem"; }
    const char* GetDisplayLabel() const override { return m_Label.empty() ? "Tab" : m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "Tab";
    int m_Align = 0;
};
