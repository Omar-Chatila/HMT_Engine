#include "ui_context.h"
#include "display_helper.h"
#include "application.h"
#include "layers.h"
#include "tests.h"
#include "motion_data.h"
#include <iostream>

int display(std::vector<Frame> &ref_frames, std::vector<Frame> &inp_frames, UIContext *context, std::pair<float, std::vector<int>> &alignment) {
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

    Application* app = new Application();
    app->push_layer<ImGuiLayer>(*context); 

    while (!glfwWindowShouldClose(window)) {
        int width = context->windowWidth;
        int height = context->windowHeight;
        glfwGetWindowSize(window, &width, &height);
        //aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT); // for example modify this in layer
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        app->activate();
     
        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view and projection matrices
        glm::mat4 view = glm::lookAt(context->camera_pos, context->center, context->camera_orientation);
        glm::mat4 projection = glm::perspective(glm::radians(context->fov), context->aspectRatio, 0.1f, 100.0f);

        // Draw spheres
        sphereShader.use();
        
        // Set uniform values
        sphereShader.setUniformMat4("view", view);
        sphereShader.setUniformMat4("projection", projection);
        sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); 

        if (context->aligned) {
            int mapping = alignment.second[map_index % (alignment.second.size() - 1)];
            update_SpherePos_Aligned(inp_frames, ref_frames, mapping);
            map_index++;
        } else {
            update_SpherePos_noAlign(ref_frames[current_frame % ref_frames.size()], inp_frames[current_frame % inp_frames.size()]);
        }
        current_frame++;
        
        for (size_t i = 0; i < JOINT_COUNT; i++) {
            glm::vec3 position = ref_spherePositions[i];
            glm::vec3 color = sphereColors[i];
            sphereShader.setUniformVec3("objectColor", color);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            sphereShader.setUniformMat4("model", model);
            sphere.draw();
        }

        for (size_t i = 0; i < JOINT_COUNT; i++) {
            glm::vec3 position = input_spherePositions[i];
            glm::vec3 color = sphereColors[i];
            sphereShader.setUniformVec3("objectColor", color);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            sphereShader.setUniformMat4("model", model);
            sphere.draw();
        }

        sphereShader.unuse();  // Unbind the sphere shader program
        for (auto& bone : Joint::bones) {
            glm::vec3 start = ref_spherePositions[bone.first];
            glm::vec3 end = ref_spherePositions[bone.second];
            Line line(start, end);
            glUseProgram(line.shaderProgram);
            glm::mat4 model = glm::mat4(1.0f);
            line.setMVP(projection * view * model);
            line.draw();
        }

        for (auto& bone : Joint::bones) {
            glm::vec3 start = input_spherePositions[bone.first];
            glm::vec3 end = input_spherePositions[bone.second];
            Line line(start, end);
            glUseProgram(line.shaderProgram);
            glm::mat4 model = glm::mat4(1.0f);
            line.setMVP(projection * view * model);
            line.draw();
        }
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

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly) 
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
    std::string input_file = R"(resources\fb_41_pre_splitted_1.txt)";
    Input_parser* input = new Input_parser(input_file.c_str());
    std::vector<Frame> input_frames = input->get_frames();
    Trajectories* input_trajectories = new Trajectories(input_frames);

    // Parse reference trajectory
    std::string ref_file = R"(resources\expertise_01_single100_2_splitted_1.txt)";
    Input_parser* reference = new Input_parser(ref_file.c_str());
    std::vector<Frame> ref_frms = reference->get_frames();
    Trajectories* ref_trajcts = new Trajectories(ref_frms);

    Trajectoy_analysis* analysis = new Trajectoy_analysis(*input_trajectories, *ref_trajcts);
    analysis->perform_DTW(Joint::l_hip, EUCLID);
    std::pair<float, std::vector<int>> alignment = analysis->perform_DTW(input_trajectories->get_anglesTrajectories(), ref_trajcts->get_anglesTrajectories());
    std::cout << "Cost: " << alignment.first << std::endl;

    analysis->perform_EDR(Joint::l_hip, EUCLID, 3.0);

    context->reference_file = cropString(ref_file).c_str();
    context->input_file = cropString(input_file).c_str();
    if (display(ref_frms, input_frames, context, alignment)) {
        std::cout << "Render Error" << std::endl;
    } 

    int n = input_trajectories->get_anglesTrajectories().size();
    int m = ref_trajcts->get_anglesTrajectories().size();

    int maxi = 0;
    int maxj = 0;
    for (int p : alignment.second) {
        maxi = std::max(p / (n + 1), maxi); 
        maxj = std::max(p % (m + 1), maxj);
    }

    std::string squats_info = R"(resources\squats_subject_info.csv)";

    delete analysis;
    delete ref_trajcts;
    delete reference;
    delete input_trajectories;
    delete input;
    return 0;
}
