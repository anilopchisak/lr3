#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#pragma once

struct m_persProj
{
	float Width; float Height;
	float zNear; float zFar;
	float FOV;
};

struct m_camera
{
	glm::vec3 Pos;
	glm::vec3 Target;
	glm::vec3 Up;
};


class Pipeline
{
protected:
	glm::vec3 m_scale;
	glm::vec3 m_worldPos;
	glm::vec3 m_rotateInfo;
	glm::mat4 m_transformation;
	m_camera camera;
	m_persProj persproj;
public:

	Pipeline()
	{
		m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_worldPos = glm::vec3(0.0f, 0.0f, 0.0f);
		m_rotateInfo = glm::vec3(0.0f, 0.0f, 0.0f);
		m_transformation = glm::mat4{ 1.0f };
	}

	void Scale(float ScaleX, float ScaleY, float ScaleZ)
	{
		m_scale.x = ScaleX;
		m_scale.y = ScaleY;
		m_scale.z = ScaleZ;
	}

	void WorldPos(float x, float y, float z)
	{
		m_worldPos.x = x;
		m_worldPos.y = y;
		m_worldPos.z = z;
	}

	void Rotate(float RotateX, float RotateY, float RotateZ)
	{
		m_rotateInfo.x = RotateX;
		m_rotateInfo.y = RotateY;
		m_rotateInfo.z = RotateZ;
	}

	void SetPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar)
	{
		persproj.FOV = FOV;
		persproj.Width = Width;
		persproj.Height = Height;
		persproj.zNear = zNear;
		persproj.zFar = zFar;
	}

	void SetCamera(glm::vec3& Pos, glm::vec3& Target, glm::vec3& Up)
	{
		camera.Pos = Pos;
		camera.Target = Target;
		camera.Up = Up;
	}

	glm::mat4* GetTrans()
	{
		glm::mat4 ScaleTrans, RotateTrans, TranslationTrans, PersProjTrans, CameraTranslationTrans, CameraRotateTrans;

		ScaleTrans = InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
		RotateTrans = InitRotateTransform(m_rotateInfo.x, m_rotateInfo.y, m_rotateInfo.z);
		TranslationTrans = InitTranslationTransform(m_worldPos.x, m_worldPos.y, m_worldPos.z);
		PersProjTrans = InitPerspectiveProj(persproj.Width, persproj.Height, persproj.zNear, persproj.zFar, persproj.FOV);
		CameraTranslationTrans = InitTranslationTransform(-camera.Pos.x, -camera.Pos.y, -camera.Pos.z);
		CameraRotateTrans = InitCameraTransform(camera.Target, camera.Up);
		m_transformation = ScaleTrans * RotateTrans * TranslationTrans * CameraTranslationTrans * CameraRotateTrans * PersProjTrans;
		return &m_transformation;
	};

protected:
	glm::mat4 InitScaleTransform(float scale_x, float scale_y, float scale_z)
	{
		glm::mat4 m;

		m[0][0] = scale_x;  m[0][1] = 0.0f;		m[0][2] = 0.0f;		 m[0][3] = 0.0f;
		m[1][0] = 0.0f;		m[1][1] = scale_y;	m[1][2] = 0.0f;		 m[1][3] = 0.0f;
		m[2][0] = 0.0f;		m[2][1] = 0.0f;		m[2][2] = scale_z;	 m[2][3] = 0.0f;
		m[3][0] = 0.0f;		m[3][1] = 0.0f;		m[3][2] = 0.0f;		 m[3][3] = 1.0f;

		return m;
	}

	glm::mat4 InitRotateTransform(float rotateInfo_x, float rotateInfo_y, float rotateInfo_z)
	{
		glm::mat4 m_x, m_y, m_z, m;
		float x = glm::radians(rotateInfo_x);
		float y = glm::radians(rotateInfo_y);
		float z = glm::radians(rotateInfo_z);

		// X
		m_x[0][0] = 1.0f;	m_x[0][1] = 0.0f;		m_x[0][2] = 0.0f;		m_x[0][3] = 0.0f;
		m_x[1][0] = 0.0f;	m_x[1][1] = cosf(x);	m_x[1][2] = -sinf(x);	m_x[1][3] = 0.0f;
		m_x[2][0] = 0.0f;	m_x[2][1] = sinf(x);	m_x[2][2] = cosf(x);	m_x[2][3] = 0.0f;
		m_x[3][0] = 0.0f;	m_x[3][1] = 0.0f;		m_x[3][2] = 0.0f;		m_x[3][3] = 1.0f;

		// Y
		m_y[0][0] = cosf(y);	m_y[0][1] = 0.0f;	m_y[0][2] = -sinf(y);	m_y[0][3] = 0.0f;
		m_y[1][0] = 0.0f;		m_y[1][1] = 1.0f;	m_y[1][2] = 0.0f;		m_y[1][3] = 0.0f;
		m_y[2][0] = sinf(y);	m_y[2][1] = 0.0f;	m_y[2][2] = cosf(y);	m_y[2][3] = 0.0f;
		m_y[3][0] = 0.0f;		m_y[3][1] = 0.0f;	m_y[3][2] = 0.0f;		m_y[3][3] = 1.0f;

		// Z
		m_z[0][0] = cosf(z);	m_z[0][1] = -sinf(z);	m_z[0][2] = 0.0f;	m_z[0][3] = 0.0f;
		m_z[1][0] = sinf(z);	m_z[1][1] = cosf(z);	m_z[1][2] = 0.0f;	m_z[1][3] = 0.0f;
		m_z[2][0] = 0.0f;		m_z[2][1] = 0.0f;		m_z[2][2] = 1.0f;	m_z[2][3] = 0.0f;
		m_z[3][0] = 0.0f;		m_z[3][1] = 0.0f;		m_z[3][2] = 0.0f;	m_z[3][3] = 1.0f;

		m = m_x * m_y * m_z;

		return m;
	}

	glm::mat4 InitTranslationTransform(float worldPos_x, float worldPos_y, float worldPos_z)
	{
		glm::mat4 m;

		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = worldPos_x;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = worldPos_y;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = worldPos_z;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;

		return m;
	}

	glm::mat4 InitPerspectiveProj(float width, float height, float zNear, float zFar, float FOV)
	{
		glm::mat4 m;

		float ar = width / height;
		float zRange = zNear - zFar;
		float tanHalfFOV = tanf(glm::radians(FOV / 2.0f));

		m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;				 m[0][2] = 0.0f;					 m[0][3] = 0.0;
		m[1][0] = 0.0f;						m[1][1] = 1.0f / tanHalfFOV; m[1][2] = 0.0f;					 m[1][3] = 0.0;
		m[2][0] = 0.0f;						m[2][1] = 0.0f;				 m[2][2] = (-zNear - zFar) / zRange; m[2][3] = 2.0f * zFar * zNear / zRange;
		m[3][0] = 0.0f;						m[3][1] = 0.0f;				 m[3][2] = 1.0f;					 m[3][3] = 0.0;

		return m;
	}

	glm::mat4 InitCameraTransform(glm::vec3& Target, glm::vec3& Up)
	{
		glm::mat4 m;

		glm::vec3 N = Target;
		N = glm::normalize(N);
		glm::vec3 U = Up;
		U = glm::normalize(U);
		U = glm::cross(U, Target);
		glm::vec3 V = glm::cross(N, U);

		m[0][0] = U.x;  m[0][1] = U.y;  m[0][2] = U.z;  m[0][3] = 0.0f;
		m[1][0] = V.x;  m[1][1] = V.y;  m[1][2] = V.z;  m[1][3] = 0.0f;
		m[2][0] = N.x;  m[2][1] = N.y;  m[2][2] = N.z;  m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

		return m;
	}
};

