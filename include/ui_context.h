#ifndef UI_CONTEXT_H
#define UI_CONTEXT_H

#include <imgui.h>
#include <glm/glm.hpp>
#include <string>
#include "trajectory_analysis.h"

constexpr float ar = 16.0f / 9.0f;

class UIContext {
public:
    UIContext() : 
    camera_pos({glm::vec3(2.0f, 2.0f, 2.0f)}), 
    aspectRatio(ar),
    near(0.1f),
    far(100.0f), 
    clear_color({0.45f, 0.55f, 0.60f, 1.00f}), 
    fov(45.0f), 
    windowHeight(720), 
    windowWidth(1280), 
    center({0.0f,1.0f, 0.0f}) {}
    // Getters
    ImVec4 getClearColor() const { return clear_color; }
    glm::vec3 getCenter() const { return center; }
    glm::vec3 getCameraPos() const { return camera_pos; }
    glm::vec3 getCameraOrientation() const { return camera_orientation; }

    float getAspectRatio() const { return aspectRatio; }
    float getFov() const { return fov; }
    float getProjectionAngle() const { return projection_angle; }
    float getNear() const { return near; }
    float getFar() const { return far; }

    int getWindowWidth() const { return windowWidth; }
    int getWindowHeight() const { return windowHeight; }

    std::string getInputFile() const { return input_file; }
    std::string getReferenceFile() const { return reference_file; }

    Distances getDistFunc() const { return dist_func; }
    Trajectoy_analysis* getAnalysis() const { return analysis; }

    // Setters
    void setClearColor(ImVec4 &color) { clear_color = color; }
    void setCenter(glm::vec3 &cen) { center = cen; }
    void setCameraPos(glm::vec3 &pos) { camera_pos = pos; }
    void setCameraOrientation(glm::vec3 &orient) { camera_orientation = orient; }

    void setAspectRatio(float ratio) { aspectRatio = ratio; }
    void setFov(float field_of_view) { fov = field_of_view; }
    void setProjectionAngle(float angle) { projection_angle = angle; }
    void setNear(float near_plane) { near = near_plane; }
    void setFar(float far_plane) { far = far_plane; }

    void setWindowWidth(int width) { windowWidth = width; }
    void setWindowHeight(int height) { windowHeight = height; }

    void setInputFile(std::string &file) { input_file = file; }
    void setReferenceFile(std::string &file) { reference_file = file; }

    void setDistFunc(Distances func) { dist_func = func; }
    void setAnalysis(Trajectoy_analysis *analy) { analysis = analy; }

private:
    ImVec4 clear_color;
    glm::vec3 center;
    glm::vec3 camera_pos;
    glm::vec3 camera_orientation;

    float aspectRatio;
    float fov;
    float projection_angle;
    float near, far;

    int windowWidth;
    int windowHeight;

    std::string input_file;
    std::string reference_file;

    Distances dist_func;
    Trajectoy_analysis *analysis;
};

#endif // UI_CONTEXT_H
