#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void unuse();
    void setUniformMat4(const std::string &name, const glm::mat4 &mat) const;
    void setUniformVec3(const std::string &name, const glm::vec3 &value) const;

private:
    GLuint ID;
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif // SHADER_H
