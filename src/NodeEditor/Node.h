#pragma once
#include <string>
#include <vector>
#include <memory>
#include <imgui.h>

struct RenderContext;

enum class PinType { Container, Theme, Widget, Animation };
enum class PinKind { Input, Output };

struct Pin {
    int id;
    std::string name;
    PinKind kind;
    PinType type;
    int nodeId;
};

struct Link {
    int id;
    int fromPinId;
    int toPinId;
};

class Node {
public:
    Node(int id, const std::string& name);
    virtual ~Node() = default;

    virtual void Draw() = 0;
    virtual const char* GetTypeName() const = 0;
    virtual const char* GetDisplayLabel() const { return m_Name.c_str(); }
    virtual void DrawProperties() {}
    virtual void RenderPreview(const struct RenderContext& ctx) { (void)ctx; }
    // Helper for section headers in properties
    static bool PropSection(const char* label) {
        ImGui::Spacing();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 hPos = ImGui::GetCursorScreenPos();
        float hW = ImGui::GetContentRegionAvail().x;
        float hH = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2 + 4;
        ImVec4 sel = ImGui::GetStyle().Colors[ImGuiCol_Header];
        ImU32 colL = IM_COL32((int)(sel.x*255), (int)(sel.y*255), (int)(sel.z*255), 35);
        ImU32 colR = colL & 0x00FFFFFF;
        dl->AddRectFilledMultiColor(hPos, hPos + ImVec2(hW, hH), colL, colR, colR, colL);
        ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255,255,255,18));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255,255,255,30));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(210, 210, 230, 255));
        bool open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::PopStyleColor(4);
        return open;
    }
    virtual void SaveExtra(struct ImGuiTextBuffer& b) const { (void)b; }
    virtual void LoadExtra(const char* key, const char* line) { (void)key; (void)line; }
    virtual float GetWidgetWidth(const struct RenderContext* ctx = nullptr) const { (void)ctx; return 100; }

    std::vector<Pin*> GetContainerInputs();
    Pin* GetContainerOutput();

protected:
    class NodeEditor* m_Editor = nullptr;
public:
    void SetEditor(class NodeEditor* ed) { m_Editor = ed; }
    class NodeEditor* GetEditor() const { return m_Editor; }

    int m_Id;
    std::string m_Name;
    ImVec2 m_Pos;
    ImVec2 m_Size;
    bool m_Selected = false;
    std::vector<Pin> m_Inputs;
    std::vector<Pin> m_Outputs;
};

static constexpr float NODE_WIDTH = 180.0f;
static constexpr float NODE_TITLE_HEIGHT = 30.0f;
static constexpr float NODE_PIN_SPACING = 22.0f;
static constexpr float NODE_PADDING = 8.0f;
