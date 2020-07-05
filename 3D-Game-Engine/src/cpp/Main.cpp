#include <Windows.h>

#include "glew.h"
#include "glfw3.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>

GLuint programHandle;
GLFWwindow* window;

inline std::chrono::duration<double> elapsedTimeInSeconds(std::chrono::steady_clock::time_point firstTime, std::chrono::steady_clock::time_point secondTime)
{
    return secondTime - firstTime;
}

//copy pasted debug callback from khronos.org
void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

namespace ShaderManager
{
    //TODO: Binary cache of program once it's linked in order to speed up GPU reupload speed
    //https://www.khronos.org/opengl/wiki/Shader_Compilation#Binary_upload

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

}

void initOpenGL()
{
    if (!glfwInit())
    {
        throw std::runtime_error::runtime_error("glfw failed to initialize");
    }
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error::runtime_error("glfw failed to create Windows window");
    }
    glfwMakeContextCurrent(window);
    if (GLEW_OK != glewInit())
    {
        throw std::runtime_error::runtime_error("Glew failed to initialize");
    }

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif
}

void initShaders()
{
    //Compile Shaders
    std::vector<GLuint> shaderHandles;
    try
    {
        shaderHandles.push_back(ShaderManager::compileShader(GL_VERTEX_SHADER, "D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\VertexShader.vert"));
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        __debugbreak();
        throw;
    }

    try
    {
        shaderHandles.push_back(ShaderManager::compileShader(GL_FRAGMENT_SHADER, "D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\FragmentShader.frag"));
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        __debugbreak();
        throw;
    }
    //Link Shaders
    try
    {
        programHandle = ShaderManager::linkProgram(shaderHandles);
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        __debugbreak();
        throw;
    }
}

//called once before main loop
void init()
{
    initOpenGL();
    initShaders();
}

//called every loop
void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(void)
{
    try
    {
        init();
    }
    catch(std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        draw();
    }
    glfwTerminate();
    return 0;
}