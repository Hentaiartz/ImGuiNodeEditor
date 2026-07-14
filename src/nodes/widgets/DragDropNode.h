#pragma once
#include "NodeEditor/Node.h"

class DragDropNode : public Node {
public:
    DragDropNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "DragDrop"; }
    const char* GetDisplayLabel() const override { return m_Label.empty() ? "DragDrop" : m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    bool m_IsSource = true;
    bool m_IsTarget = true;
    std::string m_PayloadType = "Default";
    std::string m_Label = "";
    std::string m_TooltipText = "";
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
};
