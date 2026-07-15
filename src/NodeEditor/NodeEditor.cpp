#include "NodeEditor.h"
#include "Core/Application.h"
#include "Core/PreviewHelpers.h"
#include "Core/NodeFactory.h"
#include "widgets/ComponentNode.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>

static float SnapVal(float val, float grid) {
    return floorf(val / grid + 0.5f) * grid;
}

NodeEditor::NodeEditor()
    : m_CanvasOrigin(0, 0), m_Zoom(1.0f)
    , m_Panning(false), m_PanStart(0, 0), m_PanOffset(0, 0)
    , m_DraggedNodeId(-1), m_DragOffset(0, 0)
    , m_DraggingFromPin(-1), m_HoveredPinId(-1), m_HoveredLinkId(-1)
    , m_SelectedNodeId(-1), m_SelectedLinkId(-1)
    , m_SearchOpen(false), m_SearchSpawnPos(100, 100)
{
    m_SearchBuffer[0] = '\0';
    // Uses NodeFactory::Global() registered on first call
}

void NodeEditor::EnterComponent(ComponentNode* comp) {
    if (!comp) return;
    m_ComponentStack.push_back(comp);
}

void NodeEditor::ExitComponent() {
    if (m_ComponentStack.empty()) return;
    m_ComponentStack.pop_back();
    m_SelectedNodeId = -1;
    m_SelectedLinkId = -1;
    DeselectAll();
}

std::vector<std::unique_ptr<Node>>& NodeEditor::GetActiveNodes() {
    return GetCurrentComponent() ? GetCurrentComponent()->m_SubNodes : m_Nodes;
}
const std::vector<std::unique_ptr<Node>>& NodeEditor::GetActiveNodes() const {
    return GetCurrentComponent() ? static_cast<const ComponentNode*>(GetCurrentComponent())->m_SubNodes : m_Nodes;
}
std::vector<Link>& NodeEditor::GetActiveLinks() {
    return GetCurrentComponent() ? GetCurrentComponent()->m_SubLinks : m_Links;
}
const std::vector<Link>& NodeEditor::GetActiveLinks() const {
    return GetCurrentComponent() ? static_cast<const ComponentNode*>(GetCurrentComponent())->m_SubLinks : m_Links;
}
int& NodeEditor::GetActiveNextId() {
    return GetCurrentComponent() ? GetCurrentComponent()->m_NextSubId : m_NextId;
}
int NodeEditor::GetActiveNextId() const {
    return GetCurrentComponent() ? GetCurrentComponent()->m_NextSubId : m_NextId;
}

