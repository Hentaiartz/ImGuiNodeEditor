cmake_minimum_required(VERSION 3.20)
project(ImGuiNodeEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /utf-8)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

# ─── GLFW (ya descargado en ext/glfw) ───
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/glfw ${CMAKE_BINARY_DIR}/glfw EXCLUDE_FROM_ALL)

# ─── imgui (ya descargado en ext/imgui) ───
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/ext/imgui)
add_library(imgui STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui PUBLIC
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imgui PUBLIC glfw)
if(WIN32)
    target_link_libraries(imgui PUBLIC opengl32)
endif()

# ─── Main executable ───
set(SOURCES
    src/main.cpp
    src/Core/Application.cpp
    src/Core/PreviewHelpers.cpp
    src/Core/PreviewRenderer.cpp
    src/Core/NodeFactory.cpp
    src/NodeEditor/Node.cpp
    src/NodeEditor/NodeEditor.cpp
    src/NodeEditor/NodeEditorCanvas.cpp
    src/NodeEditor/NodeEditorActions.cpp


    src/nodes/containers/ContainerNode.cpp
    src/nodes/containers/WindowNode.cpp
    src/nodes/containers/RowNode.cpp
    src/nodes/containers/ColumnNode.cpp
    src/nodes/output/PreviewNode.cpp
    src/nodes/output/NodePreviewNode.cpp
    src/nodes/style/ThemeNode.cpp
    src/nodes/widgets/ButtonNode.cpp
    src/nodes/widgets/TextNode.cpp
    src/nodes/widgets/CheckboxNode.cpp
    src/nodes/widgets/SliderNode.cpp
    src/nodes/widgets/SeparatorNode.cpp
    src/nodes/widgets/SpacerNode.cpp
    src/nodes/widgets/InputTextNode.cpp
    src/nodes/widgets/RadioButtonNode.cpp
    src/nodes/widgets/ComboBoxNode.cpp
    src/nodes/widgets/SameLineNode.cpp
    src/nodes/widgets/MenuItemNode.cpp
    src/nodes/widgets/MenuBarNode.cpp
    src/nodes/widgets/TabBarNode.cpp
    src/nodes/widgets/TabItemNode.cpp
    src/nodes/widgets/ChildNode.cpp
    src/nodes/widgets/SubMenuNode.cpp
    src/nodes/widgets/AnimationNode.cpp
    src/nodes/widgets/InputIntNode.cpp
    src/nodes/widgets/ColorEditNode.cpp
    src/nodes/widgets/SelectableNode.cpp
    src/nodes/widgets/ProgressBarNode.cpp
    src/nodes/widgets/ToggleSwitchNode.cpp
    src/nodes/widgets/SegmentedNode.cpp
    src/nodes/widgets/BadgeNode.cpp
    src/nodes/widgets/GridNode.cpp
    src/nodes/widgets/PopupNode.cpp
    src/nodes/widgets/TooltipNode.cpp
    src/nodes/widgets/TreeNode.cpp
    src/nodes/widgets/SplitterNode.cpp
    src/nodes/widgets/DragDropNode.cpp
    src/nodes/widgets/ComponentNode.cpp
    src/nodes/widgets/ComponentOutputNode.cpp
)

set(HEADERS
    src/Core/Application.h
    src/Core/PreviewHelpers.h
    src/Core/NodeFactory.h
    src/NodeEditor/Node.h
    src/NodeEditor/NodeEditor.h


    src/nodes/containers/ContainerNode.h
    src/nodes/containers/WindowNode.h
    src/nodes/containers/RowNode.h
    src/nodes/containers/ColumnNode.h
    src/nodes/widgets/MenuBarNode.h
    src/nodes/widgets/TabBarNode.h
    src/nodes/widgets/TabItemNode.h
    src/nodes/widgets/ChildNode.h
    src/nodes/widgets/SubMenuNode.h
    src/nodes/output/PreviewNode.h
    src/nodes/output/NodePreviewNode.h
    src/nodes/style/ThemeNode.h
    src/nodes/widgets/AnimationNode.h
    src/nodes/widgets/InputIntNode.h
    src/nodes/widgets/ColorEditNode.h
    src/nodes/widgets/SelectableNode.h
    src/nodes/widgets/ProgressBarNode.h
    src/nodes/widgets/ToggleSwitchNode.h
    src/nodes/widgets/SegmentedNode.h
    src/nodes/widgets/BadgeNode.h
    src/nodes/widgets/ButtonNode.h
    src/nodes/widgets/TextNode.h
    src/nodes/widgets/CheckboxNode.h
    src/nodes/widgets/SliderNode.h
    src/nodes/widgets/SeparatorNode.h
    src/nodes/widgets/SpacerNode.h
    src/nodes/widgets/InputTextNode.h
    src/nodes/widgets/RadioButtonNode.h
    src/nodes/widgets/ComboBoxNode.h
    src/nodes/widgets/SameLineNode.h
    src/nodes/widgets/MenuItemNode.h
    src/nodes/widgets/GridNode.h
    src/nodes/widgets/PopupNode.h
    src/nodes/widgets/TooltipNode.h
    src/nodes/widgets/TreeNode.h
    src/nodes/widgets/SplitterNode.h
    src/nodes/widgets/DragDropNode.h
    src/nodes/widgets/ComponentNode.h
    src/nodes/widgets/ComponentOutputNode.h
)

add_executable(ImGuiNodeEditor ${SOURCES} ${HEADERS})

target_compile_definitions(ImGuiNodeEditor PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_compile_definitions(ImGuiNodeEditor PRIVATE IMGUI_DEFINE_MATH_OPERATORS)

target_include_directories(ImGuiNodeEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/nodes
    ${CMAKE_SOURCE_DIR}/ext/glad/include
)

target_sources(ImGuiNodeEditor PRIVATE ${CMAKE_SOURCE_DIR}/ext/glad/src/glad.c)
set_source_files_properties(${CMAKE_SOURCE_DIR}/ext/glad/src/glad.c PROPERTIES LANGUAGE CXX)

target_link_libraries(ImGuiNodeEditor PRIVATE imgui glfw)

if(WIN32)
    target_link_libraries(ImGuiNodeEditor PRIVATE opengl32 comdlg32)
    if(MINGW)
        target_link_options(ImGuiNodeEditor PRIVATE -mwindows)
    endif()
endif()
