#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <Magick++.h>
#include "Main.h"


int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv);
	GLUTBackendCreateWindow(1980, 1250, "OpenGL tutors");
	Magick::InitializeMagick(nullptr);

	Main* MainProgram = new Main();
	if (!MainProgram->Init()) return 1;

	MainProgram->Run();

	delete MainProgram;

	return 0;
}