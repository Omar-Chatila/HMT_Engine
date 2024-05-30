#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Sphere.h"
#include "input_parser.h"
#include "trajectories.h"
#include "trajectory_analysis.h"

const char *vertexShaderPath = "src/shader/vertex_shader.glsl";
const char *fragmentShaderPath = "src/shader/fragment_shader.glsl";

std::vector<glm::vec3> spherePositions;
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

void update_SpherePositions(Frame &frame) {
    for (int i = 0; i < JOINT_COUNT; i++) {
        spherePositions[i].x = frame.joint_translations[i].x;
        spherePositions[i].y = frame.joint_translations[i].y;
        spherePositions[i].z = frame.joint_translations[i].z;
    }
}

int display(const std::vector<glm::vec3>& spherePositions, const std::vector<glm::vec3>& sphereColors) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Sphere", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Compile and link shaders
    Shader shader(vertexShaderPath, fragmentShaderPath);

    // Create a sphere
    Sphere sphere(0.04f, 128, 64);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    srand(0);
    // Render loop
    int iteration = 0;
    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        shader.use();

        // Set uniform values
        glm::mat4 view = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        shader.setUniformMat4("view", view);
        shader.setUniformMat4("projection", projection);

        // Set light properties
        shader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        shader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        shader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // Render spheres
        for (size_t i = 0; i < spherePositions.size(); i++) {
            glm::vec3 position = spherePositions[i];
            glm::vec3 color = sphereColors[i];
            
            shader.setUniformVec3("objectColor", color);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            shader.setUniformMat4("model", model);
            sphere.draw();
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup
    glfwTerminate();
    return 0;
}

int main() {
    for (int i = 0; i < JOINT_COUNT; i++) {
        spherePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    std::string input_file = R"(resources\fb_41_pre_splitted_1.txt)";
    Input_parser* input = new Input_parser(input_file.c_str());
    std::vector<Frame> input_frames = input->get_frames();
    const Trajectories* input_trajectories = new Trajectories(input_frames);

    std::string reference_file = R"(resources\expertise_01_single100_2_splitted_1.txt)";
    Input_parser* reference = new Input_parser(reference_file.c_str());
    std::vector<Frame> reference_frames = reference->get_frames();
    const Trajectories* reference_trajectories = new Trajectories(reference_frames);

    Trajectoy_analysis* analysis = new Trajectoy_analysis(*input_trajectories, *reference_trajectories);
    analysis->perform_DTW(Joint::l_hip, EUCLID);
    analysis->perform_EDR(Joint::l_hip, EUCLID, 3.0);

    Vec3D root_position = reference_frames[0].root_translation;
    std::cout << reference_frames[0] << std::endl;
    update_SpherePositions(reference_frames[0]);
    display(spherePositions, sphereColors);

    delete input;
    delete input_trajectories;
    delete reference_trajectories;
    return 0;
}
