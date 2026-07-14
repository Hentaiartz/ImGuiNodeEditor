#pragma once
#include "NodeEditor/Node.h"

enum class BoxSizeMode { Auto, Fill, Fixed };
enum class BoxJustify  { Start, Center, End, SpaceBetween };
enum class BoxAlign    { Start, Center, End, Stretch };

class ContainerNode : public Node {
public:
    ContainerNode(int id, const std::string& name);
    void Draw() override;
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    virtual const char* GetLayoutName() const = 0;

    void SaveBoxExtra(ImGuiTextBuffer& b) const;
    bool LoadBoxProperty(const char* key, const char* line);
    void DrawBoxProperties();

    bool m_HasBackground = false;
    float m_Padding = 0.0f;
    BoxSizeMode m_WidthMode = BoxSizeMode::Auto;
    BoxSizeMode m_HeightMode = BoxSizeMode::Auto;
    ImVec2 m_FixedSize = ImVec2(200, 100);
    BoxJustify m_Justify = BoxJustify::Start;
    BoxAlign m_Align = BoxAlign::Start;
};
