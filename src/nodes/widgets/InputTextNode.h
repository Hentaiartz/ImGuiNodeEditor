#pragma once
#include "NodeEditor/Node.h"

class InputTextNode : public Node {
public:
    float GetWidgetWidth(const RenderContext*) const override { return m_Width > 0 ? m_Width : 180; }
    InputTextNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "InputText"; }
    const char* GetDisplayLabel() const override { return m_Label.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Label = "Input";
    std::string m_Text;
    int m_MaxLength = 256;
    bool m_Password = false;
    float m_Width = 0.0f;
    float m_Height = 0.0f;
    ImVec2 m_FramePadding = ImVec2(0, 0);
    ImVec4 m_BorderColor = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    float m_BorderSize = 1.0f;
    float m_Rounding = 4.0f;
    bool m_CustomStyle = false;
    int m_Align = 0;
    ImVec2 m_Offset = ImVec2(0, 0);
    ImVec4 m_BgColor = ImVec4(0.26f, 0.42f, 0.70f, 1.0f);
    ImVec4 m_BgHoverColor = ImVec4(0.30f, 0.48f, 0.78f, 1.0f);
    ImVec4 m_BgActiveColor = ImVec4(0.22f, 0.35f, 0.60f, 1.0f);
    ImVec4 m_BorderHoverColor = ImVec4(0.7f, 0.7f, 0.8f, 1.0f);
    ImVec4 m_BorderActiveColor = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);
    ImVec4 m_TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float m_FontScale = 1.0f;
};
