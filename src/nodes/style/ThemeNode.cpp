#include "ThemeNode.h"
#include "Core/Application.h"
#include <imgui.h>

ThemeNode::ThemeNode(int id)
    : Node(id, "Theme")
{
    m_Outputs.push_back({ id * 10000 + 1, "Theme", PinKind::Output, PinType::Theme, id });
    m_Size = ImVec2(NODE_WIDTH, 140.0f);
}

void ThemeNode::Draw() {}

void ThemeNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("primary C %.3f %.3f %.3f %.3f\n", m_Primary.x, m_Primary.y, m_Primary.z, m_Primary.w);
    b.appendf("primaryHover C %.3f %.3f %.3f %.3f\n", m_PrimaryHover.x, m_PrimaryHover.y, m_PrimaryHover.z, m_PrimaryHover.w);
    b.appendf("bgColor C %.3f %.3f %.3f %.3f\n", m_BgColor.x, m_BgColor.y, m_BgColor.z, m_BgColor.w);
    b.appendf("titleBg C %.3f %.3f %.3f %.3f\n", m_TitleBg.x, m_TitleBg.y, m_TitleBg.z, m_TitleBg.w);
    b.appendf("textColor C %.3f %.3f %.3f %.3f\n", m_TextColor.x, m_TextColor.y, m_TextColor.z, m_TextColor.w);
    b.appendf("borderColor C %.3f %.3f %.3f %.3f\n", m_BorderColor.x, m_BorderColor.y, m_BorderColor.z, m_BorderColor.w);
    b.appendf("checkMark C %.3f %.3f %.3f %.3f\n", m_CheckMark.x, m_CheckMark.y, m_CheckMark.z, m_CheckMark.w);
    b.appendf("sliderGrab C %.3f %.3f %.3f %.3f\n", m_SliderGrab.x, m_SliderGrab.y, m_SliderGrab.z, m_SliderGrab.w);
    b.appendf("sliderGrabActive C %.3f %.3f %.3f %.3f\n", m_SliderGrabActive.x, m_SliderGrabActive.y, m_SliderGrabActive.z, m_SliderGrabActive.w);
    b.appendf("separator C %.3f %.3f %.3f %.3f\n", m_Separator.x, m_Separator.y, m_Separator.z, m_Separator.w);
    b.appendf("scrollbarGrab C %.3f %.3f %.3f %.3f\n", m_ScrollbarGrab.x, m_ScrollbarGrab.y, m_ScrollbarGrab.z, m_ScrollbarGrab.w);
    b.appendf("scrollbarBg C %.3f %.3f %.3f %.3f\n", m_ScrollbarBg.x, m_ScrollbarBg.y, m_ScrollbarBg.z, m_ScrollbarBg.w);
    b.appendf("popupBg C %.3f %.3f %.3f %.3f\n", m_PopupBg.x, m_PopupBg.y, m_PopupBg.z, m_PopupBg.w);
    b.appendf("tabActive C %.3f %.3f %.3f %.3f\n", m_TabActive.x, m_TabActive.y, m_TabActive.z, m_TabActive.w);
    b.appendf("tabHovered C %.3f %.3f %.3f %.3f\n", m_TabHovered.x, m_TabHovered.y, m_TabHovered.z, m_TabHovered.w);
    b.appendf("tab C %.3f %.3f %.3f %.3f\n", m_Tab.x, m_Tab.y, m_Tab.z, m_Tab.w);
    b.appendf("dockingPreview C %.3f %.3f %.3f %.3f\n", m_DockingPreview.x, m_DockingPreview.y, m_DockingPreview.z, m_DockingPreview.w);
    b.appendf("windowBg C %.3f %.3f %.3f %.3f\n", m_WindowBg.x, m_WindowBg.y, m_WindowBg.z, m_WindowBg.w);
    b.appendf("disabledText C %.3f %.3f %.3f %.3f\n", m_DisabledText.x, m_DisabledText.y, m_DisabledText.z, m_DisabledText.w);
    b.appendf("disabledBg C %.3f %.3f %.3f %.3f\n", m_DisabledBg.x, m_DisabledBg.y, m_DisabledBg.z, m_DisabledBg.w);
    b.appendf("frameBgHover C %.3f %.3f %.3f %.3f\n", m_FrameBgHover.x, m_FrameBgHover.y, m_FrameBgHover.z, m_FrameBgHover.w);
    b.appendf("frameBgActive C %.3f %.3f %.3f %.3f\n", m_FrameBgActive.x, m_FrameBgActive.y, m_FrameBgActive.z, m_FrameBgActive.w);
    b.appendf("primaryActive C %.3f %.3f %.3f %.3f\n", m_PrimaryActive.x, m_PrimaryActive.y, m_PrimaryActive.z, m_PrimaryActive.w);
    b.appendf("titleText C %.3f %.3f %.3f %.3f\n", m_TitleText.x, m_TitleText.y, m_TitleText.z, m_TitleText.w);
    b.appendf("fontSize F %.1f\n", m_FontSize);
    b.appendf("rounding F %.1f\n", m_Rounding);
    b.appendf("themeSpacing F %.1f\n", m_Spacing);
    b.appendf("borderWidth F %.1f\n", m_BorderWidth);
    b.appendf("scrollbarSize F %.1f\n", m_ScrollbarSize);
}

void ThemeNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "primary") == 0) { sscanf(line, " C %f %f %f %f", &m_Primary.x, &m_Primary.y, &m_Primary.z, &m_Primary.w); }
    else if (strcmp(key, "primaryHover") == 0) { sscanf(line, " C %f %f %f %f", &m_PrimaryHover.x, &m_PrimaryHover.y, &m_PrimaryHover.z, &m_PrimaryHover.w); }
    else if (strcmp(key, "bgColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BgColor.x, &m_BgColor.y, &m_BgColor.z, &m_BgColor.w); }
    else if (strcmp(key, "titleBg") == 0) { sscanf(line, " C %f %f %f %f", &m_TitleBg.x, &m_TitleBg.y, &m_TitleBg.z, &m_TitleBg.w); }
    else if (strcmp(key, "textColor") == 0) { sscanf(line, " C %f %f %f %f", &m_TextColor.x, &m_TextColor.y, &m_TextColor.z, &m_TextColor.w); }
    else if (strcmp(key, "borderColor") == 0) { sscanf(line, " C %f %f %f %f", &m_BorderColor.x, &m_BorderColor.y, &m_BorderColor.z, &m_BorderColor.w); }
    else if (strcmp(key, "checkMark") == 0) { sscanf(line, " C %f %f %f %f", &m_CheckMark.x, &m_CheckMark.y, &m_CheckMark.z, &m_CheckMark.w); }
    else if (strcmp(key, "sliderGrab") == 0) { sscanf(line, " C %f %f %f %f", &m_SliderGrab.x, &m_SliderGrab.y, &m_SliderGrab.z, &m_SliderGrab.w); }
    else if (strcmp(key, "sliderGrabActive") == 0) { sscanf(line, " C %f %f %f %f", &m_SliderGrabActive.x, &m_SliderGrabActive.y, &m_SliderGrabActive.z, &m_SliderGrabActive.w); }
    else if (strcmp(key, "separator") == 0) { sscanf(line, " C %f %f %f %f", &m_Separator.x, &m_Separator.y, &m_Separator.z, &m_Separator.w); }
    else if (strcmp(key, "scrollbarGrab") == 0) { sscanf(line, " C %f %f %f %f", &m_ScrollbarGrab.x, &m_ScrollbarGrab.y, &m_ScrollbarGrab.z, &m_ScrollbarGrab.w); }
    else if (strcmp(key, "scrollbarBg") == 0) { sscanf(line, " C %f %f %f %f", &m_ScrollbarBg.x, &m_ScrollbarBg.y, &m_ScrollbarBg.z, &m_ScrollbarBg.w); }
    else if (strcmp(key, "popupBg") == 0) { sscanf(line, " C %f %f %f %f", &m_PopupBg.x, &m_PopupBg.y, &m_PopupBg.z, &m_PopupBg.w); }
    else if (strcmp(key, "tabActive") == 0) { sscanf(line, " C %f %f %f %f", &m_TabActive.x, &m_TabActive.y, &m_TabActive.z, &m_TabActive.w); }
    else if (strcmp(key, "tabHovered") == 0) { sscanf(line, " C %f %f %f %f", &m_TabHovered.x, &m_TabHovered.y, &m_TabHovered.z, &m_TabHovered.w); }
    else if (strcmp(key, "tab") == 0) { sscanf(line, " C %f %f %f %f", &m_Tab.x, &m_Tab.y, &m_Tab.z, &m_Tab.w); }
    else if (strcmp(key, "dockingPreview") == 0) { sscanf(line, " C %f %f %f %f", &m_DockingPreview.x, &m_DockingPreview.y, &m_DockingPreview.z, &m_DockingPreview.w); }
    else if (strcmp(key, "windowBg") == 0) { sscanf(line, " C %f %f %f %f", &m_WindowBg.x, &m_WindowBg.y, &m_WindowBg.z, &m_WindowBg.w); }
    else if (strcmp(key, "disabledText") == 0) { sscanf(line, " C %f %f %f %f", &m_DisabledText.x, &m_DisabledText.y, &m_DisabledText.z, &m_DisabledText.w); }
    else if (strcmp(key, "disabledBg") == 0) { sscanf(line, " C %f %f %f %f", &m_DisabledBg.x, &m_DisabledBg.y, &m_DisabledBg.z, &m_DisabledBg.w); }
    else if (strcmp(key, "frameBgHover") == 0) { sscanf(line, " C %f %f %f %f", &m_FrameBgHover.x, &m_FrameBgHover.y, &m_FrameBgHover.z, &m_FrameBgHover.w); }
    else if (strcmp(key, "frameBgActive") == 0) { sscanf(line, " C %f %f %f %f", &m_FrameBgActive.x, &m_FrameBgActive.y, &m_FrameBgActive.z, &m_FrameBgActive.w); }
    else if (strcmp(key, "primaryActive") == 0) { sscanf(line, " C %f %f %f %f", &m_PrimaryActive.x, &m_PrimaryActive.y, &m_PrimaryActive.z, &m_PrimaryActive.w); }
    else if (strcmp(key, "titleText") == 0) { sscanf(line, " C %f %f %f %f", &m_TitleText.x, &m_TitleText.y, &m_TitleText.z, &m_TitleText.w); }
    else if (strcmp(key, "fontSize") == 0) { sscanf(line, " F %f", &m_FontSize); }
    else if (strcmp(key, "rounding") == 0) { sscanf(line, " F %f", &m_Rounding); }
    else if (strcmp(key, "themeSpacing") == 0) { sscanf(line, " F %f", &m_Spacing); }
    else if (strcmp(key, "borderWidth") == 0) { sscanf(line, " F %f", &m_BorderWidth); }
    else if (strcmp(key, "scrollbarSize") == 0) { sscanf(line, " F %f", &m_ScrollbarSize); }
}

static bool GradientHeader(const char* label) {
    auto& cfg = Application::GetConfig();
    int r = (int)(cfg.selection.x * 255);
    int g = (int)(cfg.selection.y * 255);
    int b = (int)(cfg.selection.z * 255);
    ImU32 colLeft  = IM_COL32(r, g, b, 35);
    ImU32 colRight = IM_COL32(r, g, b, 0);
    ImVec2 hPos = ImGui::GetCursorScreenPos();
    float hWidth = ImGui::GetContentRegionAvail().x;
    float hHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2 + 4;
    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
        hPos, hPos + ImVec2(hWidth, hHeight),
        colLeft, colRight, colRight, colLeft);
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 255, 255, 18));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 255, 255, 30));
    bool open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_SpanAvailWidth);
    ImGui::PopStyleColor(3);
    return open;
}

