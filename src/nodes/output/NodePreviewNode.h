#pragma once
#include "NodeEditor/Node.h"
#include <memory>
#include <vector>

class NodePreviewNode : public Node {
public:
    NodePreviewNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "NodePreview"; }
    const char* GetDisplayLabel() const override { return "NodePreview"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;

    Node* GetConnectedInput(const std::vector<Link>& links,
                            const std::vector<std::unique_ptr<Node>>& nodes) const;
};
