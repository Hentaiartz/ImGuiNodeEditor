#pragma once
#include "NodeEditor/Node.h"
#include <memory>
#include <vector>

class PreviewNode : public Node {
public:
    PreviewNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Preview"; }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;

    Node* GetConnectedRoot(const std::vector<Link>& links,
                           const std::vector<std::unique_ptr<Node>>& nodes) const;
};
