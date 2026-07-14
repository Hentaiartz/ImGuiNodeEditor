#pragma once
#include "containers/ContainerNode.h"

class SubMenuNode : public ContainerNode {
public:
    SubMenuNode(int id);
    const char* GetTypeName() const override { return "SubMenu"; }
    const char* GetLayoutName() const override { return "SubMenu"; }
    const char* GetDisplayLabel() const override { return m_Label.empty() ? "SubMenu" : m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "Menu";
    int m_Align = 0;
    double m_AnimStart = -1.0;
    bool m_WasOpen = false;
    bool m_Initialized = false;
};
