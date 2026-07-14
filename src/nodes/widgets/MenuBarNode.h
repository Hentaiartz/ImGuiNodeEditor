#pragma once
#include "containers/ContainerNode.h"

class MenuBarNode : public ContainerNode {
public:
    MenuBarNode(int id);
    const char* GetTypeName() const override { return "MenuBar"; }
    const char* GetLayoutName() const override { return "MenuBar"; }
    const char* GetDisplayLabel() const override { return "MenuBar"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "MenuBar";
    int m_Align = 0;
};
