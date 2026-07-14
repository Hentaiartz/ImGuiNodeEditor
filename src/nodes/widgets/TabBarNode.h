#pragma once
#include "containers/ContainerNode.h"

class TabBarNode : public ContainerNode {
public:
    TabBarNode(int id);
    const char* GetTypeName() const override { return "TabBar"; }
    const char* GetLayoutName() const override { return "TabBar"; }
    const char* GetDisplayLabel() const override { return "TabBar"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "TabBar";
    int m_Align = 0;
};
