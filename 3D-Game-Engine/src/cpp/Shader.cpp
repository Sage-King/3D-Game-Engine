#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <gtc/type_ptr.hpp>


    //TODO: Binary cache of program once it's linked in order to speed up GPU reupload speed
    //https://www.khronos.org/opengl/wiki/Shader_Compilation#Binary_upload


Shader::Shader(const std::vector<GLenum>& types, const std::vector<std::string>& filepaths)
    {
        m_handle = CompileAndLink(types, filepaths);
    }

void Shader::SetMat4(std::string uniformName, glm::mat4 inputMatrix)
{
    glUniformMatrix4fv(glGetUniformLocation(m_handle, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(inputMatrix));
}

void Shader::SetVec3(std::string uniformName, glm::vec3 inputVec)
{
    glUniform3fv(glGetUniformLocation(m_handle, uniformName.c_str()),3, glm::value_ptr(inputVec));
}

void Shader::SetVec3(std::string uniformName, float vecX, float vecY, float vecZ)
{
    SetVec3(uniformName, glm::vec3(vecX, vecY, vecZ));
}

void Shader::Bind()
    {
#ifdef _DEBUG
    static int debugNum = 0;
    if (m_handle == 0)
    {
        std::cout << "Error: You are using an uninitialized shader. \n |||This can happen because you declared a Shader and did not use a constructor (or used the default constructor).  \n Use Shader(const std::vector<GLenum>& types, const std::vector<std::string>& filepaths) for the general case. Other constructors are available, check Shader.h.|||\n " << std::endl;

        debugNum++;

        throw std::runtime_error::runtime_error("Error: You are using an uninitialized shader. Check your logs.");
    }
#endif // DEBUG

        glUseProgram(m_handle);
    }

void Shader::Unbind()
    {
        glUseProgram(0);
    }

Shader::~Shader()
{
    glDeleteProgram(m_handle);
    m_handle = 0;
}


//private functions

GLuint Shader::CompileAndLink(const std::vector<GLenum>& types, const std::vector<std::string>& filepaths)
{
    assert(types.size() != 0);
    assert(filepaths.size() != 0);
    assert(types.size() == filepaths.size());
    std::vector<GLuint> compiledShaders;
    for (unsigned int i = 0; i < types.size(); i++)
    {
        compiledShaders.push_back(compileShader(types.at(i), filepaths.at(i)));
    }
    return linkProgram(compiledShaders);
}

GLuint Shader::GenerateBinary(GLuint program)
{
    GLsizei bufsize = 32767; //number of bytes a 2 byte int can index; I think this might be a 4 byte int but I was too lazy to check and this is safer
    GLsizei length;
    GLenum binaryFormat;
    void* binary;
    glGetProgramBinary(program, bufsize, &length, &binaryFormat, &binary);
    glProgramBinary(program, binaryFormat, binary, length);
    return program;
}

void Shader::testShaderCompilation(GLuint shaderHandle)
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

void Shader::testProgramLinkage(GLuint programHandle)
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
            __debugbreak();
            throw std::runtime_error::runtime_error("OpenGL Program Linkage Failed : " + std::string(exceptionMessage));
        }
    }

GLuint Shader::compileShader(GLenum shaderType, const std::string& filepath)
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
            shaderFile.close();
            std::cout << "Shader compilation failed." << std::endl;
            throw std::runtime_error::runtime_error(e.what());
        }
        shaderFile.close();
        return shaderHandle;
    }

GLuint Shader::linkProgram(std::vector<GLuint> shaderHandles)
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
