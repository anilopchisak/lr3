#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <list>
#include "Technique.h"
#include "Pipeline.h"

static const char* vertex = R"(
	#version 330 core

	layout (location = 0) in vec3 Pos;
	layout (location = 1) in vec2 TexCoord;
	layout (location = 2) in vec3 Normal;

	uniform mat4 gWorld;
	uniform mat4 gWVP;
	out vec2 TexCoord0;
	out vec3 Normal0;

	void main()
	{
		gl_Position = gWVP * vec4(4 * Pos.x, 4 * Pos.y, Pos.z, 1.0);
		TexCoord0 = TexCoord; 
		Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;
	})";

static const char* fragment = R"(
	#version 330 core

	in vec2 TexCoord0;
	in vec3 Normal0; 
	out vec4 FragColor;

	struct DirectionalLight
	{
		vec3 Color;
		float AmbientIntensity;
		vec3 Direction;
		float DiffuseIntensity;
	};

	uniform sampler2D gSampler;
	uniform DirectionalLight gDirectionalLight;  

	void main()
	{
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *
                        gDirectionalLight.AmbientIntensity;

    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);

    vec4 DiffuseColor;

    if (DiffuseFactor > 0)
	{
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *
                       gDirectionalLight.DiffuseIntensity *
                       DiffuseFactor;
    }
    else
	{
        DiffuseColor = vec4(0,0,0,0);
    }

    FragColor = texture2D(gSampler, TexCoord0.xy) *
                (AmbientColor + DiffuseColor); 
	})";

struct DirectionLight
{
	glm::vec3 Color;
	float AmbientIntensity;
	glm::vec3 Direction;
	float DiffuseIntensity;
};

struct DirLightLocation
{
	GLuint Color;
	GLuint AmbientIntensity;
	GLuint Direction;
	GLuint DiffuseIntensity;
};


class LightingTechnique : public Technique
{
private:
	GLuint gWorldLocation;
	GLuint gSamplerLocation;
	GLuint gWVPLocation;
	DirLightLocation dirLightLocation;

public:
	LightingTechnique() { }

	virtual bool Init() override
	{
		if (!Technique::Init()) return false;
		if (!createShaders(vertex, fragment)) return false;

		gWorldLocation = GetUniformLocation("gWorld");
		gWVPLocation = GetUniformLocation("gWVP");
		gSamplerLocation = GetUniformLocation("gSampler");
		dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Color");
		dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.AmbientIntensity");
		dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
		dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.DiffuseIntensity");

		return true;
	}

	void SetWorld(glm::mat4* value)
	{
		glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)value);
	}

	void SetWVP(glm::mat4* value)
	{
		glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)value);
	}

	void SetTextureUnit(unsigned int TextureUnit)
	{
		glUniform1i(gSamplerLocation, TextureUnit);
	}

	void SetDirectionalLight(DirectionLight& Light)
	{
		glUniform3f(dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
		glUniform1f(dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
		glm::vec3 Direction = Light.Direction;
		glm::normalize(Direction);
		glUniform3f(dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
		glUniform1f(dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
	}
};

