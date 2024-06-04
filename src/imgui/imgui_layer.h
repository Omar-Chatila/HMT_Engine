#pragma once

#include "Layer.h"
#include "ui_context.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(UIContext& context) : m_Context(context) {}

    void UpdateFOVWithScroll() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseWheel != 0.0f) {
        m_Context.fov -= io.MouseWheel * 2; // Adjust the sensitivity as needed
        if (m_Context.fov < -180.0f) m_Context.fov = -180.0f;
        if (m_Context.fov > 180.0f) m_Context.fov = 180.0f;
    }
}

    virtual void onRender() override {
        static bool show_demo_window = true;
        static bool show_another_window = false;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        {
            ImGui::Begin("Visualizer for Motion Data");
            
            // if (ImGui::CollapsingHeader("Motion Data Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            //     if (ImGui::RadioButton("Squats", m_Context.squats)) {
            //         m_Context.squats = true;
            //     }
            //     ImGui::SameLine();
            //     if (ImGui::RadioButton("Tai-Chi", !m_Context.squats)) {
            //         m_Context.squats = false;
            //     }
            //     std::vector<std::string> sport_type = m_Context.squats ? m_Context.amateur_squats : m_Context.amateur_tai;
            //     if (ImGui::BeginCombo("Amateur", sport_type[m_Context.selected_amateur].c_str())) {
            //         for (int i = 0; i < sport_type.size(); i++) {
            //             bool isSelected = (m_Context.selected_amateur == i);
            //             if (ImGui::Selectable(sport_type[i].c_str(), isSelected)) {
            //                 m_Context.selected_amateur = i;
            //             }
            //             if (isSelected) {
            //                 ImGui::SetItemDefaultFocus();
            //             }
            //         }
            //         ImGui::EndCombo();
            //     }
            //     ImGui::SameLine();

            //     std::vector<std::string> sport_type2 = m_Context.squats ? m_Context.prof_squats : m_Context.prof_tai;
            //     if (ImGui::BeginCombo("Reference", sport_type2[m_Context.selected_prof].c_str())) {
            //         for (int i = 0; i < sport_type2.size(); i++) {
            //             bool isSelected = (m_Context.selected_prof == i);
            //             if (ImGui::Selectable(sport_type2[i].c_str(), isSelected)) {
            //                 m_Context.selected_prof = i;
            //             }
            //             if (isSelected) {
            //                 ImGui::SetItemDefaultFocus();
            //             }
            //         }
            //         ImGui::EndCombo();
            //     }
            // }

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

            ImGui::Text("Aspect Ratio = %.3f", m_Context.aspectRatio);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            
            ImGui::End();
        }

        if (show_another_window) {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
    }

private:
    UIContext& m_Context;
};
