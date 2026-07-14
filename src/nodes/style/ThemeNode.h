#pragma once
#include "NodeEditor/Node.h"

class ThemeNode : public Node {
public:
    ThemeNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Theme"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    // Colors
    ImVec4 m_Primary         = ImVec4(0.26f, 0.42f, 0.70f, 1.0f);
    ImVec4 m_PrimaryHover    = ImVec4(0.30f, 0.48f, 0.78f, 1.0f);
    ImVec4 m_BgColor         = ImVec4(0.10f, 0.11f, 0.14f, 1.0f);
    ImVec4 m_TitleBg         = ImVec4(0.06f, 0.07f, 0.10f, 1.0f);
    ImVec4 m_TextColor       = ImVec4(0.85f, 0.87f, 0.90f, 1.0f);
    ImVec4 m_BorderColor     = ImVec4(0.22f, 0.24f, 0.32f, 1.0f);

    ImVec4 m_CheckMark       = ImVec4(0.545f, 0.486f, 1.000f, 1.0f);
    ImVec4 m_SliderGrab      = ImVec4(0.545f, 0.486f, 1.000f, 1.0f);
    ImVec4 m_SliderGrabActive= ImVec4(0.60f, 0.55f, 1.0f, 1.0f);
    ImVec4 m_Separator       = ImVec4(0.18f, 0.19f, 0.23f, 1.0f);
    ImVec4 m_ScrollbarGrab   = ImVec4(0.20f, 0.20f, 0.23f, 1.0f);
    ImVec4 m_ScrollbarBg     = ImVec4(0.07f, 0.07f, 0.08f, 1.0f);
    ImVec4 m_PopupBg         = ImVec4(0.10f, 0.11f, 0.14f, 0.95f);
    ImVec4 m_TabActive       = ImVec4(0.392f, 0.361f, 0.702f, 1.0f);
    ImVec4 m_TabHovered      = ImVec4(0.24f, 0.27f, 0.38f, 1.0f);
    ImVec4 m_Tab             = ImVec4(0.10f, 0.11f, 0.14f, 1.0f);
    ImVec4 m_DockingPreview  = ImVec4(0.35f, 0.55f, 0.85f, 0.5f);
    ImVec4 m_WindowBg        = ImVec4(0.10f, 0.11f, 0.13f, 1.0f);

    // Sizing
    float  m_FontSize    = 14.0f;
    float  m_Rounding    = 6.0f;
    float  m_Spacing     = 8.0f;
    float  m_BorderWidth = 1.0f;

    float  m_ScrollbarSize = 14.0f;

    // Disabled
    ImVec4 m_DisabledText   = ImVec4(0.5f, 0.5f, 0.55f, 0.5f);
    ImVec4 m_DisabledBg     = ImVec4(0.25f, 0.25f, 0.25f, 0.4f);

    // Frame states
    ImVec4 m_FrameBgHover   = ImVec4(0.18f, 0.20f, 0.25f, 1.0f);
    ImVec4 m_FrameBgActive  = ImVec4(0.22f, 0.25f, 0.30f, 1.0f);

    // Primary active (pressed button)
    ImVec4 m_PrimaryActive  = ImVec4(0.22f, 0.35f, 0.60f, 1.0f);

    // Title text (window title bar color)
    ImVec4 m_TitleText      = ImVec4(0.85f, 0.87f, 0.90f, 1.0f);
};
