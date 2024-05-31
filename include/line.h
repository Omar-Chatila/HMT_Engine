#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace glm;
using namespace std;

class Line {
    unsigned int VBO, VAO;
    vector<float> vertices;
    mat4 MVP;
public:
    int shaderProgram;
    vec3 startPoint;
    vec3 lineColor;
    vec3 endPoint;

    Line(vec3 start, vec3 end) {
        startPoint = start;
        endPoint = end;
        lineColor = vec3(1,1,1);
        MVP = mat4(1.0f);

        const char *vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 MVP;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec3 color;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(color, 1.0f);\n"
            "}\n\0";

        // vertex shader
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        }

        // fragment shader
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        }

        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        vertices = {
             start.x, start.y, start.z,
             end.x, end.y, end.z,
        };
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
    }

    int setLineWidth(float width) {
        return 1;
    }

    int setMVP(mat4 mvp) {
        MVP = mvp;
        return 1;
    }

    int setColor(vec3 color) {
        lineColor = color;
        return 1;
    }

    int draw() {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, value_ptr(MVP));
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, value_ptr(lineColor));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        return 1;
    }

    ~Line() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
};