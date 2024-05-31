#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

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

int WIDTH = 800;
int HEIGHT = 600;
float aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

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

void update_SpherePositions(Frame &ref_frame, Frame &inp_frame) {
    for (int i = 0; i < JOINT_COUNT; i++) {
        ref_spherePositions[i].x = ref_frame.joint_translations[i].x;
        ref_spherePositions[i].y = ref_frame.joint_translations[i].y;
        ref_spherePositions[i].z = ref_frame.joint_translations[i].z;

        input_spherePositions[i].x = inp_frame.joint_translations[i].x + 1.0f;
        input_spherePositions[i].y = inp_frame.joint_translations[i].y;
        input_spherePositions[i].z = inp_frame.joint_translations[i].z;
    }
}

GLFWwindow* intit_window() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Create a GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Motor Action Visualizer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }

    return window;
}