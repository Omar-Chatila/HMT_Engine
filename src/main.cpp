#include "display_helper.h"
#include "application.h"
#include "layers.h"
#include "ui_context.h"

UIContext *context;  

int display(std::vector<Frame> &ref_frames, std::vector<Frame> &inp_frames, std::vector<glm::vec3> &ref_spherePositions, const std::vector<glm::vec3> &sphereColors) {
    context = new UIContext();
    GLFWwindow *window = intit_window();
    // Get OpenGL version
    const GLubyte* glVersion = glGetString(GL_VERSION);
    if (glVersion) {
        std::cout << "OpenGL Version: " << glVersion << std::endl;
    } else {
        std::cerr << "Failed to get OpenGL version\n";
    }

    // Compile and link shaders
    Shader sphereShader(vertexShaderPath, fragmentShaderPath);
    Shader textured_sphereShader(tex_vertexShaderPath, tex_fragmentShaderPath);
    // Create a sphere
    Sphere sphere(0.04f, 256, 128);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    glm::vec3 center = {0.0f, 1.0f, 0.0f};
    context->center = center;
    auto io = init_imgui(window);
    bool show = true;
    // Render loop
    int current_frame = 0;

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
        glm::mat4 view = glm::lookAt(context->camera_pos, context->center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(context->fov), context->aspectRatio, context->near, context->far);

        // Draw spheres
        sphereShader.use();
        
        // Set uniform values
        sphereShader.setUniformMat4("view", view);
        sphereShader.setUniformMat4("projection", projection);
        sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        update_SpherePositions(ref_frames[current_frame % ref_frames.size()], inp_frames[current_frame % inp_frames.size()]);
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
    return 0;
}


int main() {
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        input_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // Parse input trajectory
    std::string input_file = R"(resources\fb_41_pre_splitted_1.txt)";
    Input_parser* input = new Input_parser(input_file.c_str());
    std::vector<Frame> input_frames = input->get_frames();
    const Trajectories* input_trajectories = new Trajectories(input_frames);

    // Parse reference trajectory
    std::string reference_file = R"(resources\expertise_01_single100_2_splitted_1.txt)";
    Input_parser* reference = new Input_parser(reference_file.c_str());
    std::vector<Frame> reference_frames = reference->get_frames();
    const Trajectories* reference_trajectories = new Trajectories(reference_frames);

    Trajectoy_analysis* analysis = new Trajectoy_analysis(*input_trajectories, *reference_trajectories);
    analysis->perform_DTW(Joint::l_hip, EUCLID);
    analysis->perform_EDR(Joint::l_hip, EUCLID, 3.0);

    std::cout << reference_frames[0] << std::endl;
    //update_SpherePositions(reference_frames[0]);
    display(reference_frames, input_frames, ref_spherePositions, sphereColors);

    delete input;
    delete input_trajectories;
    delete reference_trajectories;
    return 0;
}
