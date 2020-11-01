#include "glew.h"

#include <string>
#include <vector>

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include "Shader.h"
#include "Camera.h"
#include "LightCube.h"

const float cubeVertices[] =
{
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f
};

const GLushort cubeIndex[] =
{
	//front
	0, 1, 3,
	0, 3, 2,
	//right
	1, 5, 7,
	1, 7, 3,
	//back
	5, 4, 6,
	5, 6, 7,
	//left
	4, 0, 2,
	4, 2, 6,
	//top
	4, 5, 1,
	4, 1, 0,
	//bottom
	2, 3, 7,
	2, 7, 6
};


	LightCube::LightCube()
		:
		vertexFilePath("D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\LightCube.vert"),
		fragmentFilePath("D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\LightCube.frag"),
		pos(0.0f,0.0f,-10.0f)
	{
		//Init();
	}
	void LightCube::Init()
	{
		std::vector<GLenum> types{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
		std::vector<std::string> filepaths{ vertexFilePath, fragmentFilePath };

		program = Shader(types, filepaths).GetHandle();

		transMatrix = glGetUniformLocation(program, "transformationMat");

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndex), cubeIndex, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		/*glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const void*)(8 * 3 * sizeof(float)));*/
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void LightCube::Draw(Camera* inCamera)
	{
		glm::mat4 gpuMatrix(1.0f);

		glm::mat4 modelToWorld(1.0f);

		modelToWorld[3].x = pos.x;
		modelToWorld[3].y = pos.y;
		modelToWorld[3].z = pos.z;

		gpuMatrix = inCamera->cameraToClip * inCamera->DetermineWorldToCamera() * modelToWorld * gpuMatrix;

		glUseProgram(program);
		glBindVertexArray(vertexArray);
		glUniformMatrix4fv(transMatrix, 1, GL_FALSE, glm::value_ptr(gpuMatrix));
		glDrawElements(GL_TRIANGLES, sizeof(cubeIndex) / sizeof(cubeIndex[0]), GL_UNSIGNED_SHORT, 0);
		glUseProgram(0);
	}
	LightCube::~LightCube()
	{

	}

