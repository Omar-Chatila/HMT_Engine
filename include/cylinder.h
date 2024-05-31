// Cylinder.h
#ifndef CYLINDER_H
#define CYLINDER_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Cylinder {
public:
    Cylinder(float radius, float height, int sectorCount);
    void draw();

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    void setupCylinder();
};

#endif