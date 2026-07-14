#pragma once
#include "NodeEditor/Node.h"

class TooltipNode : public Node {
public:
    TooltipNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Tooltip"; }
    const char* GetDisplayLabel() const override { return m_Label.empty() ? "Tooltip" : m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "";
    std::string m_Text = "Tooltip text";
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
};
