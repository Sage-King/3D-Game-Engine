#pragma once

#include "glew.h"
#include "glfw3.h"

#include <string>
#include <cstring>
#include <iostream>
#include <malloc.h>
#include <fstream>
#include <vector>
#include <sstream>

namespace ShaderManager 
{
	void testShaderCompilation(GLuint shaderHandle);
	void testProgramLinkage(GLuint programHandle);
	GLuint compileShader(GLenum shaderType, const std::string& filepath);
	GLuint linkProgram(std::vector<GLuint> shaderHandles);
}
