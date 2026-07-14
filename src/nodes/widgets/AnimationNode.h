#pragma once
#include "NodeEditor/Node.h"
#include <vector>

class AnimationNode : public Node {
public:
    AnimationNode(int id);
    void Draw() override;
    const char* GetTypeName() const override { return "Animation"; }
    const char* GetDisplayLabel() const override { return m_Name.c_str(); }
    void DrawProperties() override;
    void RenderPreview(const RenderContext& ctx) override;
    void SaveExtra(ImGuiTextBuffer& b) const override;
    void LoadExtra(const char* key, const char* line) override;

    // Evaluate the custom spline at t ∈ [0,1]
    // Uses Catmull-Rom interpolation for smooth curves through all points
    float EvalCustom(float t) const {
        if (m_CustomPoints.empty()) return t;
        // Build full point list with implicit start/end
        std::vector<ImVec2> pts;
        pts.push_back(ImVec2(0, 0));
        pts.insert(pts.end(), m_CustomPoints.begin(), m_CustomPoints.end());
        pts.push_back(ImVec2(1, 1));
        int n = (int)pts.size();

        // Find which segment t falls into
        float seg = t * (n - 1);
        int i = (int)seg;
        if (i < 0) i = 0;
        if (i >= n - 1) i = n - 2;
        float u = seg - i;

        // Catmull-Rom: need 4 points
        ImVec2 P0 = (i > 0) ? pts[i - 1] : pts[i];
        ImVec2 P1 = pts[i];
        ImVec2 P2 = pts[i + 1];
        ImVec2 P3 = (i + 2 < n) ? pts[i + 2] : pts[i + 1];

        float u2 = u * u;
        float u3 = u2 * u;
        return 0.5f * ((2.0f * P1.y) +
            (-P0.y + P2.y) * u +
            (2.0f * P0.y - 5.0f * P1.y + 4.0f * P2.y - P3.y) * u2 +
            (-P0.y + 3.0f * P1.y - 3.0f * P2.y + P3.y) * u3);
    }

    // Apply easing from this AnimationNode to value t ∈ [0,1]
    // If animNode is null, falls back to smoothstep
    static float ApplyEasing(float t, const std::string& easing, const AnimationNode* animNode);

    std::string m_Name = "Anim";
    std::string m_Target;
    float m_Duration = 0.35f;
    std::string m_Easing = "Linear";
    // Custom multi‑point control points (used when m_Easing == "Custom")
    std::vector<ImVec2> m_CustomPoints;
};
