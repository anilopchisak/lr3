#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <list>
#include "Technique.h"
#include "Pipeline.h"

struct DirectionLight
{
	glm::vec3 Color;
	float AmbientIntensity;
};

static const char* vertex = R"(
	#version 330 core

	layout (location = 0) in vec3 Pos;
	layout (location = 1) in vec2 TexCoord;

	uniform mat4 gWorld;
	out vec2 TexCoord0;

	void main()
	{
		gl_Position = gWorld * vec4(4 * Pos.x, 4 * Pos.y, Pos.z, 1.0);
		TexCoord0 = TexCoord; 
	})";

static const char* fragment = R"(
	#version 330 core

	in vec2 TexCoord0;
	out vec4 FragColor;

	struct DirectionalLight
	{
		vec3 Color;
		float AmbientIntensity;
	};

	uniform sampler2D gSampler;
	uniform DirectionalLight gDirectionalLight;  

	void main()
	{
		FragColor = texture2D(gSampler, TexCoord0.st) * vec4(gDirectionalLight.Color, 1.0f) *
					gDirectionalLight.AmbientIntensity;
	})";




class LightingTechnique : public Technique
{
private:
	GLuint gWorldLocation;
	GLuint gSamplerLocation;
	GLuint dirLightColorLocation;
	GLuint dirLightAmbientIntensityLocation;

public:
	LightingTechnique() { }

	virtual bool Init() override
	{
		if (!Technique::Init()) return false;
		if (!createShaders(vertex, fragment)) return false;

		gWorldLocation = GetUniformLocation("gWorld");
		gSamplerLocation = GetUniformLocation("gSampler");
		dirLightColorLocation = GetUniformLocation("gDirectionalLight.Color");
		dirLightAmbientIntensityLocation = GetUniformLocation("gDirectionalLight.AmbientIntensity");

		if (dirLightAmbientIntensityLocation == 0xFFFFFFFF || gWorldLocation == 0xFFFFFFFF ||
			gSamplerLocation == 0xFFFFFFFF || dirLightColorLocation == 0xFFFFFFFF)
		{
			return false;
		}

		return true;
	}

	void SetWVP(glm::mat4* value)
	{
		glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)value);
	}

	void SetTextureUnit(unsigned int TextureUnit)
	{
		glUniform1i(gSamplerLocation, TextureUnit);
	}

	void SetDirectionalLight(DirectionLight& Light)
	{
		glUniform3f(dirLightColorLocation, Light.Color.x, Light.Color.y, Light.Color.z);
		glUniform1f(dirLightAmbientIntensityLocation, Light.AmbientIntensity);
	}
};

