#pragma once
#include "containers/ContainerNode.h"

class ChildNode : public ContainerNode {
public:
    float GetWidgetWidth(const RenderContext*) const override { return m_ChildSize.x > 0 ? m_ChildSize.x : 200; }
    ChildNode(int id);
    const char* GetTypeName() const override { return "Child"; }
    const char* GetLayoutName() const override { return "Child"; }
    const char* GetDisplayLabel() const override { return m_Title.empty() ? "Child" : m_Title.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Title = "Child";
    ImVec2 m_ChildSize = ImVec2(200, 150);
    bool m_Border = true;
    int m_Align = 0;
    ImVec2 m_FramePadding = ImVec2(0, 0);
};
