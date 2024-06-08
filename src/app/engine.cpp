#include "Engine.h"

Renderer::Renderer(TrajectoryAnalysisManager *manager) {
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        input_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    this->dispReq = manager->displayRequirements();
    this->window = init_window(this->dispReq->context);
    init_fbo();
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteFramebuffers(1, &fbo1);
    glDeleteTextures(1, &fboTexture1);
    glDeleteRenderbuffers(1, &rbo1);
    glDeleteFramebuffers(1, &fbo2);
    glDeleteTextures(1, &fboTexture2);
    glDeleteRenderbuffers(1, &rbo2);
    glfwDestroyWindow(window);
    glfwTerminate();
    delete dispReq;
}

void Renderer::init_fbo() {
    // First FBO
    glGenFramebuffers(1, &fbo1);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);

    glGenTextures(1, &fboTexture1);
    glBindTexture(GL_TEXTURE_2D, fboTexture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dispReq->context->windowWidth, dispReq->context->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture1, 0);

    glGenRenderbuffers(1, &rbo1);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo1);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dispReq->context->windowWidth, dispReq->context->windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo1);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer 1 is not complete!" << std::endl;
    }

    // Second FBO
    glGenFramebuffers(1, &fbo2);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);

    glGenTextures(1, &fboTexture2);
    glBindTexture(GL_TEXTURE_2D, fboTexture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dispReq->context->windowWidth, dispReq->context->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture2, 0);

    glGenRenderbuffers(1, &rbo2);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dispReq->context->windowWidth, dispReq->context->windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo2);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer 2 is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


