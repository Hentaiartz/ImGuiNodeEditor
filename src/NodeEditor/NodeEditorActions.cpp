#include "NodeEditor.h"
#include "Core/Application.h"
#include "Core/NodeFactory.h"
#include <cstring>
#include <cctype>
#include <cstdio>
#include <sstream>
#include <algorithm>

static bool stristr(const char* h, const char* n) {
    if (!n || !*n) return true;
    if (!h) return false;
    for (; *h; ++h) {
        const char* hh = h, *nn = n;
        while (*hh && *nn && tolower((unsigned char)*hh) == tolower((unsigned char)*nn))
            ++hh, ++nn;
        if (!*nn) return true;
    }
    return false;
}

// ─── Search widget ───

static ImU32 CatColor(const char* cat) {
    return NodeFactory::Global().GetCategoryColor(cat);
}

void NodeEditor::DrawSearchWidget() {
    if (!m_SearchOpen) {
        m_SearchBuffer[0] = '\0';
        return;
    }

    // ── Fade-in animation ──
    float elapsed = (float)(ImGui::GetTime() - m_SearchOpenTime);
    float fadeIn = std::min(1.0f, elapsed / 0.18f);
    float smoothFade = fadeIn * fadeIn * (3.0f - 2.0f * fadeIn); // smoothstep

    ImGui::OpenPopup("Add Node");

    auto& t = Application::GetConfig();
    // Animate popup background alpha
    ImVec4 popupBg = t.windowBg;
    popupBg.w *= smoothFade;
    ImGui::PushStyleColor(ImGuiCol_PopupBg, popupBg);
    ImGui::PushStyleColor(ImGuiCol_Border, t.border);
    ImGui::PushStyleColor(ImGuiCol_Separator, t.border);

    // Animate window size (scale from 0.9 → 1.0)
    float scale = 0.9f + 0.1f * smoothFade;
    ImGui::SetNextWindowSize(ImVec2(440 * scale, 380 * scale));
    ImGui::SetNextWindowPos(m_SearchOrigin, ImGuiCond_Appearing, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, smoothFade);
    if (!ImGui::BeginPopup("Add Node", ImGuiWindowFlags_NoMove)) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        m_SearchOpen = false;
        return;
    }

    auto& io = ImGui::GetIO();
    bool wasClickedOutside = false;
    if (io.MouseClicked[0] || io.MouseClicked[1]) {
        ImVec2 m = io.MousePos;
        ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 ws = ImGui::GetWindowSize();
        if (m.x < wp.x || m.x > wp.x + ws.x || m.y < wp.y || m.y > wp.y + ws.y)
            wasClickedOutside = true;
    }

    // Search input
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(t.frameBg.x, t.frameBg.y, t.frameBg.z, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(t.frameBg.x*1.2f, t.frameBg.y*1.2f, t.frameBg.z*1.2f, 0.6f));
    if (!ImGui::IsAnyItemActive()) ImGui::SetKeyboardFocusHere(0);
    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##filter", " \uF002  Type to search nodes...", m_SearchBuffer, sizeof(m_SearchBuffer));
    ImGui::PopItemWidth();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
    ImGui::Spacing();

    // Track if search buffer was just cleared → collapse all categories
    static bool wasFiltering = false;
    bool isFiltering = (m_SearchBuffer[0] != '\0');
    bool clearTransition = wasFiltering && !isFiltering;
    wasFiltering = isFiltering;

    auto& entries = NodeFactory::Global().GetEntries();
    size_t entriesCount = entries.size();

    // Collect visible matches
    struct Match { const NodeEntry* entry; int index; };
    std::vector<Match> visible;
    for (size_t i = 0; i < entriesCount; i++) {
        auto& e = entries[i];
        bool match = !isFiltering
            || stristr(e.name.c_str(), m_SearchBuffer)
            || stristr(e.cat.c_str(), m_SearchBuffer);
        if (!match) continue;
        if (e.name == "ComponentOutput" && !IsEditingComponent()) continue;
        visible.push_back({&e, (int)i});
    }

    // Arrow key navigation
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && !visible.empty()) {
        m_SearchSelectedIndex--;
        if (m_SearchSelectedIndex < 0) m_SearchSelectedIndex = (int)visible.size() - 1;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && !visible.empty()) {
        m_SearchSelectedIndex++;
        if (m_SearchSelectedIndex >= (int)visible.size()) m_SearchSelectedIndex = 0;
    }
    if (m_SearchSelectedIndex >= (int)visible.size()) m_SearchSelectedIndex = 0;
    if (m_SearchSelectedIndex < 0) m_SearchSelectedIndex = 0;

    // Find selected match
    const NodeEntry* selectedEntry = nullptr;
    if (!visible.empty() && m_SearchSelectedIndex < (int)visible.size())
        selectedEntry = visible[m_SearchSelectedIndex].entry;

    int matchIdx = 0;
    const char* lastCat = nullptr;
    bool catOpen = true;
    ImGui::BeginChild("##nodeList", ImVec2(0, 0), false);
    for (auto& e : entries) {
        bool match = !isFiltering
            || stristr(e.name.c_str(), m_SearchBuffer)
            || stristr(e.cat.c_str(), m_SearchBuffer);
        if (!match) continue;
        if (e.name == "ComponentOutput" && !IsEditingComponent()) continue;

        if (!lastCat || strcmp(lastCat, e.cat.c_str()) != 0) {
            if (lastCat) ImGui::Spacing();
            if (isFiltering)
                ImGui::SetNextItemOpen(true);
            else if (clearTransition)
                ImGui::SetNextItemOpen(false);

            ImU32 catCol = CatColor(e.cat.c_str());
            float headerH = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2 + 4;
            ImVec2 hPos = ImGui::GetCursorScreenPos();
            float hWidth = ImGui::GetContentRegionAvail().x;

            // Gradient: left 20% opacity → right 0% opacity
            ImU32 colLeft  = (catCol & 0x00FFFFFF) | (int(0.2f * 255) << 24);
            ImU32 colRight = catCol & 0x00FFFFFF; // alpha = 0
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                hPos, hPos + ImVec2(hWidth, headerH),
                colLeft, colRight, colRight, colLeft);

            char catLabel[64];
            snprintf(catLabel, sizeof(catLabel), "%s", e.cat.c_str());
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(210, 210, 220, 255));
            ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 255, 255, 18));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 255, 255, 30));
            catOpen = ImGui::CollapsingHeader(catLabel,
                ImGuiTreeNodeFlags_SpanAvailWidth);
            ImGui::PopStyleColor(4);

            if (!catOpen) { lastCat = e.cat.c_str(); continue; }
            lastCat = e.cat.c_str();
        } else if (!catOpen) {
            continue;
        }

        bool isSelected = (&e == selectedEntry);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
        ImU32 dotCol = CatColor(e.cat.c_str());
        ImVec2 p0 = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddCircleFilled(
            p0 + ImVec2(8, ImGui::GetTextLineHeight() / 2 + 1), 4, dotCol);

        char label[128];
        snprintf(label, sizeof(label), "  %s##%s", e.name.c_str(), e.name.c_str());
        ImGui::Indent(8);
        if (ImGui::Selectable(label, isSelected, ImGuiSelectableFlags_SpanAvailWidth)) {
            m_Placing = true;
            m_PlacingType = e.name;
            m_SearchOpen = false;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0, 8.0f)) {
            m_Placing = true;
            m_PlacingType = e.name;
            m_SearchOpen = false;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", e.desc.c_str());
            m_SearchSelectedIndex = matchIdx;
        }
        ImGui::Unindent(8);
        ImGui::PopStyleVar();
        matchIdx++;
    }

    if (visible.empty())
        ImGui::TextColored(ImVec4(0.4f, 0.42f, 0.5f, 1.0f), "  No nodes match your search.");
    ImGui::EndChild();

    // Enter → select the highlighted entry (or first if nothing highlighted)
    if (ImGui::IsKeyPressed(ImGuiKey_Enter) && !visible.empty()) {
        const NodeEntry* pick = selectedEntry ? selectedEntry : visible[0].entry;
        m_Placing = true;
        m_PlacingType = pick->name;
        m_SearchOpen = false;
        ImGui::CloseCurrentPopup();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape) || wasClickedOutside) {
        m_SearchOpen = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
    ImGui::PopStyleVar(); // Alpha
    ImGui::PopStyleColor(3);
}

