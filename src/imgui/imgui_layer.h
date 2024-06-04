#pragma once

#include "Layer.h"
#include "ui_context.h"
#include <imgui.h>
#include <tuple>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <limits>
#include <cmath>
#include <vector>

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(UIContext& context) : m_Context(context) {
        precomputeDistancesAndColors();
    }

    void UpdateFOVWithScroll() {
        ImGuiIO& io = ImGui::GetIO();
        if (io.MouseWheel != 0.0f) {
            m_Context.fov -= io.MouseWheel * 2;
            if (m_Context.fov < -180.0f) m_Context.fov = -180.0f;
            if (m_Context.fov > 180.0f) m_Context.fov = 180.0f;
        }
    }

    ImU32 interpolateColor(float value, float minVal, float maxVal, ImU32 colorLow, ImU32 colorHigh) {
        float ratio = ((value - minVal) / (maxVal - minVal)) * 1.5f;
        ratio = std::max(0.0f, std::min(1.0f, ratio));

        int r1 = (colorLow >> IM_COL32_R_SHIFT) & 0xFF;
        int g1 = (colorLow >> IM_COL32_G_SHIFT) & 0xFF;
        int b1 = (colorLow >> IM_COL32_B_SHIFT) & 0xFF;
        int a1 = (colorLow >> IM_COL32_A_SHIFT) & 0xFF;

        int r2 = (colorHigh >> IM_COL32_R_SHIFT) & 0xFF;
        int g2 = (colorHigh >> IM_COL32_G_SHIFT) & 0xFF;
        int b2 = (colorHigh >> IM_COL32_B_SHIFT) & 0xFF;
        int a2 = (colorHigh >> IM_COL32_A_SHIFT) & 0xFF;

        int r = static_cast<int>(r1 + ratio * (r2 - r1));
        int g = static_cast<int>(g1 + ratio * (g2 - g1));
        int b = static_cast<int>(b1 + ratio * (b2 - b1));
        int a = static_cast<int>(a1 + ratio * (a2 - a1));

        return IM_COL32(r, g, b, a);
    }

    void precomputeDistancesAndColors() {
        int n = std::get<2>(*m_Context.matrix);
        int m = std::get<3>(*m_Context.matrix);
        std::vector<int> align_path = std::get<1>(*m_Context.matrix);
        float* mat = std::get<0>(*m_Context.matrix);

        float minDist = std::numeric_limits<float>::infinity();
        float maxDist = -std::numeric_limits<float>::infinity();

        std::vector<std::pair<int, int>> pathCoords;
        for (int idx : align_path) {
            int i = idx / (m + 1);
            int j = idx % (m + 1);
            pathCoords.emplace_back(i, j);
        }

        distances.resize((n + 1) * (m + 1), std::numeric_limits<float>::infinity());

        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                float minDistanceToPath = std::numeric_limits<float>::infinity();
                for (const auto& [pi, pj] : pathCoords) {
                    float distance = std::hypot(i - pi, j - pj);
                    if (distance < minDistanceToPath) {
                        minDistanceToPath = distance;
                    }
                }
                distances[i * (m + 1) + j] = minDistanceToPath;
                if (minDistanceToPath < minDist) minDist = minDistanceToPath;
                if (minDistanceToPath > maxDist) maxDist = minDistanceToPath;
            }
        }

        colors.resize((n + 1) * (m + 1), IM_COL32(0, 0, 0, 255));

        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                int index = i * (m + 1) + j;
                bool isAligned = (std::find(align_path.begin(), align_path.end(), index) != align_path.end());
                if (isAligned) {
                    colors[index] = IM_COL32_WHITE;
                } else {
                    colors[index] = interpolateColor(distances[index], minDist, maxDist, IM_COL32(0, 0, 255, 255), IM_COL32(255, 0, 0, 255));
                }
            }
        }
    }

    void drawDTWDiagram() {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        int n = std::get<2>(*m_Context.matrix);
        int m = std::get<3>(*m_Context.matrix);
        const float rect_size = 1.0f;
        float xPos = p.x;
        float yPos = p.y;

        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                int index = i * (m + 1) + j;
                ImU32 color = colors[index];
                draw_list->AddRectFilled(ImVec2(xPos, yPos), ImVec2(xPos + rect_size, yPos + rect_size), color);
                xPos += rect_size;
            }
            xPos = p.x;
            yPos += rect_size;
        }

        // Advance the ImGui cursor to claim space in the window (otherwise the window will appear small and needs to be resized)
        ImGui::Dummy(ImVec2((m + 1) * rect_size, (n + 1) * rect_size));
    }

    virtual void onRender() override {
        static bool show_demo_window = true;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        {
            ImGui::Begin("Visualizer for Motion Data");
            
            if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("Aspect Ratio", &m_Context.aspectRatio, 1.0f, 3.0f);
                UpdateFOVWithScroll();
                ImGui::SliderFloat("Fov", &m_Context.fov, -180.0f, 180.0f);
                
                ImGui::DragFloat3("Center", glm::value_ptr(m_Context.center), 0.1f, -3.0f, 3.0f);
                ImGui::SameLine();
                if (ImGui::Button("Reset##Center"))
                    m_Context.center = {0.4f, 1.0f, 0.0f};
                
                ImGui::DragFloat3("Position", glm::value_ptr(m_Context.camera_pos), 0.1f, -5.0f, 5.0f);
                ImGui::SameLine();
                if (ImGui::Button("Reset##Position"))
                    m_Context.camera_pos = {2.0f, 2.0f, 2.0f};

                ImGui::DragFloat3("Orientation", glm::value_ptr(m_Context.camera_orientation), 0.1f, -1.0f, 1.0f);
                ImGui::SameLine();
                if (ImGui::Button("Reset##Orientation"))
                    m_Context.camera_orientation = {0.0, 1.0, 0.0};

                ImGui::ColorEdit3("Clear Color", (float*)&m_Context.clear_color);
            };

            ImGui::Checkbox("VSync", &m_Context.vsync);
            ImGui::Checkbox("DTW Aligned", &m_Context.aligned);
            ImGui::SameLine();
            ImGui::Checkbox("Show Heatmap", &showDiagram);
            if (showDiagram)
                drawDTWDiagram();
            ImGui::Text("Aspect Ratio = %.3f", m_Context.aspectRatio);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            
            ImGui::End();
        }
    }

private:
    UIContext& m_Context;
    std::vector<float> distances;
    std::vector<ImU32> colors;
    bool showDiagram = false;
};
