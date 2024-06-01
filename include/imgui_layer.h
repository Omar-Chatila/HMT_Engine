#pragma once

#include "Layer.h"
#include "ui_context.h"
#include <imgui.h>

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
            ImGui::Begin("Hello, world!");
            static int counter = 0;

            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("Aspect Ratio", &m_Context.aspectRatio, 1.0f, 3.0f);
            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::ColorEdit3("clear color", (float*)&m_Context.clear_color);

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
