#include "DependencyInit.h"


GLFWwindow* initDependencies()
{
    if (!glfwInit())
    {
        throw std::runtime_error::runtime_error("glfw failed to initialize");
    }
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
    return window;
}

void enableOpenGLDebug()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}

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
