#pragma once
#include "NodeEditor/Node.h"

class TreeNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return 0; }
    TreeNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "TreeNode"; }
    const char* GetDisplayLabel() const override { return m_Label.empty() ? "TreeNode" : m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "Node";
    bool m_DefaultOpen = true;
    bool m_Leaf = false;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    bool m_Open = true;
    double m_AnimStart = -1.0;
    bool m_PrevOpen = true;
    bool m_LastFrameOpen = true;
    bool m_Initialized = false;
    float m_ChildrenHeight = 100.0f;
};
