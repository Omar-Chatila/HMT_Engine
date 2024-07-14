#include "engine.h"

Renderer::Renderer(Data *p_data) {
    this->data = p_data;
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions.emplace_back(0.0f, 0.0f, 0.0f);
        input_spherePositions.emplace_back(0.0f, 0.0f, 0.0f);
    }
    this->window = init_window();
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
}

void Renderer::init_fbo() {
    // First FBO
    glGenFramebuffers(1, &fbo1);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);

    glGenTextures(1, &fboTexture1);
    glBindTexture(GL_TEXTURE_2D, fboTexture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data->refView->windowWidth,
                 data->refView->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture1, 0);

    glGenRenderbuffers(1, &rbo1);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo1);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, data->inputView->windowWidth,
                          data->inputView->windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo1);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer 1 is not complete!" << std::endl;
    }

    // Second FBO
    glGenFramebuffers(1, &fbo2);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);

    glGenTextures(1, &fboTexture2);
    glBindTexture(GL_TEXTURE_2D, fboTexture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data->refView->windowWidth,
                 data->refView->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture2, 0);

    glGenRenderbuffers(1, &rbo2);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, data->refView->windowWidth,
                          data->refView->windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo2);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer 2 is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


int Renderer::display() {
    // Compile and link shaders
    Shader sphereShader(vertexShaderPath, fragmentShaderPath);
    Shader floorShader(floor_vertex_shader_path, floor_fragment_shader_path); // New shader for the floor

    // Create a sphere
    Sphere sphere(0.04f, 256, 128);

    // Create floor vertex data
    float floorVertices[] = {
            // positions          // normals      // texture coords
            -10.0f, -0.2f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            10.0f, -0.2f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            10.0f, -0.2f, 10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -10.0f, -0.2f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int floorIndices[] = {
            0, 1, 2,
            0, 2, 3
    };

    unsigned int floorVAO, floorVBO, floorEBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindVertexArray(floorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    auto io = init_imgui(window);

    // Render loop
    int current_frame = 0;
    int map_index = 0;

    auto *app = new ImGUI_Layers();
    app->push_layer<ImGuiLayer>(data);
    app->push_layer<ResultLayer>(data);
    app->push_layer<ClassifierLayer>(data);
    app->push_layer<SetupLayer>(data);
    int lastInpIndex = -1;
    int lastRefIndex = -1;

    while (!glfwWindowShouldClose(window)) {
        int width = 1920;
        int height = 1080;
        glfwGetWindowSize(window, &width, &height);
        glfwSwapInterval(data->mainLayerContext->vsync);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        app->activate();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Bind the FBO1
        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
        glViewport(0, 0, data->inputView->windowWidth,
                   data->inputView->windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto clear_color = data->inputView->clear_color;
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                     clear_color.w);

        // Render your scene
        glm::mat4 view = glm::lookAt(data->refView->camera_pos,
                                     data->refView->center,
                                     data->refView->camera_orientation);
        glm::mat4 projection = glm::perspective(glm::radians(data->refView->fov),
                                                data->refView->aspectRatio, 0.1f, 100.0f);

        glm::mat4 view2 = glm::lookAt(data->inputView->camera_pos,
                                      data->inputView->center,
                                      data->inputView->camera_orientation);
        glm::mat4 projection2 = glm::perspective(glm::radians(data->inputView->fov),
                                                 data->inputView->aspectRatio, 0.1f, 100.0f);

        sphereShader.use();
        sphereShader.setUniformMat4("view", view);
        sphereShader.setUniformMat4("projection", projection);
        sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        auto ref_frms = data->bestMatch->getRefFrames();
        auto in_frms = data->bestMatch->getInputFrames();
        if (data->mainLayerContext->aligned) {
            // TODO: Das hier nur ein mal getten
            auto alignment = data->bestMatch->matching_algorithms[data->mainLayerContext->dtwVariant]->alignmentPath;
            IM_ASSERT(!alignment.empty());
            int mapping = alignment[map_index % alignment.size()];
            int m = ref_frms.size();
            int in = mapping / (m + 1) - 1;
            int ref = mapping % (m + 1) - 1;
            data->c_ref_frame_aligned = ref;
            data->c_inp_frame_aligned = in;
            data->mainLayerContext->inpPaused = lastInpIndex == in;
            data->mainLayerContext->refPaused = lastRefIndex == ref;
            lastInpIndex = in;
            lastRefIndex = ref;
            data->mainLayerContext->c_frame = map_index % alignment.size();
            if (data->mainLayerContext->autoMode) {
                update_SpherePos_Aligned(in_frms, ref_frms, mapping, false, true);
            } else {
                update_SpherePos_Aligned(in_frms, ref_frms, data->mainLayerContext->mousePos.second,
                                         data->mainLayerContext->mousePos.first);
            }
            map_index++;

        } else {
            data->c_ref_frame = static_cast<int>(current_frame % ref_frms.size());
            data->c_inp_frame = static_cast<int>(current_frame % in_frms.size());
            update_SpherePos_noAlign(ref_frms[data->c_ref_frame],
                                     in_frms[data->c_inp_frame]);
        }
        current_frame++;
        //draw_objects(projection, view, sphere, sphereShader);
        glUseProgram(0);

        // Render to FBO1
        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
        glViewport(0, 0, data->refView->windowWidth,
                   data->refView->windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(ref_spherePositions, sphere, sphereShader, true);
        auto clear_color1 = data->refView->clear_color;
        glClearColor(clear_color1.x * clear_color1.w, clear_color1.y * clear_color1.w, clear_color1.z * clear_color1.w,
                     clear_color1.w);

        // Render to FBO2
        glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
        glViewport(0, 0, data->inputView->windowWidth,
                   data->inputView->windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(input_spherePositions, sphere, sphereShader, 0);
        auto clear_color2 = data->inputView->clear_color;
        glClearColor(clear_color2.x * clear_color2.w, clear_color2.y * clear_color2.w, clear_color2.z * clear_color2.w,
                     clear_color2.w);

        // Render floor right
        floorShader.use();
        floorShader.setUniformMat4("view", view2);
        floorShader.setUniformMat4("projection", projection2);
        floorShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        floorShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        floorShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        floorShader.setUniformMat4("model", glm::mat4(1.0f)); // Identity matrix since the floor is not transformed

        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo1);

        // Render floor left
        floorShader.use();
        floorShader.setUniformMat4("view", view);
        floorShader.setUniformMat4("projection", projection);
        floorShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        floorShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        floorShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        floorShader.setUniformMat4("model", glm::mat4(1.0f)); // Identity matrix since the floor is not transformed

        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));  // Adjust window padding

        ImGui::Begin("Reference Viewport");
        ImVec2 windowSize1 = ImGui::GetContentRegionAvail();
        ImGui::Image((void *) (intptr_t) fboTexture1, windowSize1, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        // ImGui window for FBO2
        ImGui::Begin("Input Viewport");
        ImVec2 windowSize2 = ImGui::GetContentRegionAvail();
        ImGui::Image((void *) (intptr_t) fboTexture2, windowSize2, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

void
Renderer::draw_scene(const std::vector<glm::vec3> &spherePositions, Sphere &sphere, Shader &sphereShader,
                     bool ref) {
    glm::mat4 view = glm::lookAt(ref ? data->refView->camera_pos : data->inputView->camera_pos,
                                 ref ? data->refView->center : data->inputView->center,
                                 ref ? data->refView->camera_orientation : data->inputView->camera_orientation);
    glm::mat4 projection = glm::perspective(glm::radians(ref ? data->refView->fov : data->inputView->fov),
                                            ref ? data->refView->aspectRatio : data->inputView->aspectRatio, 0.1f,
                                            100.0f);

    sphereShader.use();
    sphereShader.setUniformMat4("view", view);
    sphereShader.setUniformMat4("projection", projection);
    sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
    sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
    sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    for (size_t i = 0; i < JOINT_COUNT; i++) {
        glm::vec3 position = spherePositions[i];
        glm::vec3 color = sphereColors[i];
        if (data->mainLayerContext->autoMode &&
            (ref && data->mainLayerContext->refPaused || !ref && data->mainLayerContext->inpPaused)) {
            color = sphereColors[19];
        }
        sphereShader.setUniformVec3("objectColor", color);
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        sphereShader.setUniformMat4("model", model);
        sphere.draw();
    }
    sphereShader.unuse();  // Unbind the sphere shader program
    for (auto &bone: Joint::bones) {
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

void
Renderer::update_SpherePos_Aligned(std::vector<Frame> &input_frames, std::vector<Frame> &ref_frames, int mapping,
                                   bool refPause, bool inpPause) {
    int m = ref_frames.size();
    int in = mapping / (m + 1) - 1;
    int ref = mapping % (m + 1) - 1;
    update_SpherePos_Aligned(input_frames, ref_frames, in, ref);
}

void
Renderer::update_SpherePos_Aligned(std::vector<Frame> &input_frames, std::vector<Frame> &ref_frames, int in,
                                   int ref) {
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions[i].x = ref_frames[ref].joint_translations[i].x - 0.5;
        ref_spherePositions[i].y = ref_frames[ref].joint_translations[i].y;
        ref_spherePositions[i].z = ref_frames[ref].joint_translations[i].z;

        input_spherePositions[i].x = input_frames[in].joint_translations[i].x + 1.0f;
        input_spherePositions[i].y = input_frames[in].joint_translations[i].y;
        input_spherePositions[i].z = input_frames[in].joint_translations[i].z;
    }
}

GLFWwindow *Renderer::init_window() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    // Get the primary monitor and its video mode
    GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
    // Create a GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Window", nullptr, nullptr);
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

    const GLubyte *glVersion = glGetString(GL_VERSION);
    if (glVersion) {
        std::cout << "OpenGL Version: " << glVersion << std::endl;
    } else {
        std::cerr << "Failed to get OpenGL version\n";
    }
    std::string windowTitle =
            std::string("Motion Files Visualizer - OpenGL Version: ") + reinterpret_cast<const char *>(glVersion);
    glfwSetWindowTitle(window, windowTitle.c_str());
    return window;
}

ImGuiIO &Renderer::init_imgui(GLFWwindow *p_window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    ImGui_ImplGlfw_InitForOpenGL(p_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return io;
}