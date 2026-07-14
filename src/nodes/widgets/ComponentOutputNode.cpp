#include "ComponentOutputNode.h"
#include "NodeEditor/NodeEditor.h"
#include <imgui.h>
#include <algorithm>

ComponentOutputNode::ComponentOutputNode(int id)
    : Node(id, "ComponentOutput")
{
    m_Inputs.push_back({ id * 10000 + 1, "Root", PinKind::Input, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 50.0f);
}

void ComponentOutputNode::Draw() {}

void ComponentOutputNode::SaveExtra(ImGuiTextBuffer& b) const {
    for (int childId : m_ChildOrder)
        b.appendf("childorder %d\n", childId);
}
void ComponentOutputNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "childorder") == 0) {
        int id;
        if (sscanf(line, "childorder %d", &id) == 1)
            m_ChildOrder.push_back(id);
    }
}

void ComponentOutputNode::DrawProperties() {
    if (Node::PropSection("Output")) {
        ImGui::TextColored(ImVec4(0.49f, 0.44f, 1.0f, 1.0f),
            "This node marks the output\nof this Component.");
        ImGui::Spacing();
        if (auto* ed = GetEditor()) {
            auto roots = GetAllConnectedRoots(ed->GetActiveLinks(), ed->GetActiveNodes());
            if (roots.empty()) {
                ImGui::TextDisabled("Child: (none)");
            } else {
                ImGui::Text("Children (drag to reorder):");
                ImGui::Separator();
                for (int i = 0; i < (int)roots.size(); i++) {
                    char label[64];
                    snprintf(label, sizeof(label), "%s##co_%d", roots[i]->GetDisplayLabel(), roots[i]->m_Id);
                    // Up button
                    if (i > 0) {
                        if (ImGui::ArrowButton((std::string("##up") + std::to_string(i)).c_str(), ImGuiDir_Up)) {
                            std::swap(m_ChildOrder[i], m_ChildOrder[i - 1]);
                        }
                        ImGui::SameLine();
                    } else {
                        ImGui::Dummy(ImVec2(ImGui::GetFrameHeight(), 0));
                        ImGui::SameLine();
                    }
                    // Down button
                    if (i < (int)roots.size() - 1) {
                        if (ImGui::ArrowButton((std::string("##dn") + std::to_string(i)).c_str(), ImGuiDir_Down)) {
                            std::swap(m_ChildOrder[i], m_ChildOrder[i + 1]);
                        }
                        ImGui::SameLine();
                    } else {
                        ImGui::Dummy(ImVec2(ImGui::GetFrameHeight(), 0));
                        ImGui::SameLine();
                    }
                    ImGui::Text("%s", roots[i]->GetDisplayLabel());
                }
            }
        }
    }
}

void ComponentOutputNode::RenderPreview(const RenderContext& ctx) {
    // Renders nothing - it's just a marker
}

Node* ComponentOutputNode::GetConnectedRoot(const std::vector<Link>& links,
    const std::vector<std::unique_ptr<Node>>& nodes) const {
    auto roots = GetAllConnectedRoots(links, nodes);
    return roots.empty() ? nullptr : roots[0];
}

std::vector<Node*> ComponentOutputNode::GetAllConnectedRoots(const std::vector<Link>& links,
    const std::vector<std::unique_ptr<Node>>& nodes) const {
    // Collect connected nodes
    std::vector<Node*> raw;
    int inputPinId = m_Inputs.empty() ? -1 : m_Inputs[0].id;
    if (inputPinId < 0) return raw;
    for (auto& link : links) {
        if (link.toPinId != inputPinId) continue;
        for (auto& n : nodes)
            for (auto& op : n->m_Outputs)
                if (op.id == link.fromPinId) { raw.push_back(n.get()); break; }
    }
    if (raw.empty()) return raw;

    // Sync m_ChildOrder: add new nodes not yet in order, remove stale IDs
    for (auto* r : raw) {
        if (std::find(m_ChildOrder.begin(), m_ChildOrder.end(), r->m_Id) == m_ChildOrder.end())
            m_ChildOrder.push_back(r->m_Id);
    }
    m_ChildOrder.erase(std::remove_if(m_ChildOrder.begin(), m_ChildOrder.end(),
        [&](int id) {
            return std::find_if(raw.begin(), raw.end(), [id](Node* n) { return n->m_Id == id; }) == raw.end();
        }), m_ChildOrder.end());

    // Return nodes sorted by m_ChildOrder
    std::vector<Node*> sorted;
    for (int id : m_ChildOrder) {
        for (auto* r : raw) {
            if (r->m_Id == id) { sorted.push_back(r); break; }
        }
    }
    return sorted;
}

