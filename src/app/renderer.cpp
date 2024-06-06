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
    
}

Renderer::~Renderer() {
    
}

GLFWwindow* Renderer::initWindow(UIContext* context) {
    
}

ImGuiIO& Renderer::initImGui(GLFWwindow* window) {
    
}

void Renderer::initializePositions() {
    
}

void Renderer::updateSpherePos(Frame& frame, std::vector<glm::vec3>& spherePositions, float x_offset) {
    
}

void Renderer::updateSpherePosNoAlign(Frame& ref_frame, Frame& inp_frame) {
    
}

void Renderer::updateSpherePosAligned(std::vector<Frame>& ref_frames, std::vector<Frame>& input_frames, int mapping) {
    
}

void Renderer::drawSpheres(glm::mat4& projection, glm::mat4& view, Sphere& sphere, Shader& sphereShader, std::vector<glm::vec3>& spherePositions) {
    
}

void Renderer::drawBones(glm::mat4& projection, glm::mat4& view, std::vector<glm::vec3>& spherePositions) {
    
}

void Renderer::drawObjects(glm::mat4& projection, glm::mat4& view, Sphere& sphere, Shader& sphereShader) {
    
}

void Renderer::renderLoop(std::vector<Frame>& ref_frames, std::vector<Frame>& inp_frames, std::tuple<float, std::vector<int>, float*>& alignment) {
    
}
 */