#pragma once
#include "NodeEditor/Node.h"
#include <string>
#include <vector>
#include <memory>

class ComponentNode : public Node {
public:
    ComponentNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Component"; }
    const char* GetDisplayLabel() const override { return m_Name.empty() ? "Component" : m_Name.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    std::vector<std::unique_ptr<Node>> m_SubNodes;
    std::vector<Link> m_SubLinks;
    int m_NextSubId = 1;

private:
    Node* m_LoadingSubNode = nullptr;
};
