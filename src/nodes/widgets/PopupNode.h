#pragma once
#include "containers/ContainerNode.h"

class PopupNode : public ContainerNode {
public:
    float GetWidgetWidth(const RenderContext*) const override { return 0; }
    PopupNode(int id);
    const char* GetTypeName() const override { return "Popup"; }
    const char* GetLayoutName() const override { return "Popup"; }
    const char* GetDisplayLabel() const override { return m_Title.empty() ? "Popup" : m_Title.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::string m_Title = "Popup";
    bool m_Modal = false;
    bool m_Open = false;
    double m_AnimStart = -1.0;
    bool m_PrevOpen = false;
    bool m_LastFrameOpen = false;
    bool m_Initialized = false;
    bool m_CloseAnimating = false;
};
