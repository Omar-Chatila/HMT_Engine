#include "display_helper.h"
#include "imgui_layers.h"
#include "layers.h"
#include "tests.h"
#include "trajectory_analysis_manager.h"
#include <iostream>
#include <tuple>

int display(TrajectoryAnalysisManager *manager) {
    auto ref_frames = manager->displayRequirements().ref_frames;
    auto inp_frames = manager->displayRequirements().inp_frames;
    auto context = manager->displayRequirements().context;
    auto alignment = manager->displayRequirements().alignment;
    GLFWwindow *window = init_window(context);
    
    // Compile and link shaders
    Shader sphereShader(vertexShaderPath, fragmentShaderPath);
    Shader textured_sphereShader(tex_vertexShaderPath, tex_fragmentShaderPath);
    // Create a sphere
    Sphere sphere(0.04f, 256, 128); 
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    auto io = init_imgui(window); 
    bool show = true;
    // Render loop
    int current_frame = 0;
    int map_index = 0;

    ImGUI_Layers* app = new ImGUI_Layers();
    app->push_layer<ImGuiLayer>(*context); 

    while (!glfwWindowShouldClose(window)) {
        int width = context->windowWidth;
        int height = context->windowHeight;
        glfwGetWindowSize(window, &width, &height);
        glfwSwapInterval(context->vsync);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        app->activate();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = glm::lookAt(context->camera_pos, context->center, context->camera_orientation);
        glm::mat4 projection = glm::perspective(glm::radians(context->fov), context->aspectRatio, 0.1f, 100.0f);

        sphereShader.use();
        sphereShader.setUniformMat4("view", view);
        sphereShader.setUniformMat4("projection", projection);
        sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); 

        if (context->aligned) {
            int mapping = std::get<1>(alignment)[map_index % std::get<1>(alignment).size()];
            context->c_frame = map_index % std::get<1>(alignment).size();
            update_SpherePos_Aligned(inp_frames, ref_frames, mapping);
            map_index++;
        } else {
            update_SpherePos_noAlign(ref_frames[current_frame % ref_frames.size()], inp_frames[current_frame % inp_frames.size()]);
        }
        current_frame++;
        draw_objects(projection, view, sphere, sphereShader);
        glUseProgram(0);  // Unbind the line shader program

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        auto clear_color = context->clear_color;
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete context;
    return 0;
}

int main() {
    UIContext *context = new UIContext();
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        input_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    // Parse input trajectory
    std::string input_file = R"(resources\motion_data\squats\fb_41_pre_splitted_1.txt)";
    // Parse reference trajectory
    std::string ref_file = R"(resources\motion_data\squats\expertise_01_single100_2_splitted_1.txt)";
    TrajectoryAnalysisManager *manager = new TrajectoryAnalysisManager(input_file, ref_file, context);
    manager->performAnalysis();
    manager->updateContext();

    display(manager);

    delete manager;
    return 0;
}