// ─── Context menus ───

void NodeEditor::NodeContextMenu(Node* node) {
    if (!node) return;
    ImGui::TextDisabled("%s Node", node->GetTypeName());
    ImGui::Separator();
    if (ImGui::MenuItem("Cut", "Ctrl+X")) {
        SelectNode(node->m_Id);
        CopySelected();
        for (auto& p : node->m_Inputs) {
            GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                [&](const Link& l) { return l.toPinId == p.id || l.fromPinId == p.id; }), GetActiveLinks().end());
        }
        for (auto& p : node->m_Outputs) {
            GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                [&](const Link& l) { return l.fromPinId == p.id || l.toPinId == p.id; }), GetActiveLinks().end());
        }
        GetActiveNodes().erase(std::remove_if(GetActiveNodes().begin(), GetActiveNodes().end(),
            [node](const auto& n) { return n->m_Id == node->m_Id; }), GetActiveNodes().end());
        m_SelectedNodeId = -1;
    }
    if (ImGui::MenuItem("Copy", "Ctrl+C")) {
        SelectNode(node->m_Id);
        CopySelected();
    }
    if (ImGui::MenuItem("Duplicate")) {
        AddNodeFromType(node->GetTypeName());
    }
    if (ImGui::MenuItem("Delete", "Del")) {
        PushUndo();
        for (auto& p : node->m_Inputs) {
            GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                [&](const Link& l) { return l.toPinId == p.id || l.fromPinId == p.id; }), GetActiveLinks().end());
        }
        for (auto& p : node->m_Outputs) {
            GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
                [&](const Link& l) { return l.fromPinId == p.id || l.toPinId == p.id; }), GetActiveLinks().end());
        }
        GetActiveNodes().erase(std::remove_if(GetActiveNodes().begin(), GetActiveNodes().end(),
            [node](const auto& n) { return n->m_Id == node->m_Id; }), GetActiveNodes().end());
        m_SelectedNodeId = -1;
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Select All", "Ctrl+A")) {
        for (auto& n : GetActiveNodes()) n->m_Selected = true;
        if (!GetActiveNodes().empty()) m_SelectedNodeId = GetActiveNodes().back()->m_Id;
    }
}