void NodeEditor::Draw() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Node Editor", nullptr,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImRect canvasRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
    ImDrawList* dl = ImGui::GetWindowDrawList();


    if (m_Placing)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

    HandleInteraction(canvasRect);
    DrawGrid(dl, canvasRect);
    if (!IsEditingComponent())
        DrawRegions(dl);
    DrawLinks(dl);
    DrawDragLink(dl);

    for (auto& node : GetActiveNodes()) {
        // Skip nodes inside collapsed regions
        bool hidden = false;
        for (auto& reg : m_Regions) {
            if (!reg.collapsed) continue;
            ImRect rr(reg.pos, reg.pos + reg.size);
            if (rr.Contains(node->m_Pos)) { hidden = true; break; }
        }
        if (!hidden) DrawNode(node.get(), dl);
    }

    // Ghost node while placing
    if (m_Placing && !m_PlacingType.empty() && canvasRect.Contains(ImGui::GetIO().MousePos)) {
        ImVec2 canvasMouse = ScreenToCanvas(ImGui::GetIO().MousePos);
        if (m_SnapToGrid) {
            canvasMouse.x = SnapVal(canvasMouse.x, 50.0f);
            canvasMouse.y = SnapVal(canvasMouse.y, 50.0f);
        }
        // Glow ring at drop position
        ImU32 dropGlow = ImGui::ColorConvertFloat4ToU32(Application::GetConfig().selection);
        dl->AddCircleFilled(CanvasToScreen(canvasMouse + ImVec2(NODE_WIDTH * 0.5f, NODE_TITLE_HEIGHT * 0.5f)),
            12 * m_Zoom, (dropGlow & 0x00FFFFFF) | (35 << 24));
        dl->AddCircle(CanvasToScreen(canvasMouse + ImVec2(NODE_WIDTH * 0.5f, NODE_TITLE_HEIGHT * 0.5f)),
            12 * m_Zoom, (dropGlow & 0x00FFFFFF) | (80 << 24), 0, 2.0f * m_Zoom);
        // Hint text
        ImVec2 hintPos = CanvasToScreen(canvasMouse) + ImVec2(0, -30 * m_Zoom);
        dl->AddCircleFilled(hintPos + ImVec2(5, 5), 2, IM_COL32(200, 200, 220, 160));
        dl->AddText(hintPos + ImVec2(12, 0), IM_COL32(180, 180, 200, 200), "Click to place");
        // Node name
        dl->AddText(hintPos + ImVec2(12, -14), IM_COL32(220, 220, 240, 220), m_PlacingType.c_str());

        {
            auto ghost = NodeFactory::Global().Create(m_PlacingType.c_str(), 0);
            if (ghost) { ghost->m_Pos = canvasMouse; DrawNode(ghost.get(), dl); }
        }
    }

    // Box selection rectangle
    if (m_BoxSelecting) {
        ImVec2 a = CanvasToScreen(ImMin(m_BoxSelectStart, m_BoxSelectEnd));
        ImVec2 b = CanvasToScreen(ImMax(m_BoxSelectStart, m_BoxSelectEnd));
        dl->AddRectFilled(a, b, IM_COL32(40, 80, 180, 25));
        dl->AddRect(a, b, ImGui::ColorConvertFloat4ToU32(Application::GetConfig().selection), 0, 0, 1.5f);
    }

    // Back button when editing a component sub-canvas
    if (IsEditingComponent() && GetCurrentComponent()) {
        ImVec2 contentTL = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
        ImGui::SetCursorScreenPos(contentTL + ImVec2(10, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.28f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.38f, 0.9f));
        std::string label = "← Back to " + std::string(GetCurrentComponent()->GetDisplayLabel());
        if (ImGui::Button(label.c_str())) ExitComponent();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
        ImGui::SameLine(0, 12);
        ImGui::TextDisabled("(sub-canvas)");
    }

    // ── Minimap (always inside content area, top-right) ──
    const float mmW = 200.0f, mmH = 150.0f;
    ImVec2 contentTL = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
    ImVec2 contentBR = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();
    ImVec2 mmPos = ImVec2(contentBR.x - mmW - 12, contentTL.y + 12);
    ImVec2 mmEnd = mmPos + ImVec2(mmW, mmH);
    dl->AddRectFilled(mmPos, mmEnd, IM_COL32(15, 15, 22, 220), 6.0f);
    dl->AddRect(mmPos, mmEnd, IM_COL32(60, 60, 80, 180), 6.0f);

    if (!GetActiveNodes().empty()) {
        ImVec2 minB = GetActiveNodes()[0]->m_Pos;
        ImVec2 maxB = minB + GetActiveNodes()[0]->m_Size;
        for (auto& n : GetActiveNodes()) {
            minB = ImMin(minB, n->m_Pos);
            maxB = ImMax(maxB, n->m_Pos + n->m_Size);
        }
        ImVec2 range = maxB - minB + ImVec2(40, 40);
        float scale = std::min(mmW / range.x, mmH / range.y) * 0.85f;
        if (scale > 0.0f) {
            ImVec2 offset = mmPos + ImVec2(mmW * 0.5f - (minB.x + range.x * 0.5f) * scale,
                                            mmH * 0.5f - (minB.y + range.y * 0.5f) * scale);
            for (auto& n : GetActiveNodes()) {
                ImVec2 a = offset + n->m_Pos * scale;
                ImVec2 b = offset + (n->m_Pos + n->m_Size) * scale;
                ImU32 col = n->m_Selected ? IM_COL32(120, 100, 255, 200) : IM_COL32(70, 72, 90, 180);
                dl->AddRectFilled(a, b, col, 2);
            }
            // Viewport rect (visible canvas area), clamped to minimap
            ImVec2 canvTL = ScreenToCanvas(contentTL);
            ImVec2 canvBR = ScreenToCanvas(contentBR);
            ImVec2 vpA = offset + canvTL * scale;
            ImVec2 vpB = offset + canvBR * scale;
            vpA = ImMax(vpA, mmPos);
            vpB = ImMin(vpB, mmEnd);
            if (vpA.x < vpB.x && vpA.y < vpB.y)
                dl->AddRect(vpA, vpB, IM_COL32(255, 255, 255, 120), 1, 0, 1.5f);
        }
    }

    // Minimap click/drag to pan
    ImGui::SetCursorScreenPos(mmPos);
    ImGui::InvisibleButton("##minimap", ImVec2(mmW, mmH));
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        if (!GetActiveNodes().empty()) {
            ImVec2 mmMouse = ImGui::GetIO().MousePos - mmPos;
            ImVec2 minB = GetActiveNodes()[0]->m_Pos;
            ImVec2 maxB = minB + GetActiveNodes()[0]->m_Size;
            for (auto& n : GetActiveNodes()) {
                minB = ImMin(minB, n->m_Pos);
                maxB = ImMax(maxB, n->m_Pos + n->m_Size);
            }
            ImVec2 range = maxB - minB + ImVec2(40, 40);
            float sc = std::min(mmW / range.x, mmH / range.y) * 0.85f;
            if (sc > 0.0f) {
                float cx = mmMouse.x / sc + minB.x - mmW * 0.5f / sc;
                float cy = mmMouse.y / sc + minB.y - mmH * 0.5f / sc;
                ImVec2 contentSize = contentBR - contentTL;
                m_CanvasOrigin = ImVec2(
                    contentSize.x * 0.5f - cx * m_Zoom,
                    contentSize.y * 0.5f - cy * m_Zoom);
            }
        }
    }
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
        if (!GetActiveNodes().empty()) {
            ImVec2 mmMouse = ImGui::GetIO().MousePos - mmPos;
            ImVec2 minB = GetActiveNodes()[0]->m_Pos;
            ImVec2 maxB = minB + GetActiveNodes()[0]->m_Size;
            for (auto& n : GetActiveNodes()) {
                minB = ImMin(minB, n->m_Pos);
                maxB = ImMax(maxB, n->m_Pos + n->m_Size);
            }
            ImVec2 range = maxB - minB + ImVec2(40, 40);
            float sc = std::min(mmW / range.x, mmH / range.y) * 0.85f;
            if (sc > 0.0f) {
                float cx = mmMouse.x / sc + minB.x - mmW * 0.5f / sc;
                float cy = mmMouse.y / sc + minB.y - mmH * 0.5f / sc;
                ImVec2 contentSize = contentBR - contentTL;
                m_CanvasOrigin = ImVec2(
                    contentSize.x * 0.5f - cx * m_Zoom,
                    contentSize.y * 0.5f - cy * m_Zoom);
            }
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    // ── ComponentNode hover preview (after End so it's a top-level window) ──
    if (!m_Placing && m_DraggedNodeId < 0 && !IsEditingComponent()) {
        ImVec2 mouse = ImGui::GetIO().MousePos;
        int hoverNodeId = HitTestNode(ScreenToCanvas(mouse));
        if (hoverNodeId >= 0) {
            Node* hoverNode = GetNodeById(hoverNodeId);
            if (hoverNode && strcmp(hoverNode->GetTypeName(), "Component") == 0) {
                auto* comp = dynamic_cast<ComponentNode*>(hoverNode);
                if (comp && !comp->m_SubNodes.empty()) {
                    ImVec2 pSize(380, 280);
                    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
                    ImGui::SetNextWindowPos(ImVec2(mouse.x + 16, mouse.y));
                    ImGui::SetNextWindowSize(pSize, ImGuiCond_Appearing);
                    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.08f, 0.95f));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
                    char wndName[64]; snprintf(wndName, sizeof(wndName), "##cprev_%d", comp->m_Id);
                    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings
                        | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove
                        | ImGuiWindowFlags_NoDocking;
                    if (ImGui::Begin(wndName, nullptr, flags)) {
                        ImVec2 wPos = ImGui::GetWindowPos();
                        ImVec2 wSize = ImGui::GetWindowSize();
                        // Same technique as the minimap
                        auto& nodes = comp->m_SubNodes;
                        auto& links = comp->m_SubLinks;
                        ImVec2 minB, maxB;
                        bool first = true;
                        for (auto& n : nodes) {
                            if (strcmp(n->GetTypeName(), "ComponentOutput") == 0) continue;
                            if (first) { minB = n->m_Pos; maxB = n->m_Pos + n->m_Size; first = false; }
                            else {
                                minB = ImMin(minB, n->m_Pos);
                                maxB = ImMax(maxB, n->m_Pos + n->m_Size);
                            }
                        }
                        if (first) { ImGui::Dummy(wSize); } else {
                            ImVec2 range = maxB - minB + ImVec2(40, 40);
                            float scale = std::min(wSize.x / range.x, wSize.y / range.y) * 0.85f;
                            if (scale < 0.05f) scale = 0.05f;
                            ImVec2 offset = wPos + ImVec2(wSize.x * 0.5f - (minB.x + range.x * 0.5f) * scale,
                                                           wSize.y * 0.5f - (minB.y + range.y * 0.5f) * scale);
                            ImDrawList* dl = ImGui::GetWindowDrawList();
                            // Draw links
                            for (auto& lnk : links) {
                                Node* a = nullptr, * b = nullptr;
                                Pin* pa = nullptr, * pb = nullptr;
                                for (auto& n : nodes) {
                                    for (auto& p : n->m_Inputs) if (p.id == lnk.toPinId) { a = n.get(); pa = &p; }
                                    for (auto& p : n->m_Outputs) if (p.id == lnk.fromPinId) { b = n.get(); pb = &p; }
                                }
                                if (a && b && pa && pb) {
                                    auto pinPos = [](Node* node, Pin* pin) -> ImVec2 {
                                        float py = NODE_TITLE_HEIGHT + NODE_PADDING;
                                        if (pin->kind == PinKind::Input) {
                                            for (size_t i = 0; i < node->m_Inputs.size(); i++)
                                                if (node->m_Inputs[i].id == pin->id) { py += i * NODE_PIN_SPACING; break; }
                                            return node->m_Pos + ImVec2(0, py);
                                        } else {
                                            for (size_t i = 0; i < node->m_Outputs.size(); i++)
                                                if (node->m_Outputs[i].id == pin->id) { py += i * NODE_PIN_SPACING; break; }
                                            return node->m_Pos + ImVec2(node->m_Size.x, py);
                                        }
                                    };
                                    dl->AddLine(offset + pinPos(b, pb) * scale, offset + pinPos(a, pa) * scale,
                                        IM_COL32(160, 160, 200, 140), 1.5f);
                                }
                            }
                            // Draw nodes (same as minimap style)
                            for (auto& n : nodes) {
                                if (strcmp(n->GetTypeName(), "ComponentOutput") == 0) continue;
                                ImVec2 a = offset + n->m_Pos * scale;
                                ImVec2 b = offset + (n->m_Pos + n->m_Size) * scale;
                                dl->AddRectFilled(a, b, IM_COL32(70, 72, 90, 220), 2);
                                // Title stripe
                                float titleH = ImMin(18.0f * scale, (b.y - a.y) * 0.5f);
                                dl->AddRectFilled(a, ImVec2(b.x, a.y + titleH), IM_COL32(55, 65, 100, 240), 2);
                                // Label
                                const char* label = n->GetDisplayLabel();
                                if (scale > 0.3f) {
                                    auto font = ImGui::GetIO().Fonts->Fonts[0];
                                    float txtSize = ImMax(scale * 10.0f, 6.0f);
                                    dl->AddText(font, txtSize, a + ImVec2(3 * scale, 2 * scale),
                                        IM_COL32(220, 220, 240, 220), label);
                                }
                                // Pins
                                float py = NODE_TITLE_HEIGHT + NODE_PADDING;
                                for (auto& p : n->m_Outputs) {
                                    dl->AddCircleFilled(offset + (n->m_Pos + ImVec2(n->m_Size.x, py)) * scale,
                                        3.0f * scale, IM_COL32(90, 160, 255, 220));
                                    py += NODE_PIN_SPACING;
                                }
                                py = NODE_TITLE_HEIGHT + NODE_PADDING;
                                for (auto& p : n->m_Inputs) {
                                    dl->AddCircleFilled(offset + (n->m_Pos + ImVec2(0, py)) * scale,
                                        3.0f * scale, IM_COL32(90, 160, 255, 220));
                                    py += NODE_PIN_SPACING;
                                }
                            }
                            ImGui::Dummy(wSize);
                        }
                    }
                    ImGui::End();
                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor();
                }
            }
        }
    }
}

