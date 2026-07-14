# ImGui Node Editor

A visual node-based editor for building ImGui interfaces. Create UI layouts, widgets, animations, and interactive components by connecting nodes in a graph editor — no coding required.

## Features

- **Visual node graph** — drag, connect, and arrange nodes on an infinite canvas with zoom/pan
- **Live preview** — see your UI rendered in real-time as you build it
- **40+ node types** — containers (Window, Row, Column, Grid, TabBar), widgets (Button, Slider, Checkbox, Text, ComboBox, and more), layout helpers, menus, inputs, and output previews
- **Animation system** — Animate any widget with easing curves (13 types including bounce, elastic, and custom Catmull-Rom splines)
- **Custom styling** — ThemeNode for global color schemes, per-node custom style overrides
- **Components** — Reusable sub-graphs with their own canvas (double-click to enter/exit)
- **Undo/Redo** — Full undo/redo stack for all graph edits
- **Copy/Paste** — Copy selected nodes and their internal connections
- **Search & place** — Press Space to search and place nodes
- **Regions** — Color-coded grouping areas for organizing nodes
- **Auto-arrange** — Automatically layout selected nodes
- **File save/load** — Save your projects as `.ng` files

## Getting Started

### Prerequisites

- CMake 3.20+
- C++17 compiler (MSVC, GCC/MinGW, Clang)
- OpenGL 3.3+

### Building

```bash
git clone --recursive https://github.com/YOUR_USER/ImGuiNodeEditor.git
cd ImGuiNodeEditor
cmake -B build
cmake --build build
```

The project vendors its dependencies (GLFW, ImGui, GLAD) in the `ext/` directory. The `--recursive` flag ensures they are fetched automatically.

### Running

```bash
./build/ImGuiNodeEditor
```

Assets (fonts) are loaded from the `assets/` directory at runtime relative to the executable.

## Project Structure

```
├── assets/              # Fonts (Inter, Font Awesome)
├── ext/                 # Vendored dependencies (GLFW, ImGui, GLAD)
├── src/
│   ├── main.cpp         # Entry point
│   ├── Core/            # Application, config, preview rendering, node factory
│   ├── NodeEditor/      # Graph editor canvas, interaction, serialization
│   └── nodes/           # All node type implementations
│       ├── containers/  # Window, Row, Column, Grid, TabBar, Child, Popup, TreeNode
│       ├── widgets/     # Button, Text, Slider, Checkbox, ComboBox, etc.
│       ├── output/      # Preview, NodePreview
│       └── style/       # Theme
├── CMakeLists.txt
```

### Architecture Overview

- **Application** — Owns the GLFW window, ImGui context, main menu bar, dockspace, properties panel, and live preview renderer
- **NodeEditor** — The graph editor canvas. Handles input (drag, zoom, select), manages nodes/links, undo/redo, clipboard, serialization, and search
- **Node** — Base class for all node types. Each node defines its pins, properties, and `RenderPreview()` for live UI rendering
- **NodeFactory** — Central registry for all node types. Adding a new node type requires only implementing the node class and registering it in `NodeFactory::RegisterAll()`

## Adding a New Node Type

1. Create `src/nodes/widgets/MyNode.h` and `.cpp`
2. In `MyNode` constructor, define input/output pins
3. Implement `Draw()` (empty for most nodes), `RenderPreview()`, `SaveExtra()`, `LoadExtra()`, `DrawProperties()`
4. Register in `src/Core/NodeFactory.cpp` with `Register<MyNode>("MyNode", "Description", "Category")`

That's it — the search, palette, colors, clipboard, and serialization all work automatically.

## License

Custom license — see [LICENSE](LICENSE). Free for non-commercial use. Commercial use requires permission from the author.
