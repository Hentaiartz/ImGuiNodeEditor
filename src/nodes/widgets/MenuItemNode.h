#pragma once
#include "NodeEditor/Node.h"

class MenuItemNode : public Node {
public:
    MenuItemNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "MenuItem"; }
    const char* GetDisplayLabel() const override { return m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "MenuItem";
    std::string m_Shortcut;
    bool m_Checkable = false;
    bool m_Checked = false;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    bool m_CustomStyle = false;
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float m_FontScale = 1.0f;
};