// ─── Transforms ───

ImVec2 NodeEditor::ScreenToCanvas(ImVec2 screen) const {
    return (screen - m_CanvasOrigin) / m_Zoom;
}

ImVec2 NodeEditor::CanvasToScreen(ImVec2 canvas) const {
    return m_CanvasOrigin + canvas * m_Zoom;
}

// ─── Hit tests ───

int NodeEditor::HitTestPin(ImVec2) const {
    ImVec2 m = ImGui::GetIO().MousePos;
    for (auto& node : GetActiveNodes()) {
        for (auto& pin : node->m_Inputs) {
            ImVec2 s = GetPinScreenPos(pin, *node);
            float dx = m.x - s.x, dy = m.y - s.y;
            if (dx*dx + dy*dy < 64.0f) return pin.id;
        }
        for (auto& pin : node->m_Outputs) {
            ImVec2 s = GetPinScreenPos(pin, *node);
            float dx = m.x - s.x, dy = m.y - s.y;
            if (dx*dx + dy*dy < 64.0f) return pin.id;
        }
    }
    return -1;
}

int NodeEditor::HitTestNode(ImVec2 canvasPos) const {
    for (auto it = GetActiveNodes().rbegin(); it != GetActiveNodes().rend(); ++it) {
        bool hidden = false;
        for (auto& reg : m_Regions) {
            if (!reg.collapsed) continue;
            ImRect rr(reg.pos, reg.pos + reg.size);
            if (rr.Contains((*it)->m_Pos)) { hidden = true; break; }
        }
        if (hidden) continue;
        ImRect r((*it)->m_Pos, (*it)->m_Pos + (*it)->m_Size);
        if (r.Contains(canvasPos)) return (*it)->m_Id;
    }
    return -1;
}

// ─── Interaction ───

void NodeEditor::HandleInteraction(const ImRect& canvasRect) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouse = io.MousePos;
    bool hovered = canvasRect.Contains(mouse);

    HandlePlacing(mouse, hovered);

    HandleZoomPan(hovered);
    HandlePinHover(mouse, hovered);
    HandleRegionDrag(mouse);
    HandleLeftClick(mouse, hovered);
    HandleNodeDrag(mouse);
    HandleBoxSelect();
    HandleLinkDrop(mouse, hovered);

    // Right click selection
    if (hovered && ImGui::IsMouseClicked(1)) {
        m_SelectedLinkId = HitTestLink(mouse);
        int nid = HitTestNode(ScreenToCanvas(mouse));
        if (nid >= 0) SelectNode(nid);
        else DeselectAll();
    }

    HandleDoubleClick(mouse, hovered);

    // Context menu
    bool rightClickOnLink = (hovered && ImGui::IsMouseReleased(1) && m_HoveredLinkId >= 0);
    if (rightClickOnLink) {
        GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
            [this](const Link& l) { return l.id == m_HoveredLinkId; }), GetActiveLinks().end());
        m_SelectedLinkId = -1;
        m_HoveredLinkId = -1;
    }
    if (!rightClickOnLink && ImGui::BeginPopupContextWindow("NodeEditorPopup",
        ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
        if (m_SelectedLinkId >= 0) LinkContextMenu();
        else if (m_SelectedNodeId >= 0) {
            Node* n = GetNodeById(m_SelectedNodeId);
            if (n) NodeContextMenu(n);
        } else CanvasContextMenu();
        ImGui::EndPopup();
    }

    HandleKeyboard();
}

bool NodeEditor::TryAutoConnect(Node* dropNode, ImVec2 screenPos) {
    int linkId = ImGui::IsWindowHovered() ? HitTestLink(screenPos) : -1;
    if (linkId < 0) return false;
    int fromPinId = -1, toPinId = -1;
    for (auto& l : GetActiveLinks()) {
        if (l.id == linkId) { fromPinId = l.fromPinId; toPinId = l.toPinId; break; }
    }
    if (fromPinId < 0 || toPinId < 0) return false;
    Pin* fromP = GetPinById(fromPinId);
    Pin* toP = GetPinById(toPinId);
    if (!dropNode || !fromP || !toP) return false;
    if (dropNode->m_Inputs.empty() || dropNode->m_Outputs.empty()) return false;
    for (auto& l : GetActiveLinks())
        for (auto& pin : dropNode->m_Inputs)
            if (l.toPinId == pin.id) return false;
    for (auto& l : GetActiveLinks())
        for (auto& pin : dropNode->m_Outputs)
            if (l.fromPinId == pin.id) return false;
    Pin* compatIn = nullptr;
    Pin* compatOut = nullptr;
    for (auto& p : dropNode->m_Inputs)
        if (p.type == fromP->type) { compatIn = &p; break; }
    for (auto& p : dropNode->m_Outputs)
        if (p.type == toP->type) { compatOut = &p; break; }
    if (!compatIn || !compatOut) return false;
    GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
        [linkId](const Link& l) { return l.id == linkId; }), GetActiveLinks().end());
    int newId = GetActiveNextId()++;
    GetActiveLinks().push_back({newId, fromPinId, compatIn->id});
    newId = GetActiveNextId()++;
    GetActiveLinks().push_back({newId, compatOut->id, toPinId});
    return true;
}

void NodeEditor::HandlePlacing(ImVec2 mouse, bool hovered) {
    if (!m_Placing) return;
    if (hovered) {
        bool clicked = ImGui::IsMouseClicked(0);
        bool dragRelease = ImGui::IsMouseReleased(0);
        if (clicked || dragRelease) {
            if (!m_PlacingType.empty()) {
                int id = GetActiveNextId()++;
                auto node = NodeFactory::Global().Create(m_PlacingType.c_str(), id);
                if (node) {
                    PushUndo();
                    Node* newNodePtr = node.get();
                    ImVec2 newPos = ScreenToCanvas(mouse);
                    if (m_SnapToGrid) {
                        newPos.x = SnapVal(newPos.x, 50.0f);
                        newPos.y = SnapVal(newPos.y, 50.0f);
                    }
                    newNodePtr->m_Pos = newPos;
                    GetActiveNodes().push_back(std::move(node));
                    TryAutoConnect(newNodePtr, mouse);
                    m_DraggedNodeId = newNodePtr->m_Id;
                    m_DragOffset = ImVec2(0, 0);
                }
            }
            m_Placing = false;
            m_PlacingType.clear();
        }
    }
    if (ImGui::IsMouseClicked(1) || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        m_Placing = false;
        m_PlacingType.clear();
    }
}