void NodeEditor::LinkContextMenu() {
    // Find the connected nodes for display
    Link* link = nullptr;
    for (auto& l : GetActiveLinks()) {
        if (l.id == m_SelectedLinkId) { link = &l; break; }
    }
    if (!link) return;

    Pin* fp = GetPinById(link->fromPinId);
    Pin* tp = GetPinById(link->toPinId);
    Node* fn = fp ? GetNodeByPinId(link->fromPinId) : nullptr;
    Node* tn = tp ? GetNodeByPinId(link->toPinId) : nullptr;

    ImGui::TextDisabled("Connection");
    if (fn && fp && tn && tp)
        ImGui::Text("%s(%s) \u2192 %s(%s)",
            fn->GetTypeName(), fp->name.c_str(),
            tn->GetTypeName(), tp->name.c_str());
    ImGui::Separator();
        if (ImGui::MenuItem("Disconnect", "Del")) {
            PushUndo();
            GetActiveLinks().erase(std::remove_if(GetActiveLinks().begin(), GetActiveLinks().end(),
            [this](const Link& l) { return l.id == m_SelectedLinkId; }), GetActiveLinks().end());
        m_SelectedLinkId = -1;
        ImGui::CloseCurrentPopup();
    }
}

void NodeEditor::CanvasContextMenu() {
    ImGui::TextDisabled("Canvas");
    ImGui::Separator();
    if (ImGui::MenuItem("Search...", "Space")) {
        m_SearchOpen = true;
        m_SearchBuffer[0] = '\0';
        m_SearchSelectedIndex = 0;
        m_SearchSpawnPos = ScreenToCanvas(ImGui::GetIO().MousePos);
        m_SearchOrigin = ImGui::GetIO().MousePos;
        ImGui::SetKeyboardFocusHere();
    }
    ImGui::Separator();
    if (!IsEditingComponent() && ImGui::MenuItem("Add Region")) {
        AddRegion(ScreenToCanvas(ImGui::GetIO().MousePos));
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Select All", "Ctrl+A")) {
        for (auto& n : GetActiveNodes()) n->m_Selected = true;
        if (!GetActiveNodes().empty()) m_SelectedNodeId = GetActiveNodes().back()->m_Id;
    }
    if (ImGui::MenuItem("Paste", "Ctrl+V")
        && !m_Clipboard.empty()) {
        PasteClipboard(ScreenToCanvas(ImGui::GetIO().MousePos));
    }
    if (ImGui::MenuItem("Deselect All")) {
        DeselectAll();
    }
}

void NodeEditor::AddNodeFromType(const char* type) {
    int id = GetActiveNextId()++;
    std::unique_ptr<Node> node;
    if (strcmp(type, "ComponentOutput") == 0) {
        if (!IsEditingComponent()) { ImGui::CloseCurrentPopup(); return; }
        for (auto& n : GetActiveNodes())
            if (strcmp(n->GetTypeName(), "ComponentOutput") == 0) { ImGui::CloseCurrentPopup(); return; }
    }
    node = NodeFactory::Global().Create(type, id);
    if (node) {
        PushUndo();
        node->m_Pos = m_SearchSpawnPos;
        m_SearchSpawnPos.x += 200;
        GetActiveNodes().push_back(std::move(node));
    }
}

// ─── Clipboard ───

void NodeEditor::CopySelected() {
    m_Clipboard.clear();
    ImVec2 center(0, 0);
    int count = 0;
    for (auto& n : GetActiveNodes()) {
        if (!n->m_Selected) continue;
        center = center + n->m_Pos + n->m_Size * 0.5f;
        count++;
    }
    if (count == 0) return;
    center = ImVec2(center.x / (float)count, center.y / (float)count);

    for (auto& n : GetActiveNodes()) {
        if (!n->m_Selected) continue;
        ClipboardEntry e;
        e.type = n->GetTypeName();
        e.offset = (n->m_Pos + n->m_Size * 0.5f) - center;
        ImGuiTextBuffer buf;
        n->SaveExtra(buf);
        if (buf.size() > 0) e.extraData = buf.c_str();
        m_Clipboard.push_back(e);
    }

    // Preserve links between copied nodes
    std::unordered_map<int, int> nodeToClipIdx;
    int clipIdx = 0;
    for (auto& n : GetActiveNodes()) {
        if (!n->m_Selected) continue;
        nodeToClipIdx[n->m_Id] = clipIdx++;
    }
    for (auto& link : GetActiveLinks()) {
        Pin* fromPin = GetPinById(link.fromPinId);
        Pin* toPin = GetPinById(link.toPinId);
        if (!fromPin || !toPin) continue;
        auto fromIt = nodeToClipIdx.find(fromPin->nodeId);
        auto toIt = nodeToClipIdx.find(toPin->nodeId);
        if (fromIt != nodeToClipIdx.end() && toIt != nodeToClipIdx.end()) {
            ClipboardEntry::PinLink pl;
            pl.fromEntryIdx = fromIt->second;
            pl.fromPinType = (int)fromPin->type;
            pl.toEntryIdx = toIt->second;
            pl.toPinType = (int)toPin->type;
            m_Clipboard[0].pinLinks.push_back(pl);
        }
    }
}

void NodeEditor::PasteClipboard(ImVec2 canvasPos) {
    if (m_Clipboard.empty()) return;
    PushUndo();
    ImVec2 total(0, 0);
    for (auto& e : m_Clipboard)
        total = total + e.offset;
    ImVec2 pasteCenter = canvasPos;

    for (auto& e : m_Clipboard) {
        int id = GetActiveNextId()++;
        auto node = NodeFactory::Global().Create(e.type.c_str(), id);
        if (node) {
            // Restore extra data via LoadExtra
            if (!e.extraData.empty()) {
                std::istringstream stream(e.extraData);
                std::string line;
                while (std::getline(stream, line)) {
                    if (line.empty()) continue;
                    char key[64];
                    if (sscanf(line.c_str(), " %63s", key) == 1) {
                        const char* rest = line.c_str() + strlen(key);
                        while (*rest == ' ') rest++;
                        node->LoadExtra(key, rest);
                    }
                }
            }
            node->m_Pos = pasteCenter + e.offset - node->m_Size * 0.5f;
            node->m_Selected = true;
            GetActiveNodes().push_back(std::move(node));
        }
    }
    if (!m_Clipboard.empty())
        m_SelectedNodeId = GetActiveNodes().back()->m_Id;

    // Restore links between pasted nodes
    if (!m_Clipboard.empty()) {
        auto& pinLinks = m_Clipboard[0].pinLinks;
        int baseIdx = (int)GetActiveNodes().size() - (int)m_Clipboard.size();
        for (auto& pl : pinLinks) {
            Node* fromNode = GetActiveNodes()[baseIdx + pl.fromEntryIdx].get();
            Node* toNode = GetActiveNodes()[baseIdx + pl.toEntryIdx].get();
            int fromPinId = -1;
            for (auto& pin : fromNode->m_Outputs) {
                if ((int)pin.type == pl.fromPinType) {
                    fromPinId = pin.id;
                    break;
                }
            }
            int toPinId = -1;
            for (auto& pin : toNode->m_Inputs) {
                if ((int)pin.type == pl.toPinType) {
                    toPinId = pin.id;
                    break;
                }
            }
            if (fromPinId >= 0 && toPinId >= 0) {
                Link l;
                l.id = GetActiveNextId()++;
                l.fromPinId = fromPinId;
                l.toPinId = toPinId;
                GetActiveLinks().push_back(l);
            }
        }
    }
}

