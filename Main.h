#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <Magick++.h>

#include "Pipeline.h"
#include "Texture.h"
#include "LightingTechnique.h"
#include "ICallbacks.h"

constexpr auto WINDOW_WIDTH = 1980;
constexpr auto WINDOW_HEIGHT = 1250;

struct Vertex
{
	glm::vec3 m_pos;
	glm::vec2 m_tex;
	glm::vec3 m_normal;

	Vertex(glm::vec3 pos, glm::vec2 tex)
	{
		m_pos = pos;
		m_tex = tex;
		m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
	}
};

static ICallbacks* callbacks = nullptr;
static void aRenderSceneCB() { callbacks->RenderSceneCB(); }
static void aIdleCB() { callbacks->IdleCB(); }
static void InitCallbacks()
{
	glutDisplayFunc(aRenderSceneCB); //GLUT interacts with window system
	glutIdleFunc(aIdleCB);
}
void GLUTBackendInit(int argc, char** argv)
{
	glutInit(&argc, argv); //initialize GLUT, pass parameters
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //setup of GLUT options
}
bool GLUTBackendCreateWindow(unsigned int Width, unsigned int Height, const char* pTitle)
{
	glutInitWindowSize(Width, Height); //window parameters
	glutInitWindowPosition(100, 100);
	glutCreateWindow(pTitle); //create the window and give it a title

	//initialize GLEW and check for errors
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		std::cerr << "Error: " << glewGetErrorString(res) << "\n";
		return 0;
	}
	return 1;
}
void GLUTBackendRun(ICallbacks* pCallbacks)
{
	if (!pCallbacks)
	{
		std::cerr << __FUNCTION__ << " : callbacks are not specified!\n";
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //setting the color of the window
	// image quality improvement
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	callbacks = pCallbacks;
	InitCallbacks();
	glutMainLoop(); //transferring control to GLUT, it's waiting for events in the window
}


class Main : public ICallbacks
{
private:
	GLuint VBO; // a global variable for storing a pointer to the vertex buffer
	GLuint IBO;
	float Scale;
	Texture* pTexture;
	LightingTechnique* pEffect;
	DirectionalLight directionalLight;

public:
	Main()
	{
		Scale = 0.0f;
		pTexture = nullptr;
		pEffect = nullptr;
		directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
		directionalLight.AmbientIntensity = 0.0f;
		directionalLight.DiffuseIntensity = 0.75f;
		directionalLight.Direction = glm::vec3(1.0f, 0.0, 0.0);
	}

	~Main()
	{
		delete pTexture;
		delete pEffect;
	}

	bool Init()
	{
		CreateBuffers();

		pTexture = new Texture(GL_TEXTURE_2D, "test.jpg");
		if (!pTexture->Load()) return false;

		pEffect = new LightingTechnique();
		if (!pEffect->Init()) return false;
		pEffect->Enable();
		pEffect->SetTextureUnit(0);

		return true;
	}

	void Run()
	{
		GLUTBackendRun(this);
	}

	virtual void RenderSceneCB() override //draw
	{
		glClear(GL_COLOR_BUFFER_BIT);
		//glClear(GL_COLOR_BUFFER_BIT); //clearing the frame buffer using the color specified above

		Scale += 0.1f;
		//directionalLight.AmbientIntensity += 0.0001f;

		Pipeline p;

		p.Scale(0.1f, 0.1f, 0.1f);
		p.WorldPos(0.0f, 0.0f, 3.0f);
		p.Rotate(0, Scale, 0);

		glm::vec3 CameraPos(0.0f, 0.0f, -3.0f);
		glm::vec3 CameraTarget(0.0f, 0.0f, 2.0f);
		glm::vec3 CameraUp(0.0f, 1.0f, 0.0f);
		p.SetCamera(CameraPos, CameraTarget, CameraUp);

		p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

		SpotLight sl[2];
		sl[0].DiffuseIntensity = 15.0f;
		sl[0].Color = glm::vec3(1.0f, 1.0f, 0.7f);
		sl[0].Position = glm::vec3(-0.0f, -1.9f, -0.0f);
		sl[0].Direction = glm::vec3(sinf(Scale), 0.0f, cosf(Scale));
		sl[0].Attenuation.Linear = 0.1f;
		sl[0].Cutoff = 20.0f;

		sl[1].DiffuseIntensity = 5.0f;
		sl[1].Color = glm::vec3(0.0f, 1.0f, 1.0f);
		sl[1].Position = CameraPos;
		sl[1].Direction = CameraTarget;
		sl[1].Attenuation.Linear = 0.1f;
		sl[1].Cutoff = 10.0f;

		pEffect->SetSpotLights(2, sl);

		PointLight pl[3];
		pl[0].DiffuseIntensity = 0.5f;
		pl[0].Color = glm::vec3(1.0f, 0.0f, 0.0f);
		pl[0].Position = glm::vec3(sinf(Scale) * 10, 1.0f, cosf(Scale) * 10);
		pl[0].Attenuation.Linear = 0.1f;

		pl[1].DiffuseIntensity = 0.5f;
		pl[1].Color = glm::vec3(0.0f, 1.0f, 0.0f);
		pl[1].Position = glm::vec3(sinf(Scale + 2.1f) * 10, 1.0f, cosf(Scale + 2.1f) * 10);
		pl[1].Attenuation.Linear = 0.1f;

		pl[2].DiffuseIntensity = 0.5f;
		pl[2].Color = glm::vec3(0.0f, 0.0f, 1.0f);
		pl[2].Position = glm::vec3(sinf(Scale + 4.2f) * 10, 1.0f, cosf(Scale + 4.2f) * 10);
		pl[2].Attenuation.Linear = 0.1f;

		pEffect->SetPointLights(3, pl);

		pEffect->SetWVP(p.GetWVPTrans());
		pEffect->SetWorld(p.GetWorldTrans());
		pEffect->SetDirectionalLight(directionalLight);

		pEffect->SetEyeWorldPos(CameraPos);
		pEffect->SetMatSpecularIntensity(1.0f);
		pEffect->SetMatSpecularPower(32);

		// Rendering
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1); // Enable or disable the shared array of vertex attributes
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer, prepare it for rendering
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // data inside the buffer
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		pTexture->Bind(GL_TEXTURE0);

		glDrawArrays(GL_TRIANGLES, 0, 12);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// indicates that the current window should be redrawn and during operation
		// of the main loop GLUT render function will be called
		glutPostRedisplay();

		glutSwapBuffers(); //swap the background buffer and the frame buffer
	}

	virtual void IdleCB()
	{
		RenderSceneCB();
	}

private:

	void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount) 
	{
		for (unsigned int i = 0; i < IndexCount; i += 3) 
		{
			unsigned int Index0 = pIndices[i];
			unsigned int Index1 = pIndices[i + 1];
			unsigned int Index2 = pIndices[i + 2];
			glm::vec3 v1 = pVertices[Index1].m_pos - pVertices[Index0].m_pos;
			glm::vec3 v2 = pVertices[Index2].m_pos - pVertices[Index0].m_pos;
			glm::vec3 Normal = v1 * v2;
			glm::normalize(Normal);

			pVertices[Index0].m_normal += Normal;
			pVertices[Index1].m_normal += Normal;
			pVertices[Index2].m_normal += Normal;
		}

		for (unsigned int i = 0; i < VertexCount; i++) 
		{
			glm::normalize(pVertices[i].m_normal);
		}
	}

	void CreateBuffers()
	{
		Vertex Vertices[4] =
		{
			Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(0.0f, -1.0f, 5.0f), glm::vec2(0.5f, 0.0f)),
			Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(0.5f, 1.0f)),
		};

		unsigned int Indices[] = {	0, 3, 1,
									1, 3, 2,
									2, 3, 0,
									0, 2, 1	 };

		CalcNormals(Indices, 12, Vertices, 4);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	}
};

