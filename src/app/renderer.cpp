#include "renderer.h"

Renderer::Renderer(UIContext *p_context) : context(p_context){
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        input_spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    window = init_window(context);
    init_fbo();
}

Renderer::Renderer(TrajectoryAnalysisManager *manager) {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &fboTexture);
    glDeleteRenderbuffers(1, &rbo);
}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete context;
}

GLuint fbo, fboTexture, rbo;

void Renderer::init_fbo() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, context->windowWidth, context->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, context->windowWidth, context->windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Renderer::display(TrajectoryAnalysisManager *manager) {
    auto ref_frames = manager->displayRequirements().ref_frames;
    auto inp_frames = manager->displayRequirements().inp_frames;
    auto context = manager->displayRequirements().context;
    auto alignment = manager->displayRequirements().alignment;
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
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        // Bind the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, context->windowWidth, context->windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto clear_color = context->clear_color;
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        // Render your scene
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
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ImGui window to display the FBO texture
        ImGui::Begin("Reference (left) vs Input (right)");
        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        glViewport(0, 0, windowSize.x, windowSize.y);
        ImGui::Image((void*)(intptr_t)fboTexture, windowSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

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