#pragma once

#include "glew.h"
#include <string>

class LightCube
{
public:
	LightCube();
	void Init();
	void Draw(Camera* inCamera);
	~LightCube();
private:
	std::string vertexFilePath, fragmentFilePath;
	GLuint vertexBuffer, indexBuffer, vertexArray, transMatrix, program;

	glm::vec3 pos;
};