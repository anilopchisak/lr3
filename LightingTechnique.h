#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <list>
#include "Technique.h"
#include "Pipeline.h"

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 2;

static const char* vertex = R"(
	#version 330 core

	layout (location = 0) in vec3 Position;
	layout (location = 1) in vec2 TexCoord;
	layout (location = 2) in vec3 Normal;

	uniform mat4 gWorld;
	uniform mat4 gWVP;

	out vec2 TexCoord0;
	out vec3 Normal0;
	out vec3 WorldPos0;

	void main()
	{
		gl_Position = gWVP * vec4(4 * Position.x, 4 * Position.y, Position.z, 1.0);
		TexCoord0 = TexCoord; 
		Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;
		WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;
	})";

static const char* fragment = R"(
	#version 330                                                                        
																						
	const int MAX_POINT_LIGHTS = 3;
	const int MAX_SPOT_LIGHTS = 2;                                                     
																						
	in vec2 TexCoord0;                                                                  
	in vec3 Normal0;                                                                    
	in vec3 WorldPos0;                                                                  
																						
	out vec4 FragColor;                                                                 
																						
	struct BaseLight                                                                    
	{                                                                                   
		vec3 Color;                                                                     
		float AmbientIntensity;                                                         
		float DiffuseIntensity;                                                         
	};                                                                                  
																						
	struct DirectionalLight                                                             
	{                                                                                   
		BaseLight Base;                                                          
		vec3 Direction;                                                                 
	};                                                                                  
																						
	struct Attenuation                                                                  
	{                                                                                   
		float Constant;                                                                 
		float Linear;                                                                   
		float Exp;                                                                      
	};                                                                                  
																						
	struct PointLight                                                                           
	{                                                                                           
		BaseLight Base;                                                                  
		vec3 Position;                                                                          
		Attenuation Atten;                                                                      
	}; 

	struct SpotLight                                                                            
	{                                                                                           
		PointLight Base;                                                                 
		vec3 Direction;                                                                         
		float Cutoff;                                                                           
	};                                                                                         
																								
	uniform int gNumPointLights;                
	uniform int gNumSpotLights;                                                
	uniform DirectionalLight gDirectionalLight;                                                 
	uniform PointLight gPointLights[MAX_POINT_LIGHTS];           
	uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];                               
	uniform sampler2D gSampler;                                                                 
	uniform vec3 gEyeWorldPos;                                                                  
	uniform float gMatSpecularIntensity;                                                        
	uniform float gSpecularPower;                                                               
																								
	vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)            
	{                                                                                           
		vec4 AmbientColor = vec4(Light.Color, 1.0f) * Light.AmbientIntensity;                   
		float DiffuseFactor = dot(Normal, -LightDirection);                                     
																								
		vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
		vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
																								
		if (DiffuseFactor > 0) 
		{                                                                
			DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * DiffuseFactor;    
																								
			vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             
			vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
			float SpecularFactor = dot(VertexToEye, LightReflect);                              
			SpecularFactor = pow(SpecularFactor, gSpecularPower);                               
			if (SpecularFactor > 0) 
			{                                                           
				SpecularColor = vec4(Light.Color, 1.0f) *                                       
								gMatSpecularIntensity * SpecularFactor;                         
			}                                                                                   
		}                                                                                       
																								
		return (AmbientColor + DiffuseColor + SpecularColor);                                   
	}                                                                                           
																								
	vec4 CalcDirectionalLight(vec3 Normal)                                                      
	{                                                                                           
		return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal); 
	}                                                                                           
																								
	vec4 CalcPointLight(PointLight l, vec3 Normal)                                       
	{                                                                                           
		vec3 LightDirection = WorldPos0 - l.Position;                                           
		float Distance = length(LightDirection);                                                
		LightDirection = normalize(LightDirection);                                             
																								
		vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);                         
		float Attenuation =  l.Atten.Constant +                                                 
							 l.Atten.Linear * Distance +                                        
							 l.Atten.Exp * Distance * Distance;                                 
																								
		return Color / Attenuation;                                                             
	}      

	vec4 CalcSpotLight( SpotLight l, vec3 Normal)                                         
	{                                                                                           
		vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);                             
		float SpotFactor = dot(LightToPixel, l.Direction);                                      
																								
		if (SpotFactor > l.Cutoff) 
		{                                                            
			vec4 Color = CalcPointLight(l.Base, Normal);                                        
			return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   
		}                                                                                       
		else
		 {                                                                                  
			return vec4(0,0,0,0);                                                               
		}                                                                                       
	}                                                                                        
																								
	void main()                                                                                 
	{                                                                                           
		vec3 Normal = normalize(Normal0);                                                       
		vec4 TotalLight = CalcDirectionalLight(Normal);                                         
																								
		for (int i = 0 ; i < gNumPointLights ; i++) 
		{                                           
			TotalLight += CalcPointLight(gPointLights[i], Normal);                                            
		}                                                                                       
			
		for (int i = 0 ; i < gNumSpotLights ; i++)
		{                                            
				TotalLight += CalcSpotLight(gSpotLights[i], Normal);                                
		}       
																					
		FragColor = texture2D(gSampler, TexCoord0.xy) * TotalLight;

	})";

struct BaseLight
{
	glm::vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
	BaseLight()
	{
		Color = glm::vec3(0.0f, 0.0f, 0.0f);
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}
};

struct DirectionalLight : public BaseLight
{
	glm::vec3 Direction;
	DirectionalLight()
	{
		Direction = glm::vec3(0.0f, 0.0f, 0.0f);
	}
};