void ThemeNode::DrawProperties() {
    if (GradientHeader("Colors")) {
        ImGui::ColorEdit4("Primary",     &m_Primary.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Hover",       &m_PrimaryHover.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Window bg",   &m_WindowBg.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Child bg",    &m_BgColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Title bg",    &m_TitleBg.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Text",        &m_TextColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Border",      &m_BorderColor.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Title text",  &m_TitleText.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
    }
    if (GradientHeader("Widgets")) {
        ImGui::ColorEdit4("Check Mark",  &m_CheckMark.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Slider Grab", &m_SliderGrab.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Sldr Active", &m_SliderGrabActive.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Frame Hover", &m_FrameBgHover.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Frame Active",&m_FrameBgActive.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Btn Active",  &m_PrimaryActive.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Separator##wdg",   &m_Separator.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
    }
    if (GradientHeader("Scrollbar")) {
        ImGui::ColorEdit4("Bar Bg##sb",    &m_ScrollbarBg.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Bar Grab##sb",  &m_ScrollbarGrab.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
    }
    if (GradientHeader("Layout")) {
        ImGui::DragFloat("Font size",    &m_FontSize, 0.5f, 8.0f, 64.0f);
        ImGui::DragFloat("Rounding",     &m_Rounding, 0.5f, 0.0f, 32.0f);
        ImGui::DragFloat("Spacing",      &m_Spacing, 0.5f, 0.0f, 50.0f);
        ImGui::DragFloat("Border",       &m_BorderWidth, 0.2f, 0.0f, 10.0f);
        ImGui::DragFloat("Scrollbar size", &m_ScrollbarSize, 0.5f, 4.0f, 40.0f);
        ImGui::ColorEdit4("Disabled Text",&m_DisabledText.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit4("Disabled Bg",  &m_DisabledBg.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
    }
}

void ThemeNode::RenderPreview(const RenderContext& ctx) {}