void NodeEditor::HandleZoomPan(bool hovered) {
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsWindowHovered() || hovered) {
        if (ImGui::IsWindowHovered() && io.MouseWheel != 0) {
            ImVec2 oldMC = ScreenToCanvas(io.MousePos);
            m_Zoom = std::clamp(m_Zoom + io.MouseWheel * 0.1f * m_Zoom, 0.2f, 4.0f);
            m_CanvasOrigin += (ScreenToCanvas(io.MousePos) - oldMC) * m_Zoom;
        }
        if (ImGui::IsMouseClicked(2)) {
            m_Panning = true;
            m_PanStart = io.MousePos;
            m_PanOffset = m_CanvasOrigin;
        }
        if (m_Panning) {
            if (ImGui::IsMouseDragging(2))
                m_CanvasOrigin = m_PanOffset + io.MousePos - m_PanStart;
            if (ImGui::IsMouseReleased(2))
                m_Panning = false;
        }
    }
}

void NodeEditor::HandlePinHover(ImVec2 mouse, bool hovered) {
    m_HoveredPinId = hovered ? HitTestPin(mouse) : -1;
    if (m_HoveredPinId < 0 && m_DraggingFromPin >= 0 && hovered) {
        Pin* dragPin = GetPinById(m_DraggingFromPin);
        float bestD = 18.0f;
        for (auto& n : GetActiveNodes()) {
            for (auto& pin : n->m_Inputs) {
                if (pin.id == m_DraggingFromPin) continue;
                if (dragPin && (pin.type != dragPin->type || dragPin->kind != PinKind::Output)) continue;
                float d = ImSqrt(ImLengthSqr(GetPinScreenPos(pin, *n) - mouse));
                if (d < bestD) { bestD = d; m_HoveredPinId = pin.id; }
            }
            for (auto& pin : n->m_Outputs) {
                if (pin.id == m_DraggingFromPin) continue;
                if (dragPin && (pin.type != dragPin->type || dragPin->kind != PinKind::Input)) continue;
                float d = ImSqrt(ImLengthSqr(GetPinScreenPos(pin, *n) - mouse));
                if (d < bestD) { bestD = d; m_HoveredPinId = pin.id; }
            }
        }
    }
    m_HoveredLinkId = hovered ? HitTestLink(mouse) : -1;
}

void NodeEditor::HandleRegionDrag(ImVec2 mouse) {
    if (IsEditingComponent()) return;
    if (m_SelectedRegionId >= 0 && ImGui::IsMouseDragging(0) && m_DraggedRegionEdge >= 0) {
        for (auto& r : m_Regions) {
            if (r.id == m_SelectedRegionId) {
                if (m_DraggedRegionEdge == 0) {
                    ImVec2 newPos = ScreenToCanvas(mouse) - m_RegionDragOffset;
                    if (m_SnapToGrid) { newPos.x = SnapVal(newPos.x, 50.0f); newPos.y = SnapVal(newPos.y, 50.0f); }
                    ImRect oldRect(r.pos, r.pos + r.size);
                    r.pos = newPos;
                    for (auto& n : GetActiveNodes())
                        if (oldRect.Contains(n->m_Pos))
                            n->m_Pos = n->m_Pos + (newPos - oldRect.Min);
                } else if (m_DraggedRegionEdge == 1) {
                    ImVec2 newSize = ScreenToCanvas(mouse) - r.pos;
                    if (newSize.x < 60) newSize.x = 60;
                    if (newSize.y < 40) newSize.y = 40;
                    r.size = newSize;
                }
                break;
            }
        }
    }
    if (m_SelectedRegionId >= 0 && ImGui::IsMouseReleased(0))
        m_DraggedRegionEdge = -1;
}

void NodeEditor::HandleLeftClick(ImVec2 mouse, bool hovered) {
    if (!hovered || !ImGui::IsMouseClicked(0)) return;
    ImGuiIO& io = ImGui::GetIO();
    int pid = HitTestPin(mouse);
    if (pid >= 0) {
        m_DraggingFromPin = pid;
        return;
    }
    int nid = HitTestNode(ScreenToCanvas(mouse));
    if (nid >= 0) {
        if (io.KeyAlt) {
            PushUndo();
            Node* src = GetNodeById(nid);
            if (src) {
                int newId = GetActiveNextId()++;
                auto dup = NodeFactory::Global().Create(src->GetTypeName(), newId);
                if (dup) {
                    dup->m_Pos = ScreenToCanvas(mouse);
                    m_DraggedNodeId = newId;
                    m_DragOffset = ImVec2(0, 0);
                    GetActiveNodes().push_back(std::move(dup));
                }
            }
        } else if (io.KeyShift) {
            Node* n = GetNodeById(nid);
            if (n) {
                n->m_Selected = !n->m_Selected;
                if (n->m_Selected) m_SelectedNodeId = nid;
                else {
                    m_SelectedNodeId = -1;
                    for (auto& nn : GetActiveNodes())
                        if (nn->m_Selected) { m_SelectedNodeId = nn->m_Id; break; }
                }
            }
            m_DraggedNodeId = nid;
            Node* dn = GetNodeById(nid);
            if (dn) m_DragOffset = ScreenToCanvas(mouse) - dn->m_Pos;
        } else {
            SelectNode(nid);
            m_DraggedNodeId = nid;
            Node* n = GetNodeById(nid);
            if (n) m_DragOffset = ScreenToCanvas(mouse) - n->m_Pos;
        }
        return;
    }
    int rid = !IsEditingComponent() ? HitTestRegion(ScreenToCanvas(mouse)) : -1;
    if (rid >= 0) {
        m_SelectedRegionId = rid;
        m_SelectedNodeId = -1;
        m_SelectedLinkId = -1;
        for (auto& r : m_Regions) {
            if (r.id == rid) {
                ImVec2 rScreen = CanvasToScreen(r.pos);
                ImVec2 rScreenEnd = CanvasToScreen(r.pos + r.size);
                float resizeHandle = 12 * m_Zoom;
                if (mouse.x > rScreenEnd.x - resizeHandle && mouse.y > rScreenEnd.y - resizeHandle)
                    m_DraggedRegionEdge = 1;
                else if ((mouse.y - rScreen.y) < 24 * m_Zoom) {
                    float arrowW = 22 * m_Zoom;
                    if (mouse.x > rScreen.x + arrowW) {
                        m_DraggedRegionEdge = 0;
                        m_RegionDragOffset = ScreenToCanvas(mouse) - r.pos;
                    }
                }
                break;
            }
        }
    } else {
        m_BoxSelecting = true;
        m_BoxSelectStart = ScreenToCanvas(mouse);
        m_BoxSelectEnd = m_BoxSelectStart;
        if (!io.KeyShift) {
            DeselectAll();
            m_SelectedRegionId = -1;
        }
    }
}

void NodeEditor::HandleNodeDrag(ImVec2 mouse) {
    if (m_DraggedNodeId < 0) return;
    if (ImGui::IsMouseDragging(0)) {
        Node* n = GetNodeById(m_DraggedNodeId);
        if (n && isfinite(m_DragOffset.x) && isfinite(m_DragOffset.y)) {
            ImVec2 newPos = ScreenToCanvas(mouse) - m_DragOffset;
            if (m_SnapToGrid) {
                newPos.x = SnapVal(newPos.x, 50.0f);
                newPos.y = SnapVal(newPos.y, 50.0f);
            }
            ImVec2 delta = newPos - n->m_Pos;
            n->m_Pos = newPos;
            if (delta.x != 0 || delta.y != 0) {
                for (auto& other : GetActiveNodes()) {
                    if (other->m_Selected && other->m_Id != m_DraggedNodeId)
                        other->m_Pos = other->m_Pos + delta;
                }
            }
        }
    }
    if (ImGui::IsMouseReleased(0)) {
        PushUndo();
        Node* dn = GetNodeById(m_DraggedNodeId);
        if (dn) TryAutoConnect(dn, mouse);
        m_DraggedNodeId = -1;
    }
}

void NodeEditor::HandleBoxSelect() {
    if (!m_BoxSelecting) return;
    if (ImGui::IsMouseDragging(0))
        m_BoxSelectEnd = ScreenToCanvas(ImGui::GetIO().MousePos);
    if (ImGui::IsMouseReleased(0)) {
        ImVec2 s = ImMin(m_BoxSelectStart, m_BoxSelectEnd);
        ImVec2 e = ImMax(m_BoxSelectStart, m_BoxSelectEnd);
        ImRect box(s, e);
        DeselectAll();
        for (auto& node : GetActiveNodes()) {
            if (box.Overlaps(ImRect(node->m_Pos, node->m_Pos + node->m_Size))) {
                node->m_Selected = true;
                m_SelectedNodeId = node->m_Id;
            }
        }
        m_BoxSelecting = false;
    }
}

void NodeEditor::HandleLinkDrop(ImVec2 mouse, bool hovered) {
    if (m_DraggingFromPin < 0 || !ImGui::IsMouseReleased(0)) return;
    if (hovered) {
        int toPin = HitTestPin(mouse);
        float snapD = 18.0f;
        if (toPin < 0) {
            float bestD = snapD;
            for (auto& n : GetActiveNodes()) {
                for (auto& pin : n->m_Inputs) {
                    if (pin.id == m_DraggingFromPin) continue;
                    Pin* fromP2 = GetPinById(m_DraggingFromPin);
                    if (fromP2 && (pin.type != fromP2->type || fromP2->kind != PinKind::Output)) continue;
                    float d = ImSqrt(ImLengthSqr(GetPinScreenPos(pin, *n) - mouse));
                    if (d < bestD) { bestD = d; toPin = pin.id; }
                }
                for (auto& pin : n->m_Outputs) {
                    if (pin.id == m_DraggingFromPin) continue;
                    Pin* fromP2 = GetPinById(m_DraggingFromPin);
                    if (fromP2 && (pin.type != fromP2->type || fromP2->kind != PinKind::Input)) continue;
                    float d = ImSqrt(ImLengthSqr(GetPinScreenPos(pin, *n) - mouse));
                    if (d < bestD) { bestD = d; toPin = pin.id; }
                }
            }
        }
        if (toPin >= 0 && toPin != m_DraggingFromPin) {
            Pin* fromP = GetPinById(m_DraggingFromPin);
            Pin* toP = GetPinById(toPin);
            if (fromP && toP && fromP->kind != toP->kind && fromP->type == toP->type) {
                int src, dst;
                if (fromP->kind == PinKind::Output && toP->kind == PinKind::Input)
                    { src = m_DraggingFromPin; dst = toPin; }
                else if (fromP->kind == PinKind::Input && toP->kind == PinKind::Output)
                    { src = toPin; dst = m_DraggingFromPin; }
                else
                    { src = dst = -1; }
                if (src >= 0) {
                    Pin* srcP = GetPinById(src);
                    Pin* dstP = GetPinById(dst);
                    bool valid = true;
                    if (srcP && dstP) {
                        Node* srcN = GetNodeByPinId(src);
                        Node* dstN = GetNodeByPinId(dst);
                        if (srcN && dstN) {
                            if (srcN->m_Id == dstN->m_Id)
                                valid = false;
                            if (strcmp(srcN->GetTypeName(), "TabItem") == 0 && strcmp(dstN->GetTypeName(), "TabBar") != 0)
                                valid = false;
                            if (strcmp(srcN->GetTypeName(), "SubMenu") == 0 && 
                                strcmp(dstN->GetTypeName(), "MenuBar") != 0 && strcmp(dstN->GetTypeName(), "SubMenu") != 0)
                                valid = false;
                            if (strcmp(srcN->GetTypeName(), "MenuItem") == 0 && strcmp(dstN->GetTypeName(), "SubMenu") != 0)
                                valid = false;
                            if (strcmp(srcN->GetTypeName(), "MenuBar") == 0 &&
                                strcmp(dstN->GetTypeName(), "Window") != 0 &&
                                strcmp(dstN->GetTypeName(), "NodePreview") != 0)
                                valid = false;
                        }
                    }
                    if (!valid) { src = -1; dst = -1; }
                }
                if (src >= 0) {
                    bool dup = false;
                    for (auto& l : GetActiveLinks())
                        if (l.fromPinId == src && l.toPinId == dst) { dup = true; break; }
                    if (dup) { src = -1; }
                    if (src >= 0) {
                        Pin* pinDst = GetPinById(dst);
                        if (pinDst && pinDst->type == PinType::Theme)
                            for (auto& l : GetActiveLinks())
                                if (l.toPinId == dst) { src = -1; break; }
                    }
                    if (src >= 0) {
                        PushUndo();
                        GetActiveLinks().push_back({GetActiveNextId()++, src, dst});
                    }
                }
            }
        }
    }
    m_DraggingFromPin = -1;
}

void NodeEditor::HandleDoubleClick(ImVec2 mouse, bool hovered) {
    if (!hovered || !ImGui::IsMouseDoubleClicked(0)) return;
    int nid = HitTestNode(ScreenToCanvas(mouse));
    if (nid < 0) return;
    Node* rn = GetNodeById(nid);
    if (!rn) return;
    if (strcmp(rn->GetTypeName(), "Component") == 0)
        EnterComponent(static_cast<ComponentNode*>(rn));
    else {
        m_RenameNodeId = nid;
        snprintf(m_RenameBuffer, sizeof(m_RenameBuffer), "%s", rn->m_Name.c_str());
    }
}

void NodeEditor::HandleKeyboard() {
    ImGuiIO& io = ImGui::GetIO();
    if (m_BoxSelecting || io.WantTextInput || m_SearchOpen) return;
    // Delete
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        if (m_SelectedLinkId >= 0) {
            PushUndo();
            GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                [this](const Link& l) { return l.id == m_SelectedLinkId; }), GetActiveLinks().end());
            m_SelectedLinkId = -1;
        } else if (m_SelectedNodeId >= 0) {
            DeleteSelectedNode();
        }
    }
    if (ImGui::IsKeyPressed(ImGuiKey_F))
        ZoomToFit();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) Undo();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)) Redo();

    // Search via hotkey
    if (Application::IsHotkeyPressed(HotkeyAction::Search)) {
        m_SearchOpen = true;
        m_SearchBuffer[0] = '\0';
        m_SearchSelectedIndex = 0;
        m_SearchOpenTime = ImGui::GetTime();
        ImVec2 neCenter = ImGui::GetWindowPos() + ImGui::GetWindowSize() * 0.5f;
        m_SearchSpawnPos = ScreenToCanvas(neCenter);
        m_SearchOrigin = io.MousePos;
    }

    // Copy/Paste/Cut/SelectAll/Find
    if (!io.WantTextInput) {
        if (Application::IsHotkeyPressed(HotkeyAction::Copy)) CopySelected();
        if (Application::IsHotkeyPressed(HotkeyAction::Paste))
            PasteClipboard(ScreenToCanvas(io.MousePos));
        if (Application::IsHotkeyPressed(HotkeyAction::Cut)) CutSelected();
        if (Application::IsHotkeyPressed(HotkeyAction::SelectAll)) SelectAll();
        if (Application::IsHotkeyPressed(HotkeyAction::Find)) OpenSearch();
    }
}

// ─── Selection helpers ───

void NodeEditor::SelectNode(int id) {
    for (auto& n : GetActiveNodes())
        n->m_Selected = (n->m_Id == id);
    if (id >= 0) m_NodeSelectTime[id] = ImGui::GetTime();
    m_SelectedNodeId = id;
    m_SelectedLinkId = -1;
    m_SelectedRegionId = -1;
}

void NodeEditor::DeselectAll() {
    for (auto& n : GetActiveNodes())
        n->m_Selected = false;
    m_SelectedNodeId = -1;
    m_SelectedLinkId = -1;
}

void NodeEditor::CutSelected() {
    PushUndo();
    CopySelected();
    for (auto it = GetActiveNodes().begin(); it != GetActiveNodes().end(); ) {
        if ((*it)->m_Selected) {
            for (auto& p : (*it)->m_Inputs) {
                GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                    [&](const Link& l) { return l.toPinId == p.id || l.fromPinId == p.id; }), GetActiveLinks().end());
            }
            for (auto& p : (*it)->m_Outputs) {
                GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                    [&](const Link& l) { return l.fromPinId == p.id || l.toPinId == p.id; }), GetActiveLinks().end());
            }
            it = GetActiveNodes().erase(it);
        } else {
            ++it;
        }
    }
    m_SelectedNodeId = -1;
}

void NodeEditor::SelectAll() {
    for (auto& n : GetActiveNodes()) n->m_Selected = true;
    if (!GetActiveNodes().empty()) m_SelectedNodeId = GetActiveNodes().back()->m_Id;
}

void NodeEditor::OpenSearch() {
    m_SearchOpen = true;
    m_SearchBuffer[0] = '\0';
    m_SearchSelectedIndex = 0;
    m_SearchOpenTime = ImGui::GetTime();
    ImVec2 neCenter = ImGui::GetWindowPos() + ImGui::GetWindowSize() * 0.5f;
    m_SearchSpawnPos = ScreenToCanvas(neCenter);
    m_SearchOrigin = ImGui::GetIO().MousePos;
}

// ─── Queries ───

Node* NodeEditor::GetNodeById(int id) const {
    for (auto& n : GetActiveNodes()) if (n->m_Id == id) return const_cast<Node*>(n.get());
    return nullptr;
}

Pin* NodeEditor::GetPinById(int id) const {
    for (auto& n : GetActiveNodes()) {
        for (auto& p : n->m_Inputs) if (p.id == id) return const_cast<Pin*>(&p);
        for (auto& p : n->m_Outputs) if (p.id == id) return const_cast<Pin*>(&p);
    }
    return nullptr;
}

// ─── New Project ───

void NodeEditor::NewProject() {
    m_Nodes.clear();
    m_Links.clear();
    m_ComponentStack.clear();
    m_SelectedNodeId = -1;
    m_SelectedLinkId = -1;
    m_NextId = 1;
    m_SearchSpawnPos = ImVec2(100, 100);
    m_UndoStack.clear();
    m_RedoStack.clear();
    m_Regions.clear();
}



void NodeEditor::ZoomToFit() {
    if (GetActiveNodes().empty()) return;
    ImVec2 minPos = GetActiveNodes()[0]->m_Pos;
    ImVec2 maxPos = minPos + GetActiveNodes()[0]->m_Size;
    for (auto& n : GetActiveNodes()) {
        minPos = ImMin(minPos, n->m_Pos);
        maxPos = ImMax(maxPos, n->m_Pos + n->m_Size);
    }
    ImVec2 range = maxPos - minPos + ImVec2(80, 80);
    ImVec2 canvasSize = ImGui::GetWindowSize();
    if (canvasSize.x < 1 || canvasSize.y < 1) return;
    float zoomX = canvasSize.x / range.x;
    float zoomY = canvasSize.y / range.y;
    m_Zoom = std::min(zoomX, zoomY);
    if (m_Zoom > 2.0f) m_Zoom = 2.0f;
    if (m_Zoom < 0.1f) m_Zoom = 0.1f;
    m_CanvasOrigin = ImVec2(
        canvasSize.x * 0.5f - (minPos.x + range.x * 0.5f) * m_Zoom,
        canvasSize.y * 0.5f - (minPos.y + range.y * 0.5f) * m_Zoom);
}

void NodeEditor::AutoArrangeSelected() {
    std::vector<Node*> sel;
    for (auto& n : GetActiveNodes()) if (n->m_Selected) sel.push_back(n.get());
    if (sel.empty()) { for (auto& n : GetActiveNodes()) sel.push_back(n.get()); }
    if (sel.empty()) return;

    PushUndo();

    auto graph = BuildNodeGraph(sel);
    auto depthInfo = ComputeDepths(sel, graph.childrenOf);

    // ── Layout: group roots by parent, position parents centered over children ──
    float spacingX = 320.0f, spacingY = 80.0f, groupGapY = 100.0f;
    ImVec2 startPos = sel[0]->m_Pos;
    for (auto* n : sel) startPos = ImMin(startPos, n->m_Pos);

    std::unordered_map<int, float> yPos;

    // Map each parent at depth 1 to its children (roots) in pin connection order
    std::unordered_map<int, std::vector<Node*>> rootsByParent;
    std::unordered_map<int, int> childOrder;
    for (auto* root : depthInfo.roots) {
        Node* parent = nullptr;
        int pinIdx = 0;
        for (auto& link : GetActiveLinks()) {
            for (auto& op : root->m_Outputs) {
                if (op.id != link.fromPinId) continue;
                for (auto* other : sel) {
                    if (other->m_Id == root->m_Id) continue;
                    for (size_t pi = 0; pi < other->m_Inputs.size(); pi++) {
                        auto& ip = other->m_Inputs[pi];
                        if (ip.id == link.toPinId) {
                            parent = other;
                            pinIdx = (int)pi;
                            goto foundParent;
                        }
                    }
                }
            }
        }
        foundParent:;
        if (parent && depthInfo.depth[parent->m_Id] == 1) {
            rootsByParent[parent->m_Id].push_back(root);
            childOrder[root->m_Id] = pinIdx;
        }
    }

    // Position each parent's group
    float currentY = startPos.y;
    for (auto& kv : rootsByParent) {
        Node* parent = GetNodeById(kv.first);
        if (!parent) continue;
        auto& group = kv.second;
        std::sort(group.begin(), group.end(), [&](Node* a, Node* b) {
            return childOrder[a->m_Id] < childOrder[b->m_Id];
        });
        float groupStartY = currentY;
        for (size_t i = 0; i < group.size(); i++)
            yPos[group[i]->m_Id] = groupStartY + i * spacingY;
        float groupMid = groupStartY + (group.size() - 1) * spacingY * 0.5f;
        yPos[parent->m_Id] = groupMid;
        currentY = groupStartY + group.size() * spacingY + groupGapY;
    }

    // Roots without a parent at depth 1
    for (auto* root : depthInfo.roots) {
        if (yPos.find(root->m_Id) == yPos.end()) {
            yPos[root->m_Id] = currentY;
            currentY += spacingY;
        }
    }

    // Position nodes at depth > 1: center over children or below parent
    for (int d = 2; d <= depthInfo.maxDepth; d++) {
        auto& level = depthInfo.byDepth[d];
        for (auto* n : level) {
            float sumY = 0; int count = 0;
            for (auto* child : graph.childrenOf[n->m_Id]) {
                auto it = yPos.find(child->m_Id);
                if (it != yPos.end()) { sumY += it->second; count++; }
            }
            if (count > 0) yPos[n->m_Id] = sumY / count;
            else {
                auto pIt = graph.parentOf.find(n->m_Id);
                if (pIt != graph.parentOf.end()) {
                    auto py = yPos.find(pIt->second->m_Id);
                    yPos[n->m_Id] = (py != yPos.end()) ? py->second : currentY;
                } else {
                    yPos[n->m_Id] = currentY;
                }
                currentY += spacingY;
            }
        }
    }

    // Ensure minimum spacing between all placed nodes
    std::vector<std::pair<int, float>> allNodes;
    for (auto& kv : yPos) allNodes.push_back(kv);
    std::sort(allNodes.begin(), allNodes.end(),
        [](auto& a, auto& b) { return a.second < b.second; });
    for (size_t i = 1; i < allNodes.size(); i++) {
        float expected = allNodes[i - 1].second + spacingY * 0.5f;
        if (allNodes[i].second < expected) allNodes[i].second = expected;
        yPos[allNodes[i].first] = allNodes[i].second;
    }

    // Apply positions
    for (auto* n : sel) {
        auto it = yPos.find(n->m_Id);
        if (it != yPos.end())
            n->m_Pos = ImVec2(startPos.x + depthInfo.depth[n->m_Id] * spacingX, it->second);
    }
}

NodeEditor::NodeGraph NodeEditor::BuildNodeGraph(const std::vector<Node*>& sel) {
    NodeGraph g;
    for (auto* n : sel) g.parentCount[n->m_Id] = 0;
    for (auto& link : GetActiveLinks()) {
        int fromId = -1, toId = -1;
        for (auto& n : GetActiveNodes()) {
            for (auto& p : n->m_Outputs) if (p.id == link.fromPinId) { fromId = n->m_Id; break; }
            for (auto& p : n->m_Inputs) if (p.id == link.toPinId) { toId = n->m_Id; break; }
        }
        if (fromId < 0 || toId < 0) continue;
        bool inF = false, inT = false;
        for (auto* s : sel) { if (s->m_Id == fromId) inF = true; if (s->m_Id == toId) inT = true; }
        if (inF && inT) {
            Node* fn = GetNodeById(fromId), * tn = GetNodeById(toId);
            if (fn && tn) { g.childrenOf[fromId].push_back(tn); g.parentCount[toId]++; g.parentOf[toId] = fn; }
        }
    }
    return g;
}

NodeEditor::DepthInfo NodeEditor::ComputeDepths(const std::vector<Node*>& sel,
    const std::unordered_map<int, std::vector<Node*>>& childrenOf) {
    DepthInfo di;
    std::unordered_map<int, int> pc;
    for (auto* n : sel) pc[n->m_Id] = 0;
    for (auto& kv : childrenOf)
        for (auto* child : kv.second)
            pc[child->m_Id]++;
    for (auto* n : sel) if (pc[n->m_Id] == 0) di.roots.push_back(n);
    if (di.roots.empty() && !sel.empty()) di.roots.push_back(sel[0]);

    std::function<void(Node*, int)> assignDepth = [&](Node* n, int d) {
        auto it = di.depth.find(n->m_Id);
        if (it != di.depth.end() && it->second >= d) return;
        di.depth[n->m_Id] = d;
        auto chIt = childrenOf.find(n->m_Id);
        if (chIt != childrenOf.end())
            for (auto* child : chIt->second) assignDepth(child, d + 1);
    };
    for (auto* root : di.roots) assignDepth(root, 0);
    for (auto* n : sel) if (di.depth.find(n->m_Id) == di.depth.end()) di.depth[n->m_Id] = 0;

    for (auto* n : sel) {
        di.byDepth[di.depth[n->m_Id]].push_back(n);
        di.maxDepth = std::max(di.maxDepth, di.depth[n->m_Id]);
    }
    return di;
}

// ─── Delete selected node ───

void NodeEditor::DeleteSelectedNode() {
    PushUndo();
    // Delete all selected nodes
    for (auto it = GetActiveNodes().begin(); it != GetActiveNodes().end(); ) {
        if ((*it)->m_Selected) {
            for (auto& p : (*it)->m_Inputs) {
                GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                    [&](const Link& l) { return l.toPinId == p.id || l.fromPinId == p.id; }), GetActiveLinks().end());
            }
            for (auto& p : (*it)->m_Outputs) {
                GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                    [&](const Link& l) { return l.fromPinId == p.id || l.toPinId == p.id; }), GetActiveLinks().end());
            }
            it = GetActiveNodes().erase(it);
        } else {
            ++it;
        }
    }
    m_SelectedNodeId = -1;
}

// ─── Undo / Redo ───

std::string NodeEditor::SerializeState() const {
    std::string out;
    out += "nextId " + std::to_string(GetActiveNextId()) + "\n";
    for (auto& node : GetActiveNodes()) {
        out += "node " + std::to_string(node->m_Id) + " " + node->GetTypeName() + "\n";
        ImGuiTextBuffer b;
        b.appendf("name S \"%s\"\n", node->m_Name.c_str());
        b.appendf("pos V %.1f %.1f\n", node->m_Pos.x, node->m_Pos.y);
        node->SaveExtra(b);
        out += b.c_str();
        out += "end\n";
    }
    for (auto& link : GetActiveLinks())
        out += "link " + std::to_string(link.id) + " " +
               std::to_string(link.fromPinId) + " " + std::to_string(link.toPinId) + "\n";
    return out;
}

void NodeEditor::DeserializeState(const std::string& s) {
    GetActiveNodes().clear();
    GetActiveLinks().clear();
    m_SelectedNodeId = -1;
    m_SelectedLinkId = -1;

    std::istringstream stream(s);
    std::string line;
    Node* curNodeRaw = nullptr;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        int id;
        if (sscanf(line.c_str(), "nextId %d", &GetActiveNextId()) == 1) continue;
        char type[64];
        if (sscanf(line.c_str(), "node %d %63s", &id, type) == 2) {
            auto newNode = NodeFactory::Global().Create(type, id);
            if (newNode) {
                curNodeRaw = newNode.get();
                GetActiveNodes().push_back(std::move(newNode));
            } else {
                curNodeRaw = nullptr;
            }
            continue;
        }
        if (line == "end") { curNodeRaw = nullptr; continue; }
        if (curNodeRaw) {
            char key[64];
            if (sscanf(line.c_str(), " %63s", key) == 1) {
                const char* rest = line.c_str() + strlen(key);
                while (*rest == ' ') rest++;
                if (strcmp(key, "name") == 0) {
                    char val[256];
                    if (sscanf(rest, " S \"%255[^\"]\"", val) == 1)
                        curNodeRaw->m_Name = val;
                } else if (strcmp(key, "pos") == 0) {
                    sscanf(rest, " V %f %f", &curNodeRaw->m_Pos.x, &curNodeRaw->m_Pos.y);
                } else {
                    curNodeRaw->LoadExtra(key, rest);
                }
            }
        }
        Link l;
        if (sscanf(line.c_str(), "link %d %d %d", &l.id, &l.fromPinId, &l.toPinId) == 3)
            GetActiveLinks().push_back(l);
    }

    // Migrate old-format links (id*100+N → id*10000+N)
    auto needsPinMigration = [&]() -> bool {
        for (auto& l : GetActiveLinks()) {
            auto isOldPin = [&](int pid) {
                int oldId = pid / 100, newId = pid / 10000;
                if (oldId == newId) return false;
                return GetNodeById(oldId) != nullptr;
            };
            if (isOldPin(l.fromPinId) || isOldPin(l.toPinId)) return true;
        }
        return false;
    };
    if (needsPinMigration()) {
        for (auto& l : GetActiveLinks()) {
            l.fromPinId = (l.fromPinId / 100) * 10000 + (l.fromPinId % 100);
            l.toPinId = (l.toPinId / 100) * 10000 + (l.toPinId % 100);
        }
    }
}

void NodeEditor::PushUndo() {
    m_UndoStack.push_back(SerializeState());
    if (m_UndoStack.size() > MAX_UNDO)
        m_UndoStack.erase(m_UndoStack.begin());
    m_RedoStack.clear();
}

