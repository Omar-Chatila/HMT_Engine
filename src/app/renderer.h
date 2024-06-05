#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "display_helper.h"
#include "util.h"
#include "shader.h"
#include "sphere.h"
#include "ui_context.h"
#include "imgui_layers.h"

class Renderer {
public:
    Renderer(UIContext* context);
    ~Renderer();

    void initializePositions();
    void updateSpherePosNoAlign(Frame& ref_frame, Frame& inp_frame);
    void updateSpherePosAligned(std::vector<Frame>& ref_frames, std::vector<Frame>& input_frames, int mapping);
    void drawObjects(glm::mat4& projection, glm::mat4& view, Sphere& sphere, Shader& sphereShader);
    void renderLoop(std::vector<Frame>& ref_frames, std::vector<Frame>& inp_frames, std::tuple<float, std::vector<int>, float*>& alignment);

private:
    void updateSpherePos(Frame& frame, std::vector<glm::vec3>& spherePositions, float x_offset);
    void drawSpheres(glm::mat4& projection, glm::mat4& view, Sphere& sphere, Shader& sphereShader, std::vector<glm::vec3> &spherePositions);
    void drawBones(glm::mat4& projection, glm::mat4& view, std::vector<glm::vec3> &spherePositions);
    GLFWwindow* initWindow(UIContext* context);
    ImGuiIO& initImGui(GLFWwindow* window);

    std::vector<glm::vec3> ref_spherePositions;
    std::vector<glm::vec3> input_spherePositions;
    GLFWwindow* window;
    UIContext* context;
    ImGuiIO* io;
    Shader* sphereShader;
    Sphere* sphere;
};

#endif