int Renderer::display()
{
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
    app->push_layer<ImGuiLayer>(*dispReq->context);

    while (!glfwWindowShouldClose(window)) {
        int width = dispReq->context->windowWidth;
        int height = dispReq->context->windowHeight;
        glfwGetWindowSize(window, &width, &height);
        glfwSwapInterval(dispReq->context->vsync);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        app->activate();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        // Bind the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
        glViewport(0, 0, dispReq->context->windowWidth, dispReq->context->windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto clear_color = dispReq->context->clear_color;
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        // Render your scene
        glm::mat4 view = glm::lookAt(dispReq->context->camera_pos, dispReq->context->center, dispReq->context->camera_orientation);
        glm::mat4 projection = glm::perspective(glm::radians(dispReq->context->fov), dispReq->context->aspectRatio, 0.1f, 100.0f);
        sphereShader.use();
        sphereShader.setUniformMat4("view", view);
        sphereShader.setUniformMat4("projection", projection);
        sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        if (dispReq->context->aligned) {
            int mapping = std::get<1>(dispReq->alignment)[map_index % std::get<1>(dispReq->alignment).size()];
            dispReq->context->c_frame = map_index % std::get<1>(dispReq->alignment).size();
            update_SpherePos_Aligned(dispReq->inp_frames, dispReq->ref_frames, mapping);
            map_index++;
        } else {
            update_SpherePos_noAlign(dispReq->ref_frames[current_frame % dispReq->ref_frames.size()], dispReq->inp_frames[current_frame % dispReq->inp_frames.size()]);
        }
        current_frame++;
        //draw_objects(projection, view, sphere, sphereShader);
        glUseProgram(0);

        // Render to FBO1
        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
        glViewport(0, 0, dispReq->context->windowWidth, dispReq->context->windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(ref_spherePositions, sphere, sphereShader, dispReq->context);

        // Render to FBO2
        glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
        glViewport(0, 0, dispReq->context->windowWidth, dispReq->context->windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(input_spherePositions, sphere, sphereShader, dispReq->context);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));  // Adjust window padding


        ImGui::Begin("Reference Viewport");
        ImVec2 windowSize1 = ImGui::GetContentRegionAvail();
        ImGui::Image((void*)(intptr_t)fboTexture1, windowSize1, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        // ImGui window for FBO2
        ImGui::Begin("Input Viewport");
        ImVec2 windowSize2 = ImGui::GetContentRegionAvail();
        ImGui::Image((void*)(intptr_t)fboTexture2, windowSize2, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::Render();
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
    return 0;
}

void Renderer::draw_scene(const std::vector<glm::vec3>& spherePositions, Sphere &sphere, Shader &sphereShader, UIContext *context) {
    glm::mat4 view = glm::lookAt(context->camera_pos, context->center, context->camera_orientation);
    glm::mat4 projection = glm::perspective(glm::radians(context->fov), context->aspectRatio, 0.1f, 100.0f);

    sphereShader.use();
    sphereShader.setUniformMat4("view", view);
    sphereShader.setUniformMat4("projection", projection);
    sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
    sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
    sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    for (size_t i = 0; i < JOINT_COUNT; i++) {
        glm::vec3 position = spherePositions[i];
        glm::vec3 color = sphereColors[i];
        sphereShader.setUniformVec3("objectColor", color);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        sphereShader.setUniformMat4("model", model);
        sphere.draw();
    }
    sphereShader.unuse();  // Unbind the sphere shader program
    for (auto& bone : Joint::bones) {
        glm::vec3 start = spherePositions[bone.first];
        glm::vec3 end = spherePositions[bone.second];
        Line line(start, end);
        glUseProgram(line.shaderProgram);
        glm::mat4 model = glm::mat4(1.0f);
        line.setMVP(projection * view * model);
        line.draw();
    }
}


void Renderer::update_SpherePos_noAlign(Frame &ref_frame, Frame &inp_frame) {
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions[i].x = ref_frame.joint_translations[i].x - 0.5;
        ref_spherePositions[i].y = ref_frame.joint_translations[i].y;
        ref_spherePositions[i].z = ref_frame.joint_translations[i].z;

        input_spherePositions[i].x = inp_frame.joint_translations[i].x + 1.0f;
        input_spherePositions[i].y = inp_frame.joint_translations[i].y;
        input_spherePositions[i].z = inp_frame.joint_translations[i].z;
    }
}

void Renderer::draw_objects(glm::mat4 &projection, glm::mat4 &view, Sphere &sphere, Shader &sphereShader) {
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
    glUseProgram(0);
}

void
Renderer::update_SpherePos_Aligned(std::vector <Frame> &input_frames, std::vector <Frame> &ref_frames, int mapping) {
    int n = input_frames.size();
    int m = ref_frames.size();
    int in = mapping / (m + 1) - 1;
    int ref = mapping % (m + 1) - 1;
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions[i].x = ref_frames[ref].joint_translations[i].x - 0.5;
        ref_spherePositions[i].y = ref_frames[ref].joint_translations[i].y;
        ref_spherePositions[i].z = ref_frames[ref].joint_translations[i].z;

        input_spherePositions[i].x = input_frames[in].joint_translations[i].x + 1.0f;
        input_spherePositions[i].y = input_frames[in].joint_translations[i].y;
        input_spherePositions[i].z = input_frames[in].joint_translations[i].z;
    }
}

GLFWwindow *Renderer::init_window(UIContext *context) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Create a GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }

    const GLubyte* glVersion = glGetString(GL_VERSION);
    if (glVersion) {
        std::cout << "OpenGL Version: " << glVersion << std::endl;
    } else {
        std::cerr << "Failed to get OpenGL version\n";
    }

    std::string windowTitle = "Motion Visualizer " + context->reference_file + " vs. " + context->input_file + " - " + " OpenGL Version: " +  reinterpret_cast<const char*>(glVersion);
    glfwSetWindowTitle(window, windowTitle.c_str());

    return window;
}

ImGuiIO &Renderer::init_imgui(GLFWwindow *window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return io;
}

void Renderer::setDisplayRequirements(DisplayRequirements *p_req) {
     this->dispReq = p_req;
}