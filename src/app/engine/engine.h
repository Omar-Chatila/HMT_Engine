#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "enums.h"
#include "trajectory_analysis_manager.h"
#include "Shader.h"
#include "Sphere.h"
#include "line.h"
#include "imgui_layers.h"
#include "layers.h"

inline void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

class Renderer {
private:
    const char *vertexShaderPath = "src/shader/vertex_shader.glsl";
    const char *fragmentShaderPath = "src/shader/fragment_shader.glsl";
    const char *tex_vertexShaderPath = "src/shader/vertex_shader.glsl";
    const char *tex_fragmentShaderPath = "src/shader/textured_fragment_shader.glsl";
    
    std::vector<glm::vec3> ref_spherePositions;
    std::vector<glm::vec3> input_spherePositions;
    GLFWwindow *window;
    GLuint fbo1, fboTexture1, rbo1;
    GLuint fbo2, fboTexture2, rbo2;

    void update_SpherePos_noAlign(Frame &ref_frame, Frame &inp_frame);
    void draw_objects(glm::mat4 &projection, glm::mat4 &view, Sphere &sphere, Shader &sphereShader);
    void update_SpherePos_Aligned(std::vector<Frame> &input_frames, std::vector<Frame> &ref_frames, int mapping);
    GLFWwindow* init_window(UIContext *context);
    ImGuiIO& init_imgui(GLFWwindow *window);
    void init_fbo();
    void draw_scene(const std::vector<glm::vec3>& spherePositions, Sphere &sphere, Shader &sphereShader, UIContext *context);

public:
    Renderer();
    ~Renderer();
    int display();
};