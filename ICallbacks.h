#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include <list>

class ICallbacks
{
public:
	virtual void RenderSceneCB() = 0;
	virtual void IdleCB() = 0;
	virtual void KeyboardCB(unsigned char key, int x, int y) = 0;
};

