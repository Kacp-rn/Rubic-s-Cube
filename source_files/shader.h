#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <glm/glm.hpp>

class Shader
{
    public:
    unsigned int ID;
    private:
    unsigned int VertexShader;
    unsigned int FragmentShader;
    
    std::string frag_Path;
    std::string vert_Path;
    
    public:
    // * Cleans up shader program and compiled shader resources
    ~Shader();
    // ! Compiles vertex and fragment shaders from source files and creates shader program
    // * @param vertexShaderPath - path to vertex shader file, @param fragmentShaderPath - path to fragment shader file
    void compileShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    // * Assembles compiled shaders into final shader program and links them
    void createProgram();
    // * Activates this shader program for subsequent rendering commands
    void use();
    
//------------------------------------------------------------------------------------------------------------------------

    // * Sets boolean uniform variable in shader program
    // * @param name - uniform variable name, @param value - boolean value
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // * Sets integer uniform variable in shader program
    // * @param name - uniform variable name, @param value - integer value
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // * Sets floating point uniform variable in shader program
    // * @param name - uniform variable name, @param value - float value
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // * Sets 2D vector uniform variable in shader program (from glm::vec2)
    // * @param name - uniform variable name, @param value - vec2 value
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    // * Sets 2D vector uniform variable in shader program (from separate x, y values)
    // * @param name - uniform variable name, @param x - X component, @param y - Y component
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // * Sets 3D vector uniform variable in shader program (from glm::vec3)
    // * @param name - uniform variable name, @param value - vec3 value
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    // * Sets 3D vector uniform variable in shader program (from separate x, y, z values)
    // * @param name - uniform variable name, @param x - X component, @param y - Y component, @param z - Z component
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // * Sets 4D vector uniform variable in shader program (from glm::vec4)
    // * @param name - uniform variable name, @param value - vec4 value
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    // * Sets 4D vector uniform variable in shader program (from separate x, y, z, w values)
    // * @param name - uniform variable name, @param x - X component, @param y - Y component, @param z - Z component, @param w - W component
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // * Sets 2x2 matrix uniform variable in shader program
    // * @param name - uniform variable name, @param mat - mat2 value
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // * Sets 3x3 matrix uniform variable in shader program
    // * @param name - uniform variable name, @param mat - mat3 value
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // * Sets 4x4 matrix uniform variable in shader program (typically model/view/projection matrix)
    // * @param name - uniform variable name, @param mat - mat4 value
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    };