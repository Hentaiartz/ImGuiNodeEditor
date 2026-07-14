#include "PreviewNode.h"
#include "containers/WindowNode.h"
#include <imgui.h>

PreviewNode::PreviewNode(int id)
    : Node(id, "Preview")
{
    m_Inputs.push_back({
        id * 10000 + 1, "Window", PinKind::Input, PinType::Container, id
    });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void PreviewNode::Draw() {}

void PreviewNode::DrawProperties() {
    ImGui::TextUnformatted("Only accepts Window nodes");
}

void PreviewNode::RenderPreview(const RenderContext& ctx) {}

Node* PreviewNode::GetConnectedRoot(
    const std::vector<Link>& links,
    const std::vector<std::unique_ptr<Node>>& nodes) const
{
    if (m_Inputs.empty()) return nullptr;
    int inputPinId = m_Inputs[0].id;

    for (auto& link : links) {
        if (link.toPinId == inputPinId) {
            for (auto& node : nodes) {
                for (auto& p : node->m_Outputs) {
                    if (p.id == link.fromPinId) {
                        // Only accept WindowNode
                        if (dynamic_cast<WindowNode*>(node.get()))
                            return node.get();
                        return nullptr;
                    }
                }
            }
        }
    }
    return nullptr;
}
