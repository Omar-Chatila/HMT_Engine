#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Sphere {
public:
    Sphere(float radius, int sectorCount, int stackCount);
    ~Sphere();
    void draw();
    
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    void generateSphere(float radius, int sectorCount, int stackCount);
    void setupBuffers();
};

#endif // SPHERE_H
