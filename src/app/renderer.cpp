/* #include "renderer.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include "imgui_layer.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

const char *vertexShaderPath = "src/shader/vertex_shader.glsl";
const char *fragmentShaderPath = "src/shader/fragment_shader.glsl";
const char *tex_vertexShaderPath = "src/shader/vertex_shader.glsl";
const char *tex_fragmentShaderPath = "src/shader/textured_fragment_shader.glsl";

Renderer::Renderer(UIContext* context) : context(context), window(nullptr), io(nullptr), sphereShader(nullptr), sphere(nullptr) {
    window = initWindow(context);
    io = &initImGui(window);

    sphereShader = new Shader(vertexShaderPath, fragmentShaderPath);
    sphere = new Sphere(0.04f, 256, 128);

    glEnable(GL_DEPTH_TEST);
    initializePositions();
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete sphereShader;
    delete sphere;
}

GLFWwindow* Renderer::initWindow(UIContext* context) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "sdf", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }

    std::string windowTitle = "Motion Visualizer " + context->reference_file + " vs. " + context->input_file + " - OpenGL Version: " + reinterpret_cast<const char*>(glGetString(GL_VERSION));
    glfwSetWindowTitle(window, windowTitle.c_str());

    return window;
}

ImGuiIO& Renderer::initImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return io;
}

void Renderer::initializePositions() {
    for (int i = 0; i < JOINT_COUNT; ++i) {
        ref_spherePositions.emplace_back(0.0f, 0.0f, 0.0f);
        input_spherePositions.emplace_back(0.0f, 0.0f, 0.0f);
    }
}

void Renderer::updateSpherePos(Frame& frame, std::vector<glm::vec3>& spherePositions, float x_offset) {
    for (int i = 0; i < JOINT_COUNT; ++i) {
        spherePositions[i].x = frame.joint_translations[i].x + x_offset;
        spherePositions[i].y = frame.joint_translations[i].y;
        spherePositions[i].z = frame.joint_translations[i].z;
    }
}

void Renderer::updateSpherePosNoAlign(Frame& ref_frame, Frame& inp_frame) {
    updateSpherePos(ref_frame, ref_spherePositions, -0.5f);
    updateSpherePos(inp_frame, input_spherePositions, 1.0f);
}

void Renderer::updateSpherePosAligned(std::vector<Frame>& ref_frames, std::vector<Frame>& input_frames, int mapping) {
    int m = ref_frames.size();
    int in = mapping / (m + 1) - 1;
    int ref = mapping % (m + 1) - 1;
    updateSpherePos(ref_frames[ref], ref_spherePositions, -0.5f);
    updateSpherePos(input_frames[in], input_spherePositions, 1.0f);
}

void Renderer::drawSpheres(glm::mat4& projection, glm::mat4& view, Sphere& sphere, Shader& sphereShader, std::vector<glm::vec3>& spherePositions) {
    for (size_t i = 0; i < JOINT_COUNT; ++i) {
        glm::vec3 position = spherePositions[i];
        glm::vec3 color = sphereColors[i];
        sphereShader.setUniformVec3("objectColor", color);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        sphereShader.setUniformMat4("model", model);
        sphere.draw();
    }
}

void Renderer::drawBones(glm::mat4& projection, glm::mat4& view, std::vector<glm::vec3>& spherePositions) {
    for (const auto& bone : Joint::bones) {
        glm::vec3 start = spherePositions[bone.first];
        glm::vec3 end = spherePositions[bone.second];
        Line line(start, end);
        glUseProgram(line.shaderProgram);
        glm::mat4 model = glm::mat4(1.0f);
        line.setMVP(projection * view * model);
        line.draw();
    }
}

void Renderer::drawObjects(glm::mat4& projection, glm::mat4& view, Sphere& sphere, Shader& sphereShader) {
    drawSpheres(projection, view, sphere, sphereShader, ref_spherePositions);
    drawSpheres(projection, view, sphere, sphereShader, input_spherePositions);
    sphereShader.unuse();
    drawBones(projection, view, ref_spherePositions);
    drawBones(projection, view, input_spherePositions);
    glUseProgram(0);
}

void Renderer::renderLoop(std::vector<Frame>& ref_frames, std::vector<Frame>& inp_frames, std::tuple<float, std::vector<int>, float*>& alignment) {
    int current_frame = 0;
    int map_index = 0;

    ImGUI_Layers* app = new ImGUI_Layers();
    app->push_layer<ImGuiLayer>(*context);

    while (!glfwWindowShouldClose(window)) {
        int width, height;
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

        sphereShader->use();
        sphereShader->setUniformMat4("view", view);
        sphereShader->setUniformMat4("projection", projection);
        sphereShader->setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        sphereShader->setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        sphereShader->setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        if (context->aligned) {
            int mapping = std::get<1>(alignment)[map_index % std::get<1>(alignment).size()];
            context->c_frame = map_index % std::get<1>(alignment).size();
            updateSpherePosAligned(ref_frames, inp_frames, mapping);
            map_index++;
        } else {
            updateSpherePosNoAlign(ref_frames[current_frame % ref_frames.size()], inp_frames[current_frame % inp_frames.size()]);
        }
        current_frame++;
        drawObjects(projection, view, *sphere, *sphereShader);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(context->clear_color.x * context->clear_color.w, context->clear_color.y * context->clear_color.w, context->clear_color.z * context->clear_color.w, context->clear_color.w);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete app;
}
 */