#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/vec3.hpp>
#include <glew.h>

class Shader
{
public:
    GLuint Program;
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    void SetVector3(const GLchar* propertyName, const glm::vec3 value)
    {
        Use();
        auto propertyId = glGetUniformLocation(Program, propertyName);
        glUniform3f(propertyId, value.x, value.y, value.z);
    }
    void SetFloat(const GLchar* propertyName, const GLfloat value)
    {
        Use();
        auto propertyId = glGetUniformLocation(Program, propertyName);
        glUniform1f(propertyId, value);
    }
    void SetInt(const GLchar* propertyName, const GLint value)
    {
        Use();
        auto propertyId = glGetUniformLocation(Program, propertyName);
        glUniform1i(propertyId, value);
    }
    // Использование программы
    void Use();
};

#endif