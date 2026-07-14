#pragma once
#include "ContainerNode.h"

class WindowNode : public ContainerNode {
public:
    float GetWidgetWidth(const RenderContext*) const override { return m_WindowSize.x; }
    WindowNode(int id);
    const char* GetTypeName() const override { return "Window"; }
    const char* GetLayoutName() const override { return "Window"; }
    const char* GetDisplayLabel() const override { return m_Title.empty() ? "Window" : m_Title.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Title = "Menu";
    ImVec2 m_WindowSize = ImVec2(300, 400);
    int m_Flags = 0;
    ImVec2 m_FramePadding = ImVec2(0, 0);
};