struct PointLight : public BaseLight
{
	glm::vec3 Position;
	struct
	{
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;
	PointLight()
	{
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Attenuation.Constant = 1.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0f;
	}
};

struct SpotLight : public PointLight
{
	glm::vec3 Direction;
	float Cutoff;

	SpotLight()
	{
		Direction = glm::vec3(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}
};

class LightingTechnique : public Technique
{
private:
	GLuint gWorldLocation;
	GLuint gSamplerLocation;
	GLuint gWVPLocation;

	GLuint dirLightColor;
	GLuint dirLightAmbientIntensity;
	GLuint dirLightDirection;
	GLuint dirLightDiffuseIntensity;

	GLuint eyeWorldPosition;
	GLuint matSpecularIntensityLocation;
	GLuint matSpecularPowerLocation;

	GLuint numPointLightsLocation;
	GLuint numSpotLightsLocation;

	struct 
	{
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        struct 
		{
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } pointLightsLocation[MAX_POINT_LIGHTS];

	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		GLuint Direction;
		GLuint Cutoff;
		struct {
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} spotLightsLocation[MAX_SPOT_LIGHTS];

public:
	LightingTechnique() { }

	virtual bool Init() override
	{
		if (!Technique::Init()) return false;
		if (!createShaders(vertex, fragment)) return false;

		gWorldLocation = GetUniformLocation("gWorld");
		gWVPLocation = GetUniformLocation("gWVP");
		gSamplerLocation = GetUniformLocation("gSampler");

		dirLightColor = GetUniformLocation("gDirectionalLight.Base.Color");
		dirLightAmbientIntensity = GetUniformLocation("gDirectionalLight.Base.AmbientIntensity");
		dirLightDirection = GetUniformLocation("gDirectionalLight.Direction");
		dirLightDiffuseIntensity = GetUniformLocation("gDirectionalLight.Base.DiffuseIntensity");

		eyeWorldPosition = GetUniformLocation("gEyeWorldPos");
		matSpecularIntensityLocation = GetUniformLocation("gMatSpecularIntensity");
		matSpecularPowerLocation = GetUniformLocation("gSpecularPower");

		numPointLightsLocation = GetUniformLocation("gNumPointLights");
		numSpotLightsLocation = GetUniformLocation("gNumSpotLights");

		for (unsigned int i = 0; i < MAX_POINT_LIGHTS; i++) 
		{
			char Name[128];
			memset(Name, 0, sizeof(Name));
			snprintf(Name, sizeof(Name), "gPointLights[%d].Base.Color", i);
			pointLightsLocation[i].Color = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gPointLights[%d].Base.AmbientIntensity", i);
			pointLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gPointLights[%d].Position", i);
			pointLightsLocation[i].Position = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gPointLights[%d].Base.DiffuseIntensity", i);
			pointLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Constant", i);
			pointLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Linear", i);
			pointLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Exp", i);
			pointLightsLocation[i].Atten.Exp = GetUniformLocation(Name);
		}
		for (unsigned int i = 0; i < MAX_SPOT_LIGHTS; i++) 
		{
			char Name[128];
			memset(Name, 0, sizeof(Name));
			snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Color", i);
			spotLightsLocation[i].Color = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Base.AmbientIntensity", i);
			spotLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Position", i);
			spotLightsLocation[i].Position = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Direction", i);
			spotLightsLocation[i].Direction = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Cutoff", i);
			spotLightsLocation[i].Cutoff = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Base.DiffuseIntensity", i);
			spotLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Constant", i);
			spotLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Linear", i);
			spotLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

			snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Exp", i);
			spotLightsLocation[i].Atten.Exp = GetUniformLocation(Name);
		}

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

	void SetDirectionalLight(DirectionalLight& Light)
	{
		glUniform3f(dirLightColor, Light.Color.x, Light.Color.y, Light.Color.z);
		glUniform1f(dirLightAmbientIntensity, Light.AmbientIntensity);
		glm::vec3 Direction = Light.Direction;
		glm::normalize(Direction);
		glUniform3f(dirLightDirection, Direction.x, Direction.y, Direction.z);
		glUniform1f(dirLightDiffuseIntensity, Light.DiffuseIntensity);
	}

	void SetMatSpecularIntensity(float Intensity)
	{
		glUniform1f(matSpecularIntensityLocation, Intensity);
	}

	void SetMatSpecularPower(float Power)
	{
		glUniform1f(matSpecularPowerLocation, Power);
	}

	void SetEyeWorldPos(const glm::vec3& EyeWorldPos)
	{
		glUniform3f(eyeWorldPosition, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
	}

	void SetPointLights(unsigned int NumLights, const PointLight* pLights)
	{
		glUniform1i(numPointLightsLocation, NumLights);

		for (unsigned int i = 0; i < NumLights; i++) {
			glUniform3f(pointLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
			glUniform1f(pointLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
			glUniform1f(pointLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
			glUniform3f(pointLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
			glUniform1f(pointLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
			glUniform1f(pointLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
			glUniform1f(pointLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
		}
	}

	void SetSpotLights(unsigned int NumLights, const SpotLight* pLights)
	{
		glUniform1i(numSpotLightsLocation, NumLights);

		for (unsigned int i = 0; i < NumLights; i++)
		{
			glUniform3f(spotLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
			glUniform1f(spotLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
			glUniform1f(spotLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
			glUniform3f(spotLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
			glm::vec3 Direction = pLights[i].Direction;
			glm::normalize(Direction);
			glUniform3f(spotLightsLocation[i].Direction, Direction.x, Direction.y, Direction.z);
			glUniform1f(spotLightsLocation[i].Cutoff, cosf(glm::radians(pLights[i].Cutoff)));
			glUniform1f(spotLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
			glUniform1f(spotLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
			glUniform1f(spotLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
		}
	}
};

