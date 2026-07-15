#include "NodeEditor.h"
#include "Core/Application.h"
#include "Core/NodeFactory.h"
#include <cmath>
#include <cfloat>
#include <algorithm>

static ImVec2 BezierCubicCalc(const ImVec2& p1, const ImVec2& p2,
                               const ImVec2& p3, const ImVec2& p4, float t) {
    float u = 1.0f - t;
    return ImVec2(
        u*u*u*p1.x + 3*u*u*t*p2.x + 3*u*t*t*p3.x + t*t*t*p4.x,
        u*u*u*p1.y + 3*u*u*t*p2.y + 3*u*t*t*p3.y + t*t*t*p4.y
    );
}

static float DistPointToLine(ImVec2 pt, ImVec2 a, ImVec2 b) {
    ImVec2 ab(b.x - a.x, b.y - a.y);
    ImVec2 ap(pt.x - a.x, pt.y - a.y);
    float ab2 = ab.x*ab.x + ab.y*ab.y;
    if (ab2 < 0.0001f) return sqrtf(ap.x*ap.x + ap.y*ap.y);
    float t = (ap.x*ab.x + ap.y*ab.y) / ab2;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    ImVec2 closest(a.x + ab.x*t, a.y + ab.y*t);
    float dx = pt.x - closest.x, dy = pt.y - closest.y;
    return sqrtf(dx*dx + dy*dy);
}

// ─── Colors (from theme) ───

static ImU32 U32FromVec4(const ImVec4& v) {
    return IM_COL32((int)(v.x*255), (int)(v.y*255), (int)(v.z*255), (int)(v.w*255));
}

static ImU32 NodeHeaderColor(const char* typeName) {
    auto* entry = NodeFactory::Global().GetEntry(typeName);
    if (!entry) return U32FromVec4(Application::GetConfig().headerScript);
    return NodeFactory::Global().GetCategoryColor(entry->cat.c_str());
}

static ImU32 PinTypeColor(PinType type) {
    auto& t = Application::GetConfig();
    switch (type) {
        case PinType::Container: return U32FromVec4(t.pinContainer);
        case PinType::Theme:     return U32FromVec4(t.pinTheme);
        case PinType::Widget:    return U32FromVec4(t.pinWidget);
        case PinType::Animation: return IM_COL32(255, 180, 50, 220);
        default:                 return IM_COL32(150, 150, 150, 255);
    }
}

// ─── Grid ───

void NodeEditor::DrawGrid(ImDrawList* dl, const ImRect& rect) {
    auto& t = Application::GetConfig();
    const float gridSize = 50.0f;
    const ImU32 majorCol = ImGui::ColorConvertFloat4ToU32(t.gridMajor);
    const ImU32 minorCol = ImGui::ColorConvertFloat4ToU32(t.gridFine);

    ImVec2 tl = ScreenToCanvas(rect.Min);
    ImVec2 br = ScreenToCanvas(rect.Max);

    float sx = floorf(tl.x / gridSize) * gridSize;
    float sy = floorf(tl.y / gridSize) * gridSize;
    for (float x = sx; x < br.x; x += gridSize) {
        dl->AddLine(CanvasToScreen(ImVec2(x, tl.y)),
                     CanvasToScreen(ImVec2(x, br.y)), minorCol);
    }
    for (float y = sy; y < br.y; y += gridSize) {
        dl->AddLine(CanvasToScreen(ImVec2(tl.x, y)),
                     CanvasToScreen(ImVec2(br.x, y)), minorCol);
    }

    float majorSize = 200.0f;
    sx = floorf(tl.x / majorSize) * majorSize;
    sy = floorf(tl.y / majorSize) * majorSize;
    for (float x = sx; x < br.x; x += majorSize) {
        dl->AddLine(CanvasToScreen(ImVec2(x, tl.y)),
                     CanvasToScreen(ImVec2(x, br.y)), majorCol, 1.0f);
    }
    for (float y = sy; y < br.y; y += majorSize) {
        dl->AddLine(CanvasToScreen(ImVec2(tl.x, y)),
                     CanvasToScreen(ImVec2(br.x, y)), majorCol, 1.0f);
    }
}

// ─── Node rendering ───

void NodeEditor::DrawNode(Node* node, ImDrawList* dl) {
    float pinCount = std::max((float)node->m_Inputs.size(), (float)node->m_Outputs.size());
    node->m_Size.y = NODE_TITLE_HEIGHT + NODE_PADDING * 2 + pinCount * NODE_PIN_SPACING;

    ImVec2 pos = CanvasToScreen(node->m_Pos);
    ImVec2 size = node->m_Size * m_Zoom;

    auto& t = Application::GetConfig();
    float z = m_Zoom;
    float r = 6 * z;
    float softRim = 10 * z;

    // Shadow (3 layers, very soft)
    dl->AddRectFilled(pos + ImVec2(10, 10)*z, pos + size + ImVec2(10, 10)*z,
                      IM_COL32(0, 0, 0, 18), softRim);
    dl->AddRectFilled(pos + ImVec2(6, 6)*z, pos + size + ImVec2(6, 6)*z,
                      IM_COL32(0, 0, 0, 25), r + 2*z);
    dl->AddRectFilled(pos + ImVec2(3, 3)*z, pos + size + ImVec2(3, 3)*z,
                      IM_COL32(0, 0, 0, 35), r);

    // Animated selection glow
    if (node->m_Selected) {
        auto it = m_NodeSelectTime.find(node->m_Id);
        float anim = 1.0f;
        if (it != m_NodeSelectTime.end()) {
            float dt = (float)(ImGui::GetTime() - it->second);
            anim = std::min(1.0f, dt / 0.25f);
            anim = anim * anim * (3.0f - 2.0f * anim); // smoothstep
        }
        float glowAlpha = 18 * anim;
        float innerAlpha = 35 * anim;
        float borderAlpha = 0.25f * anim;

        float glowSpread = 10 * z;
        ImU32 selCol = ImGui::ColorConvertFloat4ToU32(t.selection);
        dl->AddRectFilled(pos + ImVec2(-glowSpread, -glowSpread),
            pos + size + ImVec2(glowSpread, glowSpread),
            (selCol & 0x00FFFFFF) | ((int)glowAlpha << 24), r + glowSpread);
        dl->AddRectFilled(pos + ImVec2(-glowSpread*0.5f, -glowSpread*0.5f),
            pos + size + ImVec2(glowSpread*0.5f, glowSpread*0.5f),
            (selCol & 0x00FFFFFF) | ((int)innerAlpha << 24), r + glowSpread*0.4f);
        dl->AddRect(pos + ImVec2(-1.5f, -1.5f)*z, pos + size + ImVec2(1.5f, 1.5f)*z,
            ImColor(t.selection.x, t.selection.y, t.selection.z, borderAlpha), r, 0, 2.0f * z);
    }

    // Drop shadow
    {
        float shadowOff = 4.0f * m_Zoom;
        float shadowBlur = 8.0f * m_Zoom;
        ImU32 shadowCol = IM_COL32(0, 0, 0, (int)(55 * (m_Zoom / 1.5f)));
        dl->AddRectFilled(pos + ImVec2(shadowOff, shadowOff),
            pos + size + ImVec2(shadowOff + shadowBlur, shadowOff + shadowBlur),
            (shadowCol & 0x00FFFFFF) | ((int)((shadowCol >> 24) * 0.5f) << 24), r);
        dl->AddRectFilled(pos + ImVec2(shadowOff, shadowOff),
            pos + size + ImVec2(shadowOff, shadowOff),
            shadowCol, r);
    }

    // Body gradient (lighter top → darker bottom)
    ImVec4 bc = t.nodeBody;
    ImU32 bodyTop = IM_COL32(
        std::min(255, (int)(bc.x * 255 * 1.35f)),
        std::min(255, (int)(bc.y * 255 * 1.35f)),
        std::min(255, (int)(bc.z * 255 * 1.35f)),
        245);
    ImVec4 bot = ImVec4(bc.x * 0.85f, bc.y * 0.85f, bc.z * 0.85f, bc.w);
    ImU32 bodyBot = U32FromVec4(bot);
    dl->AddRectFilledMultiColor(pos, pos + size, bodyTop, bodyTop, bodyBot, bodyBot);

    // Border
    ImU32 borderCol = node->m_Selected ? U32FromVec4(t.nodeBorderSel) : U32FromVec4(t.nodeBorder);
    float borderThick = node->m_Selected ? 2.5f * m_Zoom : 1.5f * m_Zoom;
    dl->AddRect(pos, pos + size, borderCol, 6 * m_Zoom, 0, borderThick);

    // Title bar
    ImVec2 titleMax(pos.x + size.x, pos.y + NODE_TITLE_HEIGHT * m_Zoom);
    ImU32 titleCol = NodeHeaderColor(node->GetTypeName());
    dl->AddRectFilled(pos, titleMax, titleCol, 6 * m_Zoom);
    dl->AddRectFilled(
        ImVec2(pos.x, pos.y + 6 * m_Zoom),
        titleMax, titleCol, 0);

    // Title text (or rename input)
    if (m_RenameNodeId == node->m_Id) {
        // Draw rename input in screen space
        ImGui::SetCursorScreenPos(pos + ImVec2(8 * m_Zoom, 5 * m_Zoom));
        ImGui::PushItemWidth(size.x - 16 * m_Zoom);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::SetKeyboardFocusHere(0);
        if (ImGui::InputText("##rename", m_RenameBuffer, sizeof(m_RenameBuffer),
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
            node->m_Name = m_RenameBuffer;
            m_RenameNodeId = -1;
        }
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();
        if (!ImGui::IsItemActive() && !ImGui::IsItemActivated())
            m_RenameNodeId = -1;
    } else if (m_Zoom > 0.25f) {
        dl->PushClipRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), true);
        dl->AddText(pos + ImVec2(8 * m_Zoom, 5 * m_Zoom),
                    IM_COL32(255, 255, 255, 255),
                    node->m_Name.c_str());
        dl->PopClipRect();
    }

    // Pin compatibility checker for drag validation
    auto isPinCompatible = [&](const Pin& pin) -> bool {
        if (m_DraggingFromPin < 0) return true;
        Pin* fromP = GetPinById(m_DraggingFromPin);
        return fromP && fromP->kind != pin.kind && fromP->type == pin.type;
    };

    auto drawPin = [&](const ImVec2& pinPos, const Pin& pin) {
        bool hov = (m_HoveredPinId == pin.id);
        bool dragging = (m_DraggingFromPin >= 0);
        bool compat = isPinCompatible(pin);
        ImU32 pinColor = PinTypeColor(pin.type);
        float baseR = 5.0f * m_Zoom;
        if (hov && dragging) {
            float glowR = baseR * 3.5f;
            ImU32 glowCol = compat ? IM_COL32(80, 220, 80, 50) : IM_COL32(220, 80, 80, 50);
            dl->AddCircleFilled(pinPos, glowR, glowCol);
            baseR = compat ? 7.0f : 5.5f;
            ImU32 hovCol = compat ? IM_COL32(80, 255, 80, 255) : IM_COL32(255, 80, 80, 200);
            dl->AddCircleFilled(pinPos, baseR * m_Zoom, hovCol);
            dl->AddCircle(pinPos, baseR * m_Zoom + 1, IM_COL32(255, 255, 255, 180), 0, 2.0f * m_Zoom);
        } else if (hov) {
            float glowR = baseR * 3.0f;
            ImU32 glowCol = (pinColor & 0x00FFFFFF) | (55 << 24);
            dl->AddCircleFilled(pinPos, glowR, glowCol);
            float midR = baseR * 1.8f;
            ImU32 midCol = (pinColor & 0x00FFFFFF) | (80 << 24);
            dl->AddCircleFilled(pinPos, midR, midCol);
            baseR = 6.5f * m_Zoom;
            ImU32 brightCol = IM_COL32(
                std::min(255, (int)((pinColor & 0xFF) + 80)),
                std::min(255, (int)(((pinColor >> 8) & 0xFF) + 80)),
                std::min(255, (int)(((pinColor >> 16) & 0xFF) + 80)), 255);
            dl->AddCircleFilled(pinPos, baseR, brightCol);
            dl->AddCircle(pinPos, baseR + 1, IM_COL32(255, 255, 255, 220), 0, 2.5f * m_Zoom);
        } else if (dragging && !compat) {
            dl->AddCircleFilled(pinPos, baseR, IM_COL32(80, 80, 80, 100));
        } else {
            dl->AddCircleFilled(pinPos, baseR, pinColor);
            dl->AddCircle(pinPos, baseR + 0.5f, IM_COL32(255, 255, 255, 80), 0, 1.5f * m_Zoom);
        }
    };

    bool showLabels = (m_Zoom > 0.4f);

    // Input pins
    float pinY = NODE_TITLE_HEIGHT + NODE_PADDING;
    for (size_t i = 0; i < node->m_Inputs.size(); i++) {
        auto& pin = node->m_Inputs[i];
        ImVec2 p(pos.x, pos.y + pinY * m_Zoom);
        drawPin(p, pin);
        if (showLabels)
            dl->AddText(pos + ImVec2(16 * m_Zoom, pinY * m_Zoom - 4 * m_Zoom),
                        ImGui::ColorConvertFloat4ToU32(Application::GetConfig().textDisabled),
                        pin.name.c_str());
        pinY += NODE_PIN_SPACING;
    }

    // Output pins
    pinY = NODE_TITLE_HEIGHT + NODE_PADDING;
    for (size_t i = 0; i < node->m_Outputs.size(); i++) {
        auto& pin = node->m_Outputs[i];
        ImVec2 p(pos.x + size.x, pos.y + pinY * m_Zoom);
        drawPin(p, pin);
        if (showLabels) {
            float tw = ImGui::CalcTextSize(pin.name.c_str()).x;
            dl->AddText(pos + ImVec2(size.x - tw - 16 * m_Zoom, pinY * m_Zoom - 4 * m_Zoom),
                        ImGui::ColorConvertFloat4ToU32(Application::GetConfig().textDisabled),
                        pin.name.c_str());
        }
        pinY += NODE_PIN_SPACING;
    }
}

// ─── Links ───

void NodeEditor::DrawLinks(ImDrawList* dl) {
    Node* dragNode = (m_DraggedNodeId >= 0) ? GetNodeById(m_DraggedNodeId) : nullptr;
    bool canDropOnLink = false;
    float time = (float)ImGui::GetTime();

    for (auto& link : GetActiveLinks()) {
        Node* fn = GetNodeByPinId(link.fromPinId);
        Node* tn = GetNodeByPinId(link.toPinId);
        Pin* fp = GetPinById(link.fromPinId);
        Pin* tp = GetPinById(link.toPinId);
        if (!fn || !tn || !fp || !tp) continue;

        // Redirect links to collapsed region title bar
        auto regionPort = [&](Node* n, Pin* p) -> ImVec2 {
            for (auto& reg : m_Regions) {
                if (!reg.collapsed) continue;
                ImRect rr(reg.pos, reg.pos + reg.size);
                if (rr.Contains(n->m_Pos)) {
                    ImVec2 rs = CanvasToScreen(reg.pos);
                    float h = 24 * m_Zoom;
                    return ImVec2(rs.x + 90 * m_Zoom, rs.y + h * 0.5f);
                }
            }
            return GetPinScreenPos(*p, *n);
        };

        ImVec2 a = regionPort(fn, fp);
        ImVec2 b = regionPort(tn, tp);
        ImVec2 ca = a + ImVec2(50 * m_Zoom, 0);
        ImVec2 cb = b - ImVec2(50 * m_Zoom, 0);

        bool sel = (link.id == m_SelectedLinkId);
        bool hov = (link.id == m_HoveredLinkId);

        bool canConnect = false;
        if (dragNode && hov) {
            for (auto& ip : dragNode->m_Inputs)
                if (ip.type == fp->type)
                    for (auto& op : dragNode->m_Outputs)
                        if (op.type == tp->type) { canConnect = true; break; }
        }

        ImU32 col;
        float thick;
        if (sel) {
            col = ImGui::ColorConvertFloat4ToU32(Application::GetConfig().selection);
            thick = 4.0f * m_Zoom;
        } else if (canConnect) {
            col = U32FromVec4(Application::GetConfig().linkValid);
            thick = 4.0f * m_Zoom;
        } else if (hov) {
            col = U32FromVec4(Application::GetConfig().linkValid);
            thick = 4.0f * m_Zoom;
        } else {
            col = U32FromVec4(Application::GetConfig().linkDefault);
            thick = 3.0f * m_Zoom;
        }

        // Draw base line
        dl->AddBezierCubic(a, ca, cb, b, col, thick, 24);

        // Animated dash for selected/not selected links
        if (!sel && !canConnect) {
            float dashLen = 12.0f * m_Zoom;
            float gapLen = 8.0f * m_Zoom;
            float totalLen = dashLen + gapLen;
            float offset = fmodf(time * 60.0f * m_Zoom, totalLen);
            // Draw dashed overlay with slightly brighter color
            ImU32 dashCol = (col & 0x00FFFFFF) | (80 << 24);
            // Approximate dash: draw segments along the curve
            float linkLen = ImSqrt(ImLengthSqr(a - b));
            const int segs = std::max(4, (int)((linkLen / (dashLen + gapLen)) * 2));
            float denom = linkLen + 1.0f;
            for (int i = 0; i < segs; i++) {
                float t0 = (i * totalLen + offset) / denom;
                float t1 = (i * totalLen + offset + dashLen) / denom;
                if (t0 > 1.0f) break;
                if (t1 > 1.0f) t1 = 1.0f;
                ImVec2 p0 = ImBezierCubicCalc(a, ca, cb, b, t0);
                ImVec2 p1 = ImBezierCubicCalc(a, ca, cb, b, t1);
                dl->AddLine(p0, p1, dashCol, thick * 0.6f);
            }
        }

        if (canConnect) canDropOnLink = true;
    }

    if (canDropOnLink) {
        ImVec2 m = ImGui::GetIO().MousePos;
        dl->AddText(m + ImVec2(12, -8), IM_COL32(100, 255, 100, 220), "Snap to link");
    }
}

void NodeEditor::DrawDragLink(ImDrawList* dl) {
    if (m_DraggingFromPin < 0) return;
    Node* n = GetNodeByPinId(m_DraggingFromPin);
    Pin* p = GetPinById(m_DraggingFromPin);
    if (!n || !p) return;

    ImVec2 a = GetPinScreenPos(*p, *n);
    ImVec2 b = ImGui::GetIO().MousePos;
    float snapDist = 18.0f;

    // Snap to nearest compatible pin
    int snapPinId = -1;
    int hoverPin = HitTestPin(b);
    float minDist = snapDist;
    if (hoverPin >= 0 && hoverPin != m_DraggingFromPin) {
        Pin* hp = GetPinById(hoverPin);
        if (hp && p->kind != hp->kind && p->type == hp->type) {
            snapPinId = hoverPin;
            minDist = 0;
        }
    }
    // If no pin under mouse, check nearby pins
    if (snapPinId < 0) {
        for (auto& node : GetActiveNodes()) {
            for (auto& pin : node->m_Inputs) {
                if (pin.id == m_DraggingFromPin || pin.kind == p->kind) continue;
                if (pin.type != p->type) continue;
                ImVec2 pos = GetPinScreenPos(pin, *node);
                float d = ImSqrt(ImLengthSqr(pos - b));
                if (d < minDist) {
                    minDist = d;
                    snapPinId = pin.id;
                }
            }
            for (auto& pin : node->m_Outputs) {
                if (pin.id == m_DraggingFromPin || pin.kind == p->kind) continue;
                if (pin.type != p->type) continue;
                ImVec2 pos = GetPinScreenPos(pin, *node);
                float d = ImSqrt(ImLengthSqr(pos - b));
                if (d < minDist) {
                    minDist = d;
                    snapPinId = pin.id;
                }
            }
        }
    }

    if (snapPinId >= 0) {
        Pin* snapP = GetPinById(snapPinId);
        Node* snapN = snapP ? GetNodeByPinId(snapPinId) : nullptr;
        if (snapP && snapN)
            b = GetPinScreenPos(*snapP, *snapN);
    }

    ImVec2 ca = a + ImVec2(50 * m_Zoom, 0);
    ImVec2 cb = b - ImVec2(50 * m_Zoom, 0);

    float time = (float)ImGui::GetTime();
    float pulse = sinf(time * 4.0f) * 0.15f + 1.0f;

    ImU32 col;
    float thick;
    if (snapPinId >= 0) {
        col = U32FromVec4(Application::GetConfig().linkValid);
        thick = 3.0f * m_Zoom * pulse;
    } else if (hoverPin >= 0) {
        Pin* hp = GetPinById(hoverPin);
        if (hp && p->kind != hp->kind && p->type == hp->type) {
            col = U32FromVec4(Application::GetConfig().linkValid);
            thick = 3.0f * m_Zoom * pulse;
        } else {
            col = U32FromVec4(Application::GetConfig().linkInvalid);
            thick = 2.0f * m_Zoom;
        }
    } else {
        col = U32FromVec4(Application::GetConfig().linkDefault);
        thick = 2.0f * m_Zoom;
    }

    // Draw animated glow
    if (snapPinId >= 0) {
        ImU32 glowCol = (col & 0x00FFFFFF) | (40 << 24);
        dl->AddBezierCubic(a, ca, cb, b, glowCol, thick * 2.5f, 20);
    }
    dl->AddBezierCubic(a, ca, cb, b, col, thick, 20);
}

ImVec2 NodeEditor::GetPinScreenPos(const Pin& pin, const Node& node) const {
    float py = NODE_TITLE_HEIGHT + NODE_PADDING;
    if (pin.kind == PinKind::Input) {
        for (size_t i = 0; i < node.m_Inputs.size(); i++)
            if (node.m_Inputs[i].id == pin.id) { py += i * NODE_PIN_SPACING; break; }
        return CanvasToScreen(node.m_Pos + ImVec2(0, py));
    } else {
        for (size_t i = 0; i < node.m_Outputs.size(); i++)
            if (node.m_Outputs[i].id == pin.id) { py += i * NODE_PIN_SPACING; break; }
        return CanvasToScreen(node.m_Pos + ImVec2(node.m_Size.x, py));
    }
}

// ─── Link hit test ───

int NodeEditor::HitTestLink(ImVec2 screenPos) const {
    for (auto& link : GetActiveLinks()) {
        Node* fn = GetNodeByPinId(link.fromPinId);
        Node* tn = GetNodeByPinId(link.toPinId);
        Pin* fp = GetPinById(link.fromPinId);
        Pin* tp = GetPinById(link.toPinId);
        if (!fn || !tn || !fp || !tp) continue;

        ImVec2 a = GetPinScreenPos(*fp, *fn);
        ImVec2 b = GetPinScreenPos(*tp, *tn);
        ImVec2 ca = a + ImVec2(50 * m_Zoom, 0);
        ImVec2 cb = b - ImVec2(50 * m_Zoom, 0);

        const int N = 20;
        float minD = FLT_MAX;
        ImVec2 prev = a;
        for (int i = 1; i <= N; i++) {
            float t = i / (float)N;
            ImVec2 p = BezierCubicCalc(a, ca, cb, b, t);
            float d = DistPointToLine(screenPos, prev, p);
            if (d < minD) minD = d;
            prev = p;
        }
        if (minD < 8.0f) return link.id;
    }
    return -1;
}

