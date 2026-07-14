#pragma once
#include "NodeEditor/Node.h"
#include <vector>

class ComponentOutputNode : public Node {
public:
    ComponentOutputNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "ComponentOutput"; }
    const char* GetDisplayLabel() const override { return "Output"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    // Returns the single root connected to this output (for backward compat)
    Node* GetConnectedRoot(const std::vector<Link>& links,
        const std::vector<std::unique_ptr<Node>>& nodes) const;

    // Returns ALL nodes connected to this output's input pin, in m_ChildOrder
    std::vector<Node*> GetAllConnectedRoots(const std::vector<Link>& links,
        const std::vector<std::unique_ptr<Node>>& nodes) const;

    // Reorderable list of child node IDs (used by GetAllConnectedRoots)
    mutable std::vector<int> m_ChildOrder;
};
