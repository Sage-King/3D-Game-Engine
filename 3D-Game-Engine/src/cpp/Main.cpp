#include "glew.h"
#include "glfw3.h"

#include <chrono>
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    double oscRed = 0.0;
    double oscAmountPerSecond = 1;

    std::chrono::steady_clock::time_point oldFrameTime = std::chrono::steady_clock::now();

     while (!glfwWindowShouldClose(window))
     {
        //update
        //oscillate Red Color
        if (oscRed > 1.0)
        {
            oscRed = 1.0;
            oscAmountPerSecond *= -1;
        }
        else if (oscRed < 0.0)
        {
            oscRed = 0.0;
            oscAmountPerSecond *= -1;
        }
        //end oscillate Red Color
        std::chrono::duration<double> deltaTime = std::chrono::steady_clock::now() - oldFrameTime;
        oscRed += oscAmountPerSecond * deltaTime.count();
        oldFrameTime = std::chrono::steady_clock::now();

        //debug print
        std::cout << oscRed << std::endl;

        //render
        glClearColor(oscRed, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}