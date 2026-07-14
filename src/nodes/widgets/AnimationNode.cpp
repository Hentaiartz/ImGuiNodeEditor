#include "AnimationNode.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <math.h>
#include <algorithm>

static const char* kEasingNames[] = {
    "Linear",
    "EaseIn",
    "EaseOut",
    "EaseInOut",
    "EaseInCubic",
    "EaseOutCubic",
    "EaseInOutCubic",
    "EaseInBack",
    "EaseOutBack",
    "EaseInOutBack",
    "Bounce",
    "Elastic",
    "Custom",
};

static float EvalEasing(const char* name, float t, const AnimationNode* anim) {
    if (strcmp(name, "Linear") == 0) return t;
    if (strcmp(name, "EaseIn") == 0) return t * t;
    if (strcmp(name, "EaseOut") == 0) return 1.0f - (1.0f - t) * (1.0f - t);
    if (strcmp(name, "EaseInOut") == 0) return t * t * (3.0f - 2.0f * t);
    if (strcmp(name, "EaseInCubic") == 0) return t * t * t;
    if (strcmp(name, "EaseOutCubic") == 0) { float v = 1.0f - t; return 1.0f - v * v * v; }
    if (strcmp(name, "EaseInOutCubic") == 0) {
        if (t < 0.5f) return 4.0f * t * t * t;
        float v = 1.0f - (2.0f * t - 1.0f);
        return 1.0f - v * v * v * 0.5f;
    }
    if (strcmp(name, "EaseInBack") == 0) {
        float c = 1.70158f;
        return (c + 1.0f) * t * t * t - c * t * t;
    }
    if (strcmp(name, "EaseOutBack") == 0) {
        float c = 1.70158f;
        float v = t - 1.0f;
        return v * v * ((c + 1.0f) * v + c) + 1.0f;
    }
    if (strcmp(name, "EaseInOutBack") == 0) {
        float c = 1.70158f * 1.525f;
        if (t < 0.5f) { float v = 2.0f * t; return 0.5f * (v * v * ((c + 1.0f) * v - c)); }
        float v = 2.0f * t - 2.0f;
        return 0.5f * (v * v * ((c + 1.0f) * v + c) + 2.0f);
    }
    if (strcmp(name, "Bounce") == 0) {
        if (t < 0.5f) return 2.0f * t * t;
        return -1.0f + (4.0f - 2.0f * t) * t;
    }
    if (strcmp(name, "Elastic") == 0) {
        if (t < 0.001f) return 0.0f;
        return (float)(pow(2.0, -10.0 * t) * sin((t - 0.075) * (2.0 * 3.14159) / 0.3) + 1.0);
    }
    if (strcmp(name, "Custom") == 0 && anim) return anim->EvalCustom(t);
    return t;
}

float AnimationNode::ApplyEasing(float t, const std::string& easing, const AnimationNode* animNode) {
    if (animNode)
        return EvalEasing(easing.c_str(), t, animNode);
    return t * t * (3.0f - 2.0f * t); // smoothstep fallback
}

AnimationNode::AnimationNode(int id)
    : Node(id, "Animation")
{
    m_Outputs.push_back({ id * 10000 + 2, "Anim", PinKind::Output, PinType::Animation, id });
    m_Size = ImVec2(NODE_WIDTH, 60.0f);
    // Default custom points: two inner points forming a gentle ease-in-out
    m_CustomPoints.push_back(ImVec2(0.25f, 0.15f));
    m_CustomPoints.push_back(ImVec2(0.75f, 0.85f));
}

void AnimationNode::Draw() {}

void AnimationNode::SaveExtra(ImGuiTextBuffer& b) const {
    b.appendf("animName S \"%s\"\n", m_Name.c_str());
    b.appendf("target S \"%s\"\n", m_Target.c_str());
    b.appendf("duration F %.3f\n", m_Duration);
    b.appendf("easing S \"%s\"\n", m_Easing.c_str());
    for (auto& pt : m_CustomPoints)
        b.appendf("cpt V %.3f %.3f\n", pt.x, pt.y);
}
void AnimationNode::LoadExtra(const char* key, const char* line) {
    if (strcmp(key, "animName") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Name = v; }
    else if (strcmp(key, "target") == 0) { char v[256]; if (sscanf(line, " S \"%255[^\"]\"", v) >= 1) m_Target = v; }
    else if (strcmp(key, "duration") == 0) { sscanf(line, " F %f", &m_Duration); }
    else if (strcmp(key, "easing") == 0) { char v[64]; if (sscanf(line, " S \"%63[^\"]\"", v) >= 1) m_Easing = v; }
    else if (strcmp(key, "cpt") == 0) { ImVec2 pt; if (sscanf(line, " V %f %f", &pt.x, &pt.y) == 2) m_CustomPoints.push_back(pt); }
    // Backward compat: old cp1/cp2 → two inner points
    else if (strcmp(key, "cp1") == 0) { ImVec2 pt; if (sscanf(line, " V %f %f", &pt.x, &pt.y) == 2) { m_CustomPoints.clear(); m_CustomPoints.push_back(pt); } }
    else if (strcmp(key, "cp2") == 0) { ImVec2 pt; if (!m_CustomPoints.empty() && sscanf(line, " V %f %f", &pt.x, &pt.y) == 2) m_CustomPoints.push_back(pt); }
}

void AnimationNode::DrawProperties() {
    if (Node::PropSection("Basic")) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s", m_Name.c_str());
        if (ImGui::InputText("Name", buf, sizeof(buf)))
            m_Name = buf;
        ImGui::DragFloat("Duration (s)", &m_Duration, 0.005f, 0.0f, 100.0f, "%.3f");

        int idx = 0;
        for (int i = 0; i < IM_ARRAYSIZE(kEasingNames); i++)
            if (m_Easing == kEasingNames[i]) { idx = i; break; }
        if (ImGui::Combo("Easing", &idx, kEasingNames, IM_ARRAYSIZE(kEasingNames)))
            m_Easing = kEasingNames[idx];
    }

    // Easing curve preview + custom points editor
    if (Node::PropSection("Curve")) {
        float w = ImGui::GetContentRegionAvail().x;
        float h = 120.0f;
        ImVec2 o = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        dl->AddRectFilled(o, o + ImVec2(w, h), IM_COL32(15, 15, 22, 200), 4);
        dl->AddRect(o, o + ImVec2(w, h), IM_COL32(60, 60, 80, 120), 4);

        for (int i = 1; i < 4; i++) {
            float fx = o.x + w * i / 4.0f;
            float fy = o.y + h * i / 4.0f;
            dl->AddLine(ImVec2(fx, o.y), ImVec2(fx, o.y + h), IM_COL32(60, 60, 80, 40));
            dl->AddLine(ImVec2(o.x, fy), ImVec2(o.x + w, fy), IM_COL32(60, 60, 80, 40));
        }
        dl->AddLine(o, o + ImVec2(w, h), IM_COL32(80, 80, 100, 50));

        bool isCustom = (m_Easing == "Custom");

        // Draw curve
        ImVec2 prev = o;
        for (int i = 0; i <= 64; i++) {
            float t = i / 64.0f;
            float v = 1.0f - EvalEasing(m_Easing.c_str(), t, this);
            ImVec2 p = o + ImVec2(t * w, v * h);
            if (i > 0) dl->AddLine(prev, p, IM_COL32(255, 180, 50, 220), 2.0f);
            prev = p;
        }

        // Custom point editor
        if (isCustom) {
            auto toScreen = [&](ImVec2 p) { return o + ImVec2(p.x * w, (1.0f - p.y) * h); };
            // Build full list with implicit (0,0) and (1,1)
            std::vector<ImVec2> fullPts;
            fullPts.push_back(ImVec2(0, 0));
            fullPts.insert(fullPts.end(), m_CustomPoints.begin(), m_CustomPoints.end());
            fullPts.push_back(ImVec2(1, 1));

            // Draw polyline skeleton
            for (size_t i = 0; i + 1 < fullPts.size(); i++) {
                ImVec2 a = toScreen(fullPts[i]);
                ImVec2 b = toScreen(fullPts[i + 1]);
                dl->AddLine(a, b, IM_COL32(255, 180, 50, 50), 1.0f);
            }

            // Draw handles for inner points (exclude implicit start/end)
            for (size_t i = 1; i + 1 < fullPts.size(); i++) {
                ImVec2 sp = toScreen(fullPts[i]);
                dl->AddCircleFilled(sp, 5.0f, IM_COL32(255, 180, 50, 220));
                dl->AddCircle(sp, 5.0f, IM_COL32(255, 255, 255, 180));
            }

            // Drag interaction
            static int dragIdx = -1;
            ImGui::SetCursorScreenPos(o);
            ImGui::InvisibleButton("##curve", ImVec2(w, h));

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
                ImVec2 mp = (ImGui::GetIO().MousePos - o);
                float mx = mp.x / w; float my = 1.0f - mp.y / h;
                mx = std::max(0.0f, std::min(1.0f, mx));
                my = std::max(0.0f, std::min(1.0f, my));
                // Check if clicking near an existing point
                dragIdx = -1;
                for (size_t i = 1; i + 1 < fullPts.size(); i++) {
                    float d = (mx - fullPts[i].x) * (mx - fullPts[i].x) + (my - fullPts[i].y) * (my - fullPts[i].y);
                    if (d < 0.015f) { dragIdx = (int)i - 1; break; } // -1 because fullPts[0] is (0,0)
                }
                if (dragIdx < 0) {
                    // Add a new point at click position (sorted by x)
                    m_CustomPoints.push_back(ImVec2(mx, my));
                    std::sort(m_CustomPoints.begin(), m_CustomPoints.end(),
                        [](const ImVec2& a, const ImVec2& b) { return a.x < b.x; });
                }
            }
            if (ImGui::IsMouseDragging(0) && dragIdx >= 0 && dragIdx < (int)m_CustomPoints.size()) {
                ImVec2 mp = (ImGui::GetIO().MousePos - o);
                float mx = (mp.x < 0 ? 0 : (mp.x > w ? w : mp.x)) / w;
                float my = 1.0f - (mp.y < 0 ? 0 : (mp.y > h ? h : mp.y)) / h;
                m_CustomPoints[dragIdx] = ImVec2(mx, my);
            }
            if (!ImGui::IsMouseDown(0)) dragIdx = -1;

            // Remove point with right-click
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
                ImVec2 mp = (ImGui::GetIO().MousePos - o);
                float mx = mp.x / w; float my = 1.0f - mp.y / h;
                for (int i = (int)m_CustomPoints.size() - 1; i >= 0; i--) {
                    float d = (mx - m_CustomPoints[i].x) * (mx - m_CustomPoints[i].x) + (my - m_CustomPoints[i].y) * (my - m_CustomPoints[i].y);
                    if (d < 0.02f) { m_CustomPoints.erase(m_CustomPoints.begin() + i); break; }
                }
            }
        }
        ImGui::Dummy(ImVec2(w, h));

        if (isCustom) {
            ImGui::TextDisabled("Left-click to add/drag points | Right-click to remove");
        }

        // Save / Load animation presets
        ImGui::Spacing();
        ImGui::Separator();

        struct AnimPreset { std::string name; float duration; std::string easing; std::vector<ImVec2> points; };
        static std::vector<AnimPreset> presets;
        static int loadIdx = -1;
        static bool presetsLoaded = false;

        if (!presetsLoaded) {
            presetsLoaded = true;
            FILE* f = fopen("presets.anim", "r");
            if (f) {
                char line[256];
                AnimPreset cur;
                while (fgets(line, sizeof(line), f)) {
                    size_t len = strlen(line);
                    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
                    if (line[0] == '[') {
                        if (!cur.name.empty()) presets.push_back(cur);
                        cur = AnimPreset();
                        cur.name = line + 1;
                        if (!cur.name.empty() && cur.name.back() == ']') cur.name.pop_back();
                    } else if (line[0] == 'd' && line[1] == 'u' && line[2] == 'r') {
                        sscanf(line + 9, "%f", &cur.duration);
                    } else if (line[0] == 'e' && line[1] == 'a') {
                        cur.easing = line + 7;
                    } else if (line[0] == 'p' && line[1] == 't') {
                        ImVec2 pt; if (sscanf(line + 3, "%f,%f", &pt.x, &pt.y) == 2) cur.points.push_back(pt);
                    }
                }
                if (!cur.name.empty()) presets.push_back(cur);
                fclose(f);
            }
        }

        auto savePresetsFile = [&]() {
            FILE* f = fopen("presets.anim", "w");
            if (!f) return;
            for (auto& p : presets) {
                fprintf(f, "[%s]\n", p.name.c_str());
                fprintf(f, "duration=%.3f\n", p.duration);
                fprintf(f, "easing=%s\n", p.easing.c_str());
                for (auto& pt : p.points)
                    fprintf(f, "pt=%.3f,%.3f\n", pt.x, pt.y);
                fprintf(f, "\n");
            }
            fclose(f);
        };

        ImGui::TextUnformatted("Presets:");
        if (ImGui::BeginChild("##presetList", ImVec2(ImGui::GetContentRegionAvail().x, 80), true)) {
            for (int i = 0; i < (int)presets.size(); i++) {
                bool sel = (i == loadIdx);
                if (ImGui::Selectable(presets[i].name.c_str(), sel)) {
                    loadIdx = i;
                    m_Name = presets[i].name;
                    m_Duration = presets[i].duration;
                    m_Easing = presets[i].easing;
                    m_CustomPoints = presets[i].points;
                }
            }
            ImGui::EndChild();
        }

        static char saveName[64] = "";
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90);
        ImGui::InputText("##saveName", saveName, sizeof(saveName));
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            std::string n = saveName[0] ? saveName : m_Name.c_str();
            if (!n.empty()) {
                bool found = false;
                for (auto& p : presets) {
                    if (p.name == n) {
                        p.duration = m_Duration; p.easing = m_Easing; p.points = m_CustomPoints;
                        found = true; break;
                    }
                }
                if (!found) presets.push_back({n, m_Duration, m_Easing, m_CustomPoints});
                savePresetsFile();
                saveName[0] = '\0';
            }
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Save current animation as preset");
    }
}

void AnimationNode::RenderPreview(const RenderContext& ctx) {
    ImGui::TextUnformatted(m_Name.c_str());
}

