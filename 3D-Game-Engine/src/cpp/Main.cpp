#include <Windows.h>

#include "glew.h"
#include "glfw3.h"

#include "DependencyInit.h"
#include "ShaderManager.h"

#include <iostream>

//TODO: Vertex Buffers & Related Arrays

int main(void)
{
    GLFWwindow* window;
    try
    {
        window = initDependencies();
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        __debugbreak();
        return -1;
    }


    #ifdef _DEBUG
        enableOpenGLDebug();
    #endif // DEBUG
    
    std::vector<GLuint> shaderHandles;

    try
    {
        shaderHandles.push_back(ShaderManager::compileShader(GL_VERTEX_SHADER, "D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\VertexShader.shader"));
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        __debugbreak();
        return -1;
    }

    try
    {
        shaderHandles.push_back(ShaderManager::compileShader(GL_FRAGMENT_SHADER, "D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\FragmentShader.shader"));
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        __debugbreak();
        return -1;
    }
    
    GLuint programHandle;

    try
    {
        programHandle = ShaderManager::linkProgram(shaderHandles);
    }
    catch(std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        __debugbreak();
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(programHandle);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}