#include "NodePreviewNode.h"
#include "containers/WindowNode.h"
#include "Core/PreviewHelpers.h"
#include <imgui.h>

NodePreviewNode::NodePreviewNode(int id)
    : Node(id, "NodePreview")
{
    m_Inputs.push_back({
        id * 10000 + 1, "Input", PinKind::Input, PinType::Container, id
    });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
}

void NodePreviewNode::Draw() {}

void NodePreviewNode::DrawProperties() {
    ImGui::TextUnformatted("Preview any node directly");
}

void NodePreviewNode::RenderPreview(const RenderContext& ctx) {}

Node* NodePreviewNode::GetConnectedInput(
    const std::vector<Link>& links,
    const std::vector<std::unique_ptr<Node>>& nodes) const
{
    if (m_Inputs.empty()) return nullptr;
    int inputPinId = m_Inputs[0].id;

    for (auto& link : links) {
        if (link.toPinId == inputPinId) {
            for (auto& node : nodes) {
                for (auto& p : node->m_Outputs) {
                    if (p.id == link.fromPinId)
                        return node.get();
                }
            }
        }
    }
    return nullptr;
}
