#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "ui_context.h"
#include "Shader.h"
#include "Sphere.h"
#include "input_parser.h"
#include "trajectories.h"
#include "trajectory_analysis.h"
#include "line.h"

const char *vertexShaderPath = "src/shader/vertex_shader.glsl";
const char *fragmentShaderPath = "src/shader/fragment_shader.glsl";
const char *tex_vertexShaderPath = "src/shader/vertex_shader.glsl";
const char *tex_fragmentShaderPath = "src/shader/textured_fragment_shader.glsl";

std::vector<glm::vec3> ref_spherePositions;
std::vector<glm::vec3> input_spherePositions;

namespace Joint {
    std::vector<std::pair<int, int>> bones = {
         {l_shoulder, l_sternoclavicular},
         {l_sternoclavicular, r_sternoclavicular},
         {r_sternoclavicular, r_shoulder},

         {skullbase, vc7},
         {vc7, vt10},
         {vt10, vl5},
        
         {HumanoidRoot, l_hip},
         {HumanoidRoot, r_hip},
         {HumanoidRoot, vl5},

         {l_shoulder, l_elbow},
         {l_elbow, l_wrist},
         {r_shoulder, r_elbow},
         {r_elbow, r_wrist},

         {l_hip, l_knee},
         {l_knee, l_ankle},
         {r_hip, r_knee},
         {r_knee, r_ankle}
    };
};

// Define sphere colors for each joint type
std::vector<glm::vec3> sphereColors = {
    glm::vec3(1.0f, 0.5f, 1.0f), 

    glm::vec3(1.0f, 0.0f, 0.0f), 
    glm::vec3(1.0f, 0.0f, 0.0f), 
    glm::vec3(1.0f, 0.0f, 0.0f),  
    glm::vec3(1.0f, 0.0f, 0.0f),  

    glm::vec3(0.0f, 1.0f, 0.0f),  

    glm::vec3(0.0f, 1.0f, 0.0f),  
    glm::vec3(0.0f, 1.0f, 0.0f),  
    glm::vec3(0.0f, 1.0f, 0.0f),  

    glm::vec3(0.0f, 1.0f, 0.0f),  

    glm::vec3(0.0f, 1.0f, 0.0f), 
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(1.0f, 1.0f, 0.0f), 

    glm::vec3(1.0f, 0.8f, 0.4f),  
    glm::vec3(0.4f, 0.2f, 0.8f), 

    glm::vec3(1.0f, 1.0f, 0.0f), 

    glm::vec3(1.0f, 0.8f, 0.4f), 
    glm::vec3(0.4f, 0.2f, 0.8f)   
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void update_SpherePos_noAlign(Frame &ref_frame, Frame &inp_frame) {
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions[i].x = ref_frame.joint_translations[i].x - 0.5;
        ref_spherePositions[i].y = ref_frame.joint_translations[i].y;
        ref_spherePositions[i].z = ref_frame.joint_translations[i].z;

        input_spherePositions[i].x = inp_frame.joint_translations[i].x + 1.0f;
        input_spherePositions[i].y = inp_frame.joint_translations[i].y;
        input_spherePositions[i].z = inp_frame.joint_translations[i].z;
    }
}

void draw_objects(glm::mat4 &projection, glm::mat4 &view, Sphere &sphere, Shader &sphereShader) {
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

void update_SpherePos_Aligned(std::vector<Frame> &input_frames, std::vector<Frame> &ref_frames, int mapping) {
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

GLFWwindow* init_window(UIContext *context) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Create a GLFW window
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

ImGuiIO& init_imgui(GLFWwindow *window) {
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