void NodeEditor::Undo() {
    if (m_UndoStack.empty()) return;
    m_RedoStack.push_back(SerializeState());
    DeserializeState(m_UndoStack.back());
    m_UndoStack.pop_back();
}

void NodeEditor::Redo() {
    if (m_RedoStack.empty()) return;
    m_UndoStack.push_back(SerializeState());
    DeserializeState(m_RedoStack.back());
    m_RedoStack.pop_back();
}

void NodeEditor::SaveToFile(const char* path) {
    FILE* f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "nextId %d\n", m_NextId);
    for (auto& node : m_Nodes) {
        fprintf(f, "node %d %s\n", node->m_Id, node->GetTypeName());
        ImGuiTextBuffer b;
        b.appendf("name S \"%s\"\n", node->m_Name.c_str());
        b.appendf("pos V %.1f %.1f\n", node->m_Pos.x, node->m_Pos.y);
        node->SaveExtra(b);
        fprintf(f, "%s", b.c_str());
        fprintf(f, "end\n");
    }
    for (auto& link : m_Links)
        fprintf(f, "link %d %d %d\n", link.id, link.fromPinId, link.toPinId);
    fclose(f);
}

void NodeEditor::LoadFromFile(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return;
    m_ComponentStack.clear();
    m_Nodes.clear();
    m_Links.clear();
    m_SelectedNodeId = -1;
    m_SelectedLinkId = -1;

    std::string lineBuf;
    Node* curNodeRaw = nullptr;
    while (true) {
        lineBuf.clear();
        int c;
        while ((c = fgetc(f)) != EOF && c != '\n') lineBuf.push_back((char)c);
        if (lineBuf.empty() && c == EOF) break;
        const char* line = lineBuf.c_str();

        if (sscanf(line, "nextId %d", &m_NextId) == 1) continue;

        char type[64];
        int id;
        if (sscanf(line, "node %d %63s", &id, type) == 2) {
            auto newNode = NodeFactory::Global().Create(type, id);
            if (newNode) {
                curNodeRaw = newNode.get();
                m_Nodes.push_back(std::move(newNode));
            } else {
                curNodeRaw = nullptr;
            }
            continue;
        }

        if (strcmp(line, "end") == 0) { curNodeRaw = nullptr; continue; }

        if (curNodeRaw) {
            char key[64];
            if (sscanf(line, " %63s", key) == 1) {
                const char* rest = line + strlen(key);
                while (*rest == ' ') rest++;

                if (strcmp(key, "name") == 0) {
                    char val[256];
                    if (sscanf(rest, " S \"%255[^\"]\"", val) == 1)
                        curNodeRaw->m_Name = val;
                } else if (strcmp(key, "pos") == 0) {
                    sscanf(rest, " V %f %f", &curNodeRaw->m_Pos.x, &curNodeRaw->m_Pos.y);
                } else {
                    curNodeRaw->LoadExtra(key, rest);
                }
            }
        }

        Link l;
        if (sscanf(line, "link %d %d %d", &l.id, &l.fromPinId, &l.toPinId) == 3)
            m_Links.push_back(l);
    }
    fclose(f);
    m_SearchSpawnPos = ImVec2(100, 100);

    // Migrate old-format links (id*100+N → id*10000+N)
    auto needsPinMigration = [&]() -> bool {
        for (auto& l : m_Links) {
            auto isOldPin = [&](int pid) {
                int oldId = pid / 100, newId = pid / 10000;
                if (oldId == newId) return false;
                return GetNodeById(oldId) != nullptr;
            };
            if (isOldPin(l.fromPinId) || isOldPin(l.toPinId)) return true;
        }
        return false;
    };
    if (needsPinMigration()) {
        for (auto& l : m_Links) {
            l.fromPinId = (l.fromPinId / 100) * 10000 + (l.fromPinId % 100);
            l.toPinId = (l.toPinId / 100) * 10000 + (l.toPinId % 100);
        }
    }
}

Node* NodeEditor::GetNodeByPinId(int pinId) const {
    for (auto& n : GetActiveNodes()) {
        for (auto& p : n->m_Inputs) if (p.id == pinId) return const_cast<Node*>(n.get());
        for (auto& p : n->m_Outputs) if (p.id == pinId) return const_cast<Node*>(n.get());
    }
    return nullptr;
}

// ─── Regions ───

int NodeEditor::HitTestRegion(ImVec2 canvasPos) const {
    for (int i = (int)m_Regions.size() - 1; i >= 0; i--) {
        ImRect r(m_Regions[i].pos, m_Regions[i].pos + m_Regions[i].size);
        if (r.Contains(canvasPos)) return m_Regions[i].id;
    }
    return -1;
}

void NodeEditor::AddRegion(ImVec2 canvasPos) {
    int id = GetActiveNextId()++;
    Region r;
    r.id = id;
    r.name = "Region " + std::to_string(id);
    r.pos = canvasPos - ImVec2(100, 30);
    r.size = ImVec2(200, 150);
    m_Regions.push_back(r);
}

void NodeEditor::DrawRegions(ImDrawList* dl) {
    for (auto& r : m_Regions) {
        ImVec2 a = CanvasToScreen(r.pos);
        ImVec2 b = CanvasToScreen(r.pos + r.size);
        float h = 24 * m_Zoom;

        // Draw body only if not collapsed
        if (!r.collapsed) {
            ImU32 bg = ImColor(r.color.x, r.color.y, r.color.z, r.color.w);
            dl->AddRectFilled(a, b, bg, 6 * m_Zoom);

        }

        // Title bar always visible
        ImU32 titleCol = ImColor(
            std::min(1.0f, r.color.x * 0.7f),
            std::min(1.0f, r.color.y * 0.7f),
            std::min(1.0f, r.color.z * 0.7f),
            std::min(1.0f, r.color.w * 1.5f));
        dl->AddRectFilled(a, ImVec2(b.x, a.y + h), titleCol, 6 * m_Zoom);
        dl->AddRectFilled(ImVec2(a.x, a.y + h - 4 * m_Zoom), ImVec2(b.x, a.y + h), titleCol, 0);

        if (!r.collapsed) {
            dl->AddRectFilled(ImVec2(a.x, a.y + h), ImVec2(b.x, a.y + h + 2 * m_Zoom), titleCol, 0);
        }

        // Collapse arrow button
        float arrowSize = 14 * m_Zoom;
        ImVec2 arrowPos(a.x + 4 * m_Zoom, a.y + (h - arrowSize) * 0.5f);
        ImGui::SetCursorScreenPos(arrowPos);
        ImGui::PushID(&r);
        if (ImGui::InvisibleButton("##collapse", ImVec2(arrowSize, arrowSize))) {
            m_Regions[(int)(&r - m_Regions.data())].collapsed = !r.collapsed;
        }
        ImGui::PopID();
        // Draw arrow icon using draw list (not text, to avoid font issues)
        const char* arrow = r.collapsed ? "\u25B6" : "\u25BC";
        dl->AddText(arrowPos, IM_COL32(220, 220, 230, 220), arrow);

        // Region name
        ImGui::SetCursorScreenPos(a + ImVec2(arrowSize + 8 * m_Zoom, 3 * m_Zoom));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(220, 220, 230, 220));
        ImGui::Text("%s", r.name.c_str());
        ImGui::PopStyleColor();

        // Resize handle (only when not collapsed)
        if (!r.collapsed) {
            float hs = 10 * m_Zoom;
            ImVec2 br(b.x, b.y);
            dl->AddTriangleFilled(
                ImVec2(br.x - hs, br.y),
                ImVec2(br.x, br.y - hs),
                ImVec2(br.x, br.y),
                IM_COL32(255, 255, 255, 100));
        }
    }
}



