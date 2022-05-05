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

	Vertex(glm::vec3 pos, glm::vec2 tex)
	{
		m_pos = pos;
		m_tex = tex;
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
	DirectionLight directionalLight;

public:
	Main()
	{
		Scale = 0.0f;
		pTexture = nullptr;
		pEffect = nullptr;
		directionalLight = { {0.9f, 0.9f, 0.9f}, 0.5f };
		//directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
		//directionalLight.AmbientIntensity = 0.5f;
	}

	~Main()
	{
		delete pTexture;
		delete pEffect;
	}

	bool Init()
	{
		CreateVertexBuffer();
		CreateIndexBuffer();

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
		directionalLight.AmbientIntensity += 0.0005f;

		Pipeline p;

		p.Scale(0.1f, 0.1f, 0.1f);
		p.WorldPos(0.0f, 0.0f, 3.0f);
		p.Rotate(0, Scale, 0);

		glm::vec3 CameraPos(0.0f, 0.0f, -3.0f);
		glm::vec3 CameraTarget(0.0f, 0.0f, 2.0f);
		glm::vec3 CameraUp(0.0f, 1.0f, 0.0f);
		p.SetCamera(CameraPos, CameraTarget, CameraUp);
		p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);
		pEffect->SetWVP(p.GetTrans());
		pEffect->SetDirectionalLight(directionalLight);

		// Rendering
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1); // Enable or disable the shared array of vertex attributes
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer, prepare it for rendering
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // data inside the buffer
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		pTexture->Bind(GL_TEXTURE0);

		glDrawArrays(GL_TRIANGLES, 0, 12);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

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
	void CreateVertexBuffer()
	{
		Vertex Vertices[4] =
		{
			Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
			Vertex(glm::vec3(0.0f, -1.0f, 5.0f), glm::vec2(0.5f, 0.0f)),
			Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
			Vertex(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(0.5f, 1.0f)),
		};
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	}

	void CreateIndexBuffer()
	{
		unsigned int Indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 2, 1 };

		// buffers (index array)
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	}
};

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv);
	GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL tutors");
	Magick::InitializeMagick(nullptr);

	Main* MainProgram = new Main();
	if (!MainProgram->Init()) return 1;

	MainProgram->Run();

	delete MainProgram;

	return 0;
}