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

std::vector<glm::vec3> spherePositions;
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

void update_SpherePositions(Frame &frame) {
    for (int i = 0; i < JOINT_COUNT; i++) {
        spherePositions[i].x = frame.joint_translations[i].x;
        spherePositions[i].y = frame.joint_translations[i].y;
        spherePositions[i].z = frame.joint_translations[i].z;
    }
}


int display(std::vector<Frame> &frames, std::vector<glm::vec3> &spherePositions, const std::vector<glm::vec3> &sphereColors) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Motor Action Visualizer", nullptr, nullptr);
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
    Shader sphereShader(vertexShaderPath, fragmentShaderPath);
    // Create a sphere
    Sphere sphere(0.04f, 256, 128);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    glm::vec3 min_pos = spherePositions[0];
    glm::vec3 max_pos = spherePositions[0];
    
    for (const auto& pos : spherePositions) {
        if (pos.x < min_pos.x) min_pos.x = pos.x;
        if (pos.y < min_pos.y) min_pos.y = pos.y;
        if (pos.z < min_pos.z) min_pos.z = pos.z;

        if (pos.x > max_pos.x) max_pos.x = pos.x;
        if (pos.y > max_pos.y) max_pos.y = pos.y;
        if (pos.z > max_pos.z) max_pos.z = pos.z;
    }
    glm::vec3 center = (min_pos + max_pos) / 2.0f;
    int current_frame = 0;
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &WIDTH, &HEIGHT);
        aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view and projection matrices
        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);

        // Draw spheres
        sphereShader.use();
        
        // Set uniform values
        sphereShader.setUniformMat4("view", view);
        sphereShader.setUniformMat4("projection", projection);
        sphereShader.setUniformVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
        sphereShader.setUniformVec3("viewPos", glm::vec3(3.0f, 3.0f, 3.0f));
        sphereShader.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        update_SpherePositions(frames[current_frame++ % frames.size()]);
        
        for (size_t i = 0; i < spherePositions.size(); i++) {
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
        glUseProgram(0);  // Unbind the line shader program
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

    std::cout << reference_frames[0] << std::endl;
    //update_SpherePositions(reference_frames[0]);
    display(input_frames, spherePositions, sphereColors);

    delete input;
    delete input_trajectories;
    delete reference_trajectories;
    return 0;
}
