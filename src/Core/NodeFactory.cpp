#include "Core/NodeFactory.h"
#include "Core/Application.h"
#include "NodeEditor/Node.h"

#include "containers/WindowNode.h"
#include "containers/RowNode.h"
#include "containers/ColumnNode.h"
#include "widgets/GridNode.h"
#include "widgets/MenuBarNode.h"
#include "widgets/TabBarNode.h"
#include "widgets/TabItemNode.h"
#include "widgets/SubMenuNode.h"
#include "widgets/ChildNode.h"
#include "widgets/PopupNode.h"
#include "widgets/TreeNode.h"
#include "widgets/SplitterNode.h"
#include "output/PreviewNode.h"
#include "output/NodePreviewNode.h"
#include "style/ThemeNode.h"
#include "widgets/AnimationNode.h"
#include "widgets/ButtonNode.h"
#include "widgets/TextNode.h"
#include "widgets/CheckboxNode.h"
#include "widgets/SliderNode.h"
#include "widgets/SeparatorNode.h"
#include "widgets/SpacerNode.h"
#include "widgets/InputTextNode.h"
#include "widgets/RadioButtonNode.h"
#include "widgets/ComboBoxNode.h"
#include "widgets/SameLineNode.h"
#include "widgets/MenuItemNode.h"
#include "widgets/InputIntNode.h"
#include "widgets/ColorEditNode.h"
#include "widgets/SelectableNode.h"
#include "widgets/ProgressBarNode.h"
#include "widgets/ToggleSwitchNode.h"
#include "widgets/SegmentedNode.h"
#include "widgets/BadgeNode.h"
#include "widgets/TooltipNode.h"
#include "widgets/DragDropNode.h"
#include "widgets/ComponentNode.h"
#include "widgets/ComponentOutputNode.h"

void NodeFactory::RegisterAll() {
    // ── Containers ──
    Register<WindowNode>("Window",       "Root window container",                  "Containers");
    Register<RowNode>("Row",             "Horizontal layout container",            "Containers");
    Register<ColumnNode>("Column",       "Vertical layout container",              "Containers");
    Register<GridNode>("Grid",           "Multi-column grid layout (BeginTable)",  "Containers");
    Register<TabBarNode>("TabBar",       "Tab strip container",                    "Containers");
    Register<TabItemNode>("TabItem",     "Individual tab (inside TabBar)",          "Containers");
    Register<ChildNode>("Child",         "Nested child window",                    "Containers");
    Register<PopupNode>("Popup",         "Popup/modal window container",            "Containers");
    Register<TreeNode>("TreeNode",       "Collapsible tree node",                  "Containers");
    // ── Layout ──
    Register<SameLineNode>("SameLine",   "Continue on same line (spacing)",        "Layout");
    Register<SeparatorNode>("Separator", "Horizontal divider line",                "Layout");
    Register<SpacerNode>("Spacer",       "Empty spacing filler",                   "Layout");
    Register<SplitterNode>("Splitter",   "Draggable split between two panes",      "Layout");
    // ── Menus ──
    Register<MenuBarNode>("MenuBar",     "Top menu bar container",                 "Menus");
    Register<SubMenuNode>("SubMenu",     "Sub-menu (inside MenuBar)",               "Menus");
    Register<MenuItemNode>("MenuItem",   "Menu bar clickable item",                "Menus");
    // ── Input ──
    Register<CheckboxNode>("Checkbox",   "Toggle checkbox",                        "Input");
    Register<SliderNode>("Slider",       "Adjustable float slider",                "Input");
    Register<InputTextNode>("InputText", "Single-line text input field",            "Input");
    Register<InputIntNode>("InputInt",   "Integer number input",                    "Input");
    Register<RadioButtonNode>("RadioButton","Radio button selection",              "Input");
    Register<ComboBoxNode>("ComboBox",   "Dropdown combo box",                     "Input");
    Register<ColorEditNode>("ColorEdit", "Color picker widget",                     "Input");
    Register<ToggleSwitchNode>("ToggleSwitch","Toggle switch with knob",           "Input");
    Register<DragDropNode>("DragDrop",   "Drag source / drop target",               "Input");
    // ── Widgets ──
    Register<ButtonNode>("Button",       "Clickable button widget",                "Widgets");
    Register<TextNode>("Text",           "Static text label",                      "Widgets");
    Register<SelectableNode>("Selectable","Clickable selectable item",              "Widgets");
    Register<ProgressBarNode>("ProgressBar","Progress bar indicator",              "Widgets");
    Register<SegmentedNode>("Segmented", "Segmented control (tabs style)",          "Widgets");
    Register<BadgeNode>("Badge",         "Notification badge indicator",            "Widgets");
    Register<TooltipNode>("Tooltip",     "Tooltip on hover (with text)",            "Widgets");
    // ── Output ──
    Register<PreviewNode>("Preview",     "Live preview of connected Window",       "Output");
    Register<NodePreviewNode>("NodePreview","Preview any node directly",           "Output");
    // ── Style ──
    Register<ThemeNode>("Theme",         "Color & style configuration",            "Style");
    // ── Animation ──
    Register<AnimationNode>("Animation", "Animation curve and timing",              "Animation");
    // ── Components ──
    Register<ComponentNode>("Component", "Reusable sub-graph component (double-click to edit)", "Components");
    Register<ComponentOutputNode>("ComponentOutput", "Marks what renders as this Component's output", "Components");
}

ImU32 NodeFactory::GetCategoryColor(const char* cat) const {
    auto& c = Application::GetConfig();
    if (strcmp(cat, "Containers") == 0) return ImGui::ColorConvertFloat4ToU32(c.headerContainer);
    if (strcmp(cat, "Input") == 0)      return ImGui::ColorConvertFloat4ToU32(c.headerInput);
    if (strcmp(cat, "Widgets") == 0)    return ImGui::ColorConvertFloat4ToU32(c.headerOutput);
    if (strcmp(cat, "Menus") == 0)      return ImGui::ColorConvertFloat4ToU32(c.headerConverter);
    if (strcmp(cat, "Output") == 0)     return ImGui::ColorConvertFloat4ToU32(c.headerShader);
    if (strcmp(cat, "Layout") == 0)     return ImGui::ColorConvertFloat4ToU32(c.headerScript);
    if (strcmp(cat, "Style") == 0)      return ImGui::ColorConvertFloat4ToU32(c.headerTexture);
    if (strcmp(cat, "Animation") == 0)  return ImGui::ColorConvertFloat4ToU32(c.headerTexture);
    if (strcmp(cat, "Components") == 0) return ImGui::ColorConvertFloat4ToU32(c.headerVector);
    return ImGui::ColorConvertFloat4ToU32(c.headerScript);
}

std::unique_ptr<Node> NodeFactory::Create(const char* type, int id) const {
    for (auto& e : m_Entries)
        if (e.name == type)
            return e.createFn(id);
    return nullptr;
}

const NodeEntry* NodeFactory::GetEntry(const char* type) const {
    for (auto& e : m_Entries)
        if (e.name == type)
            return &e;
    return nullptr;
}

NodeFactory& NodeFactory::Global() {
    static NodeFactory factory;
    static bool registered = false;
    if (!registered) { factory.RegisterAll(); registered = true; }
    return factory;
}

std::unique_ptr<Node> CreateNode(const char* type, int id) {
    return NodeFactory::Global().Create(type, id);
}
