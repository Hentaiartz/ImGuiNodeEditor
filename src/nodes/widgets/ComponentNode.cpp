#include "ComponentNode.h"
#include "Core/NodeFactory.h"
#include "Core/PreviewHelpers.h"
#include "style/ThemeNode.h"
#include "containers/WindowNode.h"
#include "output/PreviewNode.h"
#include "widgets/ComponentOutputNode.h"
#include <imgui.h>
#include <cstdio>
#include <cstring>
#include <sstream>

ComponentNode::ComponentNode(int id)
    : Node(id, "Component")
{
    m_Outputs.push_back({ id * 10000 + 2, "Container", PinKind::Output, PinType::Container, id });
    m_Size = ImVec2(NODE_WIDTH, 80.0f);
}

void ComponentNode::Draw() {}

void ComponentNode::DrawProperties() {
    if (Node::PropSection("Component")) {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s", m_Name.c_str());
        if (ImGui::InputText("Name", buf, sizeof(buf)))
            m_Name = buf;
        ImGui::TextColored(ImVec4(0.55f, 0.56f, 0.60f, 1.0f),
            "Double-click this node to edit its internal graph.");
        ImGui::Text("%d sub-nodes, %d links",
            (int)m_SubNodes.size(), (int)m_SubLinks.size());
    }
}

void ComponentNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("subnext %d\n", m_NextSubId);
    for (auto& node : m_SubNodes) {
        b.appendf("subnode %d %s\n", node->m_Id, node->GetTypeName());
        b.appendf("_n S \"%s\"\n", node->m_Name.c_str());
        b.appendf("_p V %.1f %.1f\n", node->m_Pos.x, node->m_Pos.y);
        ImGuiTextBuffer extraBuf;
        node->SaveExtra(extraBuf);
        std::string extraStr = extraBuf.c_str();
        std::istringstream stream(extraStr);
        std::string line;
        while (std::getline(stream, line)) {
            if (!line.empty())
                b.appendf("_e %s\n", line.c_str());
        }
        b.appendf("subend\n");
    }
    for (auto& link : m_SubLinks)
        b.appendf("sublink %d %d %d\n", link.id, link.fromPinId, link.toPinId);
}

void ComponentNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "subnext") == 0) {
        sscanf(line, "%d", &m_NextSubId);
    } else if (strcmp(key, "subnode") == 0) {
        int id; char type[64];
        if (sscanf(line, "%d %63s", &id, type) == 2) {
            auto newNode = CreateNode(type, id);
            if (newNode) {
                m_LoadingSubNode = newNode.get();
                m_SubNodes.push_back(std::move(newNode));
            } else {
                m_LoadingSubNode = nullptr;
            }
        }
    } else if (strcmp(key, "subend") == 0) {
        m_LoadingSubNode = nullptr;
    } else if (strcmp(key, "sublink") == 0) {
        Link l;
        if (sscanf(line, "%d %d %d", &l.id, &l.fromPinId, &l.toPinId) == 3)
            m_SubLinks.push_back(l);
    } else if (m_LoadingSubNode) {
        if (strcmp(key, "_n") == 0) {
            char v[256];
            if (sscanf(line, " S \"%255[^\"]\"", v) >= 1)
                m_LoadingSubNode->m_Name = v;
        } else if (strcmp(key, "_p") == 0) {
            sscanf(line, " V %f %f", &m_LoadingSubNode->m_Pos.x, &m_LoadingSubNode->m_Pos.y);
        } else if (strcmp(key, "_e") == 0) {
            char subKey[64];
            if (sscanf(line, " %63s", subKey) == 1) {
                const char* subRest = line + strlen(subKey);
                while (*subRest == ' ') subRest++;
                m_LoadingSubNode->LoadExtra(subKey, subRest);
            }
        }
    }
}

void ComponentNode::RenderPreview(const RenderContext& ctx) {
    if (m_SubNodes.empty()) return;

    // Collect all roots connected to ComponentOutput
    std::vector<Node*> roots;
    for (auto& n : m_SubNodes) {
        if (strcmp(n->GetTypeName(), "ComponentOutput") == 0) {
            auto* out = dynamic_cast<ComponentOutputNode*>(n.get());
            if (out) roots = out->GetAllConnectedRoots(m_SubLinks, m_SubNodes);
            break;
        }
    }
    // Fallback: single root via Preview node
    if (roots.empty()) {
        Node* singleRoot = nullptr;
        for (auto& pn : m_SubNodes) {
            if (strcmp(pn->GetTypeName(), "Preview") == 0) {
                for (auto& l : m_SubLinks) {
                    for (auto& ip : pn->m_Inputs) {
                        if (ip.id == l.toPinId) {
                            for (auto& n : m_SubNodes)
                                for (auto& op : n->m_Outputs)
                                    if (op.id == l.fromPinId) { singleRoot = n.get(); goto foundRoot; }
                        }
                    }
                }
            }
        }
        foundRoot:
        if (!singleRoot) {
            for (auto& n : m_SubNodes) {
                if (dynamic_cast<WindowNode*>(n.get())) { singleRoot = n.get(); break; }
            }
        }
        if (singleRoot) roots.push_back(singleRoot);
    }
    if (roots.empty()) return;

    ThemeNode* theme = nullptr;
    for (auto* r : roots) {
        ThemeNode* t = RenderHelpers::FindNodeTheme(r, m_SubLinks, m_SubNodes);
        if (t) { theme = t; break; }
    }
    if (!theme) theme = ctx.theme;

    for (auto* root : roots) {
        RenderContext subCtx = { theme, m_SubLinks, m_SubNodes, 0 };
        if (dynamic_cast<WindowNode*>(root)) {
            auto children = RenderHelpers::GetChildren(root, m_SubLinks, m_SubNodes);
            for (auto* c : children) c->RenderPreview(subCtx);
        } else {
            root->RenderPreview(subCtx);
        }
    }
}

