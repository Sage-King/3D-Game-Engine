
#include "ShaderManager.h"

namespace ShaderManager
{
	//TODO: Binary cache of program once it's linked in order to speed up GPU reupload speed
	//https://www.khronos.org/opengl/wiki/Shader_Compilation#Binary_upload


	GLuint compileShader(GLenum shaderType, const std::string& filepath)
	{
		GLuint shaderHandle;
		std::stringstream shaderText;
		std::ifstream shaderFile;

		shaderFile.open(filepath);

		if (shaderFile.is_open())
		{
			shaderHandle = glCreateShader(shaderType);
			std::string lineText;
			while (std::getline(shaderFile, lineText))
			{
				shaderText << lineText << "\n";
			}
		}
		else
		{
			std::cout << "The shader text could not be opened." << std::endl;
			throw std::runtime_error::runtime_error("The shader text could not be opened.");
		}
		std::string tempString = shaderText.str();
		const char* shaderSource = tempString.c_str();
		glShaderSource(shaderHandle, 1, &shaderSource, NULL);
		glCompileShader(shaderHandle);
		try
		{
			testShaderCompilation(shaderHandle);
		}
		catch (std::runtime_error e)
		{
			std::cout << "Shader compilation failed." << std::endl;
			throw std::runtime_error::runtime_error(e.what());
		}
		shaderFile.close();
		return shaderHandle;
	}

	GLuint linkProgram(std::vector<GLuint> shaderHandles)
	{
		GLuint programHandle = glCreateProgram();

		for (const auto& shaderHandle : shaderHandles)
		{
			glAttachShader(programHandle, shaderHandle);
		}

		glLinkProgram(programHandle);

		for (const auto& shaderHandle : shaderHandles)
		{
			glDetachShader(programHandle, shaderHandle);
			glDeleteShader(shaderHandle);
		}

		try
		{
			testProgramLinkage(programHandle);
		}
		catch (std::runtime_error e)
		{
			std::cout << "Program linkage failed." << std::endl;
			throw std::runtime_error::runtime_error(e.what());
		}
		return programHandle;
	}

	void testShaderCompilation(GLuint shaderHandle)
	{
		GLint compileStatus = 0;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == GL_FALSE)
		{
			GLint logSize = 0;
			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logSize);
			GLchar* exceptionMessage = (char*)_malloca(logSize * sizeof(char));
			glGetShaderInfoLog(shaderHandle, logSize, NULL, exceptionMessage);

			glDeleteShader(shaderHandle);
			throw std::runtime_error::runtime_error("OpenGL Shader Compilation Failed : " + std::string(exceptionMessage));
		}
	}

	void testProgramLinkage(GLuint programHandle)
	{
		GLint linkStatus = 0;
		glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == GL_FALSE)
		{
			GLint logSize = 0;
			glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logSize);
			GLchar* exceptionMessage = (char*)_malloca(logSize * sizeof(char));
			glGetProgramInfoLog(programHandle, logSize, NULL, exceptionMessage);

			glDeleteProgram(programHandle);

			throw std::runtime_error::runtime_error("OpenGL Program Linkage Failed : " + std::string(exceptionMessage));
		}
	}
}