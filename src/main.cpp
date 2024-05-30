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

const char *vertexShaderPath = "src/shader/vertex_shader.glsl";
const char *fragmentShaderPath = "src/shader/fragment_shader.glsl";

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

int display() {
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
    Sphere sphere(0.05f, 128, 64);

    // Define sphere positions
    std::vector<glm::vec3> spherePositions = {
        glm::vec3(-1.5f, 0.0f, -1.5f),
        glm::vec3(0.4f, 0.6f, -1.5f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(-1.1f, 0.0f, -1.5f),
        glm::vec3(1.4f, 0.8f, -1.5f),
        glm::vec3(0.0f, 0.2f, 0.5f),
        glm::vec3(-0.9f, 0.1f, -1.3f),
        glm::vec3(0.5f, -0.3f, -1.2f),
        glm::vec3(0.25f, 0.0f, 1.5f)};

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
        shader.setUniformVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

        // Render spheres
        for (glm::vec3 position : spherePositions) {   
            glm::mat4 model = glm::mat4(1.0f);
            position.x += rand() % 10 / 10.0;
            position.y += rand() % 10 / 10.0;
            position.z += rand() % 10 / 10.0;
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
    //display();
    std::string file = R"(resources\fb_41_pre_splitted_1.txt)";
    std::cout << "File: " << file << std::endl;
    
    Input_parser* input = new Input_parser(file.c_str());
    std::vector<Frame> frames = input->get_frames();
    Frame first = frames[120];
    std::cout << first << std::endl;

    Trajectories* trajectories = new Trajectories(frames);
    auto positions = trajectories->get_positionsTrajectory(Joint::l_hip);
    for (int i = 0; i < trajectories->size(); i++) {
        std::cout << positions[i] << std::endl;
    }    

    delete input;
    delete trajectories;
    return 0;
}
