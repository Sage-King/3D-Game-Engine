#include <Windows.h>

#include "glew.h"
#include "glfw3.h"

#include "DependencyInit.h"
#include "ShaderManager.h"
#include "VertexBufferObject.h"

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
    //Compile Shaders
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
    //Link Shaders
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
    //init vbo and vao
    GLuint vbo = VertexManager::initVertexBuffer();
    GLuint vao = VertexManager::initVertexArray();
    

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 1.0f, 1.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(programHandle);
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(0);
        glUseProgram(0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}