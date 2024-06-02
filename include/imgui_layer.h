#pragma once

#include "Layer.h"
#include "ui_context.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(UIContext& context) : m_Context(context) {}

    virtual void onRender() override {
        static bool show_demo_window = false;
        static bool show_another_window = false;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        {
            ImGui::Begin("Visualizer for squats");
            static int counter = 0;

            if (ImGui::CollapsingHeader("Camera Options", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("Aspect Ratio", &m_Context.aspectRatio, 1.0f, 3.0f);
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
