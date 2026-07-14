#include "PreviewRenderer.h"
#include "PreviewHelpers.h"
#include "NodeEditor/NodeEditor.h"
#include "output/PreviewNode.h"
#include "output/NodePreviewNode.h"
#include "containers/WindowNode.h"
#include "style/ThemeNode.h"
#include <imgui.h>

void RenderLivePreview(const std::vector<std::unique_ptr<Node>>& allNodes, const std::vector<Link>& allLinks) {
    using namespace RenderHelpers;
    for (auto& node : allNodes) {

        // PreviewNode: accepts any node (wraps in a container window)
        if (strcmp(node->GetTypeName(), "Preview") == 0) {
            PreviewNode* pn = dynamic_cast<PreviewNode*>(node.get());
            if (!pn) continue;
            Node* root = pn->GetConnectedRoot(allLinks, allNodes);
            if (!root) continue;
            ThemeNode* theme = FindNodeTheme(root, allLinks, allNodes);
            if (!theme) theme = &DefaultTheme();
            RenderContext ctx = { theme, allLinks, allNodes, 0 };
            float prevScale = ImGui::GetIO().FontGlobalScale;
            if (theme) ImGui::GetIO().FontGlobalScale = theme->m_FontSize / 14.0f;
            ImGui::Begin("##preview", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
            root->RenderPreview(ctx);
            ImGui::End();
            ImGui::GetIO().FontGlobalScale = prevScale;
            continue;
        }

        // NodePreviewNode: accepts any node (except Theme/Animation)
        if (strcmp(node->GetTypeName(), "NodePreview") == 0) {
            NodePreviewNode* npn = dynamic_cast<NodePreviewNode*>(node.get());
            if (!npn) continue;
            Node* input = npn->GetConnectedInput(allLinks, allNodes);
            if (!input) continue;
            ThemeNode* theme = FindNodeTheme(input, allLinks, allNodes);
            if (!theme) theme = &DefaultTheme();
            RenderContext ctx = { theme, allLinks, allNodes, 0 };
            float prevScale = ImGui::GetIO().FontGlobalScale;
            if (theme) ImGui::GetIO().FontGlobalScale = theme->m_FontSize / 14.0f;
            ImGui::Begin("##nodePreview", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
            input->RenderPreview(ctx);
            ImGui::End();
            ImGui::GetIO().FontGlobalScale = prevScale;
            continue;
        }

    }
}
