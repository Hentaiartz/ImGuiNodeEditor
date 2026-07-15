#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <imgui.h>
#include <imgui_internal.h>
#include "Node.h"

class ComponentNode;

class NodeEditor {
public:
    NodeEditor();
    ~NodeEditor() = default;

    void Draw();
    void DrawSearchWidget();
    void SaveToFile(const char* path);
    void LoadFromFile(const char* path);
    void NewProject();
    int GetNextId() { return GetActiveNextId()++; }

    Node* GetNodeById(int id) const;
    Pin* GetPinById(int id) const;
    Node* GetNodeByPinId(int pinId) const;

    std::vector<std::unique_ptr<Node>>& GetNodes() { return m_Nodes; }
    std::vector<Link>& GetLinks() { return m_Links; }
    int GetSelectedNodeId() const { return m_SelectedNodeId; }
    void SetSelectedNodeId(int id) { m_SelectedNodeId = id; if (id >= 0) m_NodeSelectTime[id] = ImGui::GetTime(); }

    bool& SnapToGrid() { return m_SnapToGrid; }
    void ZoomToFit();
    void AutoArrangeSelected();

    // Sub-canvas editing (stack for nested components)
    std::vector<ComponentNode*> m_ComponentStack;
    void EnterComponent(ComponentNode* comp);
    void ExitComponent();
    bool IsEditingComponent() const { return !m_ComponentStack.empty(); }
    ComponentNode* GetCurrentComponent() const { return m_ComponentStack.empty() ? nullptr : m_ComponentStack.back(); }

    std::vector<std::unique_ptr<Node>>& GetActiveNodes();
    const std::vector<std::unique_ptr<Node>>& GetActiveNodes() const;
    std::vector<Link>& GetActiveLinks();
    const std::vector<Link>& GetActiveLinks() const;
    int& GetActiveNextId();
    int GetActiveNextId() const;

    // Undo / Redo
    void PushUndo();
    void Undo();
    void Redo();
    bool CanUndo() const { return !m_UndoStack.empty(); }
    bool CanRedo() const { return !m_RedoStack.empty(); }

private:
    std::vector<std::unique_ptr<Node>> m_Nodes;
    std::vector<Link> m_Links;
    int m_NextId = 1;

    ImVec2 m_CanvasOrigin;
    float m_Zoom = 1.0f;
    float m_TargetZoom = 1.0f;

    bool m_Panning;
    ImVec2 m_PanStart;
    ImVec2 m_PanOffset;

    int m_DraggedNodeId;
    ImVec2 m_DragOffset;

    int m_DraggingFromPin;
    int m_HoveredPinId;
    int m_HoveredLinkId = -1;

    int m_SelectedNodeId;
    int m_SelectedLinkId;

    bool m_SearchOpen;
    char m_SearchBuffer[64];
    int m_SearchSelectedIndex = 0;
    ImVec2 m_SearchSpawnPos;
    ImVec2 m_SearchOrigin;
    double m_SearchOpenTime = 0.0;

    bool m_Placing = false;
    std::string m_PlacingType;
    std::unordered_map<int, double> m_NodeSelectTime;

    bool m_BoxSelecting = false;
    ImVec2 m_BoxSelectStart;
    ImVec2 m_BoxSelectEnd;

    bool m_SnapToGrid = false;

    // Rename state
    int m_RenameNodeId = -1;
    char m_RenameBuffer[128];

    // Undo/Redo stacks (serialized state strings)
    std::vector<std::string> m_UndoStack;
    std::vector<std::string> m_RedoStack;
    static constexpr int MAX_UNDO = 50;

    struct ClipboardEntry {
        struct PinLink { int fromEntryIdx; int fromPinType; int toEntryIdx; int toPinType; };
        std::vector<PinLink> pinLinks;
        std::string type;
        std::string extraData;
        ImVec2 offset;
    };
    std::vector<ClipboardEntry> m_Clipboard;

    void DrawGrid(ImDrawList* dl, const ImRect& rect);
    void DrawNode(Node* node, ImDrawList* dl);
    void DrawLinks(ImDrawList* dl);
    void DrawDragLink(ImDrawList* dl);
    void SelectNode(int id);
    void DeselectAll();

public:
    void CopySelected();
    void CutSelected();
    void PasteClipboard(ImVec2 canvasPos);
    void DeleteSelectedNode();
    void SelectAll();
    void OpenSearch();
    ImVec2 ScreenToCanvas(ImVec2 screen) const;

    std::string SerializeState() const;
    void DeserializeState(const std::string& s);

    ImVec2 CanvasToScreen(ImVec2 canvas) const;
    ImVec2 GetPinScreenPos(const Pin& pin, const Node& node) const;

private:
    void HandleInteraction(const ImRect& canvasRect);
    void NodeContextMenu(Node* node);
    void LinkContextMenu();
    void CanvasContextMenu();
    void SearchPopup();
    void AddNodeFromType(const char* type);

private:

    int HitTestPin(ImVec2 canvasPos) const;
    int HitTestNode(ImVec2 canvasPos) const;
    int HitTestLink(ImVec2 screenPos) const;

    // Interaction helpers (extracted from HandleInteraction)
    bool TryAutoConnect(Node* dropNode, ImVec2 screenPos);
    void HandlePlacing(ImVec2 mouse, bool hovered);
    void HandleZoomPan(bool hovered);
    void HandlePinHover(ImVec2 mouse, bool hovered);
    void HandleRegionDrag(ImVec2 mouse);
    void HandleLeftClick(ImVec2 mouse, bool hovered);
    void HandleNodeDrag(ImVec2 mouse);
    void HandleBoxSelect();
    void HandleLinkDrop(ImVec2 mouse, bool hovered);
    void HandleDoubleClick(ImVec2 mouse, bool hovered);
    void HandleKeyboard();

    // Auto-arrange helpers
    struct NodeGraph {
        std::unordered_map<int, std::vector<class Node*>> childrenOf;
        std::unordered_map<int, class Node*> parentOf;
        std::unordered_map<int, int> parentCount;
    };
    NodeGraph BuildNodeGraph(const std::vector<class Node*>& sel);
    struct DepthInfo {
        std::vector<class Node*> roots;
        std::unordered_map<int, int> depth;
        std::unordered_map<int, std::vector<class Node*>> byDepth;
        int maxDepth = 0;
    };
    DepthInfo ComputeDepths(const std::vector<class Node*>& sel,
                            const std::unordered_map<int, std::vector<class Node*>>& childrenOf);

    // ─── Regions ───
public:
    struct Region {
        int id;
        std::string name;
        ImVec2 pos;
        ImVec2 size;
        ImVec4 color = ImVec4(0.3f, 0.4f, 0.6f, 0.2f);
        bool collapsed = false;
    };
    std::vector<Region>& GetRegions() { return m_Regions; }
    int GetSelectedRegionId() const { return m_SelectedRegionId; }
    void SetSelectedRegionId(int id) { m_SelectedRegionId = id; }
    int HitTestRegion(ImVec2 canvasPos) const;
    void AddRegion(ImVec2 canvasPos);
    void DrawRegions(ImDrawList* dl);

private:
    std::vector<Region> m_Regions;
    int m_SelectedRegionId = -1;
    int m_DraggedRegionEdge = -1; // 0=move, 1-4=resize corners
    ImVec2 m_RegionDragOffset;
};
