#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <list>

class Technique
{
private:
    GLuint ShaderProgram;
    GLint success;
    GLchar InfoLog[1024];

public:
    Technique() 
    {
        ShaderProgram = 0;
        InfoLog[1024] = { 0 };
        success = 0;
    }

    ~Technique() 
    {

        if (ShaderProgram != 0) 
        {
            glDeleteProgram(ShaderProgram);
            ShaderProgram = 0;
        }
    }

    virtual bool Init() 
    {
        ShaderProgram = glCreateProgram();
        if (ShaderProgram == 0) 
        {
            std::cerr << "Error creating shader program " << "\n";
            return 0;
        }
        return 1;
    }

    void Enable() 
    {
        glUseProgram(ShaderProgram);
    }

    GLint GetUniformLocation(const char* pUniformName)
    {
        GLint Location = glGetUniformLocation(ShaderProgram, pUniformName);

        if (Location == 0xFFFFFFFF)
            std::cerr << "Warning!Unable to get the location of uniform ' " << pUniformName << "'\n";

        return Location;
    }

protected:
    bool addshader(const char* ShaderText, GLenum ShaderType)
    {
        GLuint shader = glCreateShader(ShaderType);

        const GLchar* ShaderSource[1];
        ShaderSource[0] = ShaderText;
        glShaderSource(shader, 1, ShaderSource, nullptr);
        glCompileShader(shader);

        // Checking for vertex shader compilation errors
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!checkerror(shader, success, ShaderType)) return 0;

        glAttachShader(ShaderProgram, shader);

        return 1;
    }

    bool createShaders(const char* ShaderText_v, const char* ShaderText_f)
    {
        addshader(ShaderText_v, GL_VERTEX_SHADER);
        addshader(ShaderText_f, GL_FRAGMENT_SHADER);

        // Checking for shader binding errors
        glLinkProgram(ShaderProgram);
        glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
        if (!checkerror(ShaderProgram, success, -1)) return 0;

        glValidateProgram(ShaderProgram);
        if (!checkerror(ShaderProgram, success, -2)) return 0;

        return 1;
    }

    bool checkerror(GLuint program, GLint success, GLenum ShaderType)
    {
        if (!success)
        {
            if (ShaderType == -1)
            {
                glGetShaderInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
                std::cerr << "Error linking shader program " << InfoLog << "\n";
                return 0;
            }
            else if (ShaderType == -2)
            {
                glGetProgramInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
                std::cerr << "Invalid shader program " << InfoLog << "\n";
                return 0;
            }
            else
            {
                glGetProgramInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
                std::cerr << "Error compiling shader type " << static_cast<int>(ShaderType) << InfoLog << "\n";
                return 0;
            }
        }
        return 1;
    }
};

