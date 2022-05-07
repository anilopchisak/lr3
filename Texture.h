#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <Magick++.h>

class Texture
{
private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    Magick::Image* m_pImage;
    Magick::Blob m_blob;
public:
    Texture(GLenum TextureTarget, const std::string& FileName)
    {
        m_textureTarget = TextureTarget; // тип текстуры GL_TEXTURE_2D
        m_fileName = FileName;
        m_pImage = nullptr;
    }

    bool Load() // load the file and prepare the memory to load the file to OpenGL
    {
        try 
        {
            // upload the texture to the private memory
            m_pImage = new Magick::Image(m_fileName);
            // upload the image to the obj BLOB
            m_pImage->write(&m_blob, "RGBA");
            // BLOB stores encoded image so other apps could use it
        }
        catch (Magick::Error& Error) 
        {
            std::cout << "Error loading texture '" << m_fileName << "': " << Error.what() << std::endl;
            return false;
        }

        // generate the objs textures and upload them to the pointer to array of GLuint
        glGenTextures(1, &m_textureObj); // = glGenBuffers()
        glBindTexture(m_textureTarget, m_textureObj);
        // upload the main part of texture obj
        //             парам       качество         высота текстуры     ширина          |     источник входящих данных текстуры       |
        glTexImage2D(m_textureTarget, 0, GL_RGBA, m_pImage->columns(), m_pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
        // condition of sampler of the texture
        // увеличить или уменьшить текстуру для совпадения пропорций с труегольником
        glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // GL_NEAREST-фильтрацию, когда текстуры масштабируются с уменьшением масштаба
        // GL_LINEAR-фильтрацию для текстур, масштабируемых с увеличением масштаба

        return true;
    }

    // bind texture object and allow using texture module 
    // (make texture to be available in fragment shader)
    void Bind(GLenum TextureUnit) // gets module of texture GL_TEXTURE0, GL_TEXTURE1
    {
        glActiveTexture(TextureUnit);
        glBindTexture(m_textureTarget, m_textureObj);
    }
};

