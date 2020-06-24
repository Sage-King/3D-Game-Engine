#pragma once

#include "glew.h"
#include "glfw3.h"
#include <iostream>

GLFWwindow* initDependencies();
void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam);
void enableOpenGLDebug();