/*
--------------------------------------------------------------
                           To-Do List
      - Shader Class
      - Textures
      - Lighting
      - Renderable Object Class 
      - Model Loader
      - More functional and flexible (no hard coding) 
        input system
      - Transition to CMake
      - Get rid of as many globals as possible ;-; They're bad, I know.
      - Relative File paths
-------------------------------------------------------------
                           Done List
      - Textures part 1
 */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <Windows.h>

#include "glew.h"
#include "glfw3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/matrix_access.hpp"

#include "Camera.h"
#include "Armature.h"
#include "Shader.h"
#include "LightCube.h"

#include "OpenGLUtilities.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>
#include <stack>
#include <deque>
#include <array>
#include <unordered_map>
#include <vector>

#define PI 3.14159f


std::vector<std::unique_ptr<Renderable>> renderables;
Armature armature;
LightCube lightCube;
std::vector<Camera> cameras;
Camera* currentCamera;

GLuint texture, texture2;
GLuint squareBuffer, squareArray, squareIbo, squareShader;
GLFWwindow* window;

int texWidth, texHeight, numOfColorChannels;

unsigned char* data;

float incrementalAngleX = 0.0f, incrementalAngleY = 0.0f, incrementalAngleZ = 0.0f;

double pastMouseX = 0, pastMouseY = 0;
float lastX, lastY;

MatrixStack matrixStack;

float currentFrameTime = 0.0f, lastFrameTime = 0.0f, deltaTime = 0.0f;

bool firstMouseEntry = true;

const int numberOfVertices = 8;


#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f
#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR 0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const float intersectingPyramids[] =
{
    +1.0f, +1.0f, +1.0f,
    -1.0f, -1.0f, +1.0f,
    -1.0f, +1.0f, -1.0f,
    +1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    +1.0f, +1.0f, -1.0f,
    +1.0f, -1.0f, +1.0f,
    -1.0f, +1.0f, +1.0f,

    GREEN_COLOR,
    BLUE_COLOR,
    RED_COLOR,
    BROWN_COLOR,

    GREEN_COLOR,
    BLUE_COLOR,
    RED_COLOR,
    BROWN_COLOR,
};

const GLshort indexData[] =
{
    0, 1, 2,
    1, 0, 3,

    2, 3, 0,
    3, 2, 1,

    5, 4, 6,
    4, 5, 7,

    7, 6, 4,
    6, 7, 5,
};

const float squareVertices[] =
{
    //pos, rgb, tex
    1.0f, 1.0f,0.0f,   1.0f,0.0f,0.0f,   1.0f,1.0f,
    1.0f,-1.0f,0.0f,   0.0f,1.0f,0.0f,   1.0f,0.0f,
   -1.0f,-1.0f,0.0f,   0.0f,0.0f,1.0f,   0.0f,0.0f,
   -1.0f, 1.0f,0.0f,   1.0f,1.0f,1.0f,   0.0f,1.0f
};

const unsigned short squareIndex[] =
{
    0, 1, 3,
    1, 2, 3,
};


void printMat4(glm::mat4 inMat)
{

    std::cout << inMat[0].x << " ";
    std::cout << inMat[1].x << " ";
    std::cout << inMat[2].x << " ";
    std::cout << inMat[3].x << '\n';

    std::cout << inMat[0].y << " ";
    std::cout << inMat[1].y << " ";
    std::cout << inMat[2].y << " ";
    std::cout << inMat[3].y << '\n';

    std::cout << inMat[0].z << " ";
    std::cout << inMat[1].z << " ";
    std::cout << inMat[2].z << " ";
    std::cout << inMat[3].z << '\n';

    std::cout << inMat[0].w << " ";
    std::cout << inMat[1].w << " ";
    std::cout << inMat[2].w << " ";
    std::cout << inMat[3].w << '\n' << std::endl;
}

void printFQuat(glm::fquat inQuat)
{
    std::cout << inQuat.x << '\n';
    std::cout << inQuat.y << '\n';
    std::cout << inQuat.z << '\n';
    std::cout << inQuat.w << std::endl;
}

inline std::chrono::duration<float> timeDifference(std::chrono::steady_clock::time_point lhs, std::chrono::steady_clock::time_point rhs)
{
    return lhs - rhs;
}

void DrawTextureSquare(glm::mat4 inCameraToClip, glm::mat4 worldToCamera)
{
    glUseProgram(squareShader);

    GLuint transformationMatrixSquare = glGetUniformLocation(squareShader, "inTransformationMat");

    glm::mat4 modelToWorld(1.0f);

    modelToWorld[3].z = -2;

    glm::mat4 gpuMatrix(1.0f);
    gpuMatrix = inCameraToClip * worldToCamera * modelToWorld * gpuMatrix;

    glUniformMatrix4fv(transformationMatrixSquare, 1, GL_FALSE, glm::value_ptr(gpuMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(squareArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
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


void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    currentCamera->cameraToClip[0][0] = currentCamera->frustumScale / (width / (float)height);
    currentCamera->cameraToClip[1][1] = currentCamera->frustumScale;

    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (action == GLFW_PRESS)
    {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_ENTER:
            //camera.Reset();
            break;
        case GLFW_KEY_0:
            //camera.IncrementXAngle();
            break;
        }
            
    }
        
    /*for (std::vector<int>::iterator iter = keyPressedVector.begin(); iter != keyPressedVector.end(); iter = keyPressedVector.begin())
    {
        switch (*iter) {

        

        case GLFW_KEY_0:
            armature.IncrementBaseAngle();
            keyPressedVector.erase(iter);
            break;

        case GLFW_KEY_9:
            armature.DecrementBaseAngle();
            break;

        case GLFW_KEY_8:
            armature.IncrementUpperArmAngle();
            break;

        case GLFW_KEY_7:
            armature.DecrementUpperArmAngle();
            break;

        case GLFW_KEY_P:
            armature.IncrementForeArmAngle();
            break;

        case GLFW_KEY_O:
            armature.DecrementForeArmAngle();
            break;
        case GLFW_KEY_I:
            armature.IncrementFingerAngle();
            break;

        case GLFW_KEY_U:
            armature.DecrementFingerAngle();
            break;

        case GLFW_KEY_Q:
            camera.IncrementZAngle();
            break;

        case GLFW_KEY_E:
            camera.DecrementZAngle();
            break;

        case GLFW_KEY_A:
            camera.DecrementXPos();
            break;


        case GLFW_KEY_D:
            camera.IncrementXPos();
            break;

        case GLFW_KEY_W:
            camera.DecrementZPos();
            break;

        case GLFW_KEY_S:
            camera.IncrementZPos();
            break;

        case GLFW_KEY_R:
            camera.IncrementYPos();
            break;

        case GLFW_KEY_F:
            camera.DecrementYPos();
            break;
        }
    }*/
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouseEntry)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouseEntry = false;
    }
    if (currentCamera->pitch > 89.0f)
        currentCamera->pitch = 89.0f;
    if (currentCamera->pitch < -89.0f)
        currentCamera->pitch = -89.0f;

    float xOffset = (float)xpos - lastX;
    float yOffset = (float)ypos - lastY;
    lastX = (float)xpos;
    lastY = (float)ypos;

    const float sensitivity = 0.065f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    currentCamera->yaw += xOffset;
    currentCamera->pitch += yOffset;
}

void windowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        firstMouseEntry = true;
    }
    else
    {
        
    }
}

void handleKeyInputs()
{
    float movementNum = 50.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        currentCamera->pos -= movementNum * currentCamera->cameraZ;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        currentCamera->pos += movementNum * currentCamera->cameraZ ;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        currentCamera->pos -= movementNum * currentCamera->cameraX;

    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        currentCamera->pos += movementNum * currentCamera->cameraX;

    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        currentCamera->pos += movementNum * currentCamera->cameraY;

    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        currentCamera->pos -= movementNum * currentCamera->cameraY;

    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        armature.IncrementBaseAngle();
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        armature.DecrementBaseAngle();
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        armature.IncrementUpperArmAngle();
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        armature.DecrementUpperArmAngle();
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        armature.IncrementForeArmAngle();
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        armature.DecrementForeArmAngle();
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        armature.IncrementFingerAngle();
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        armature.DecrementFingerAngle();
    }
}

void initOpenGL()
{
    if (!glfwInit())
    {
        throw std::runtime_error::runtime_error("glfw failed to initialize");
    }

    int numOfMonitors;
    GLFWmonitor** monitors = glfwGetMonitors(&numOfMonitors);
    if (!monitors[1])
    {
        throw std::runtime_error::runtime_error("Failed to get Second Monitor");
    }
    const GLFWvidmode* mode = glfwGetVideoMode(monitors[1]);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    std::cout << mode->width << std::endl;
    std::cout << mode->height << std::endl;

    window = glfwCreateWindow(mode->width, mode->height, "Test Application", NULL, NULL);
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

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    glfwSetKeyCallback(window, keyboardCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetCursorPosCallback(window, mousePosCallback);
#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif
}

//void initShaders()
//{
//    std::vector<GLenum> types{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
//    std::vector<std::string> filepaths{ "D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\Square.vert","D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\Square.frag" };
//    squareShader = Shader(types, filepaths).GetHandle();
//}

void RegisterRenderables()
{
    //renderables.push_back(&armature);
}

void InitRenderables()
{

}

//called once before main loop
void init()
{
    initOpenGL();
    //initShaders();

    RegisterRenderables();
    InitRenderables();

    //RainbowCube rainbowCube;
    Camera mainCamera;
    armature.Init();
    lightCube.Init();

    //renderables.push_back(rainbowCube);
    cameras.push_back(mainCamera);

    currentCamera = &cameras[0];

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\Assets\\Textures\\pattern.png", &texWidth, &texHeight, &numOfColorChannels, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture data" << std::endl;
    }

    stbi_image_free(data);

    data = stbi_load("D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\Assets\\Textures\\rose.jpg", &texWidth, &texHeight, &numOfColorChannels, 0);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture data" << std::endl;
    }

    stbi_image_free(data);

    glUseProgram(squareShader);
    glUniform1i(glGetUniformLocation(squareShader, "texture1"), 0);
    glUniform1i(glGetUniformLocation(squareShader, "texture2"), 1);
    glUseProgram(0);

    glGenBuffers(1, &squareBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, squareBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &squareIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndex), squareIndex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &squareArray);
    glBindVertexArray(squareArray);

    glBindBuffer(GL_ARRAY_BUFFER, squareBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareIbo);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glGenBuffers(1, &vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(intersectingPyramids), intersectingPyramids, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    //glGenBuffers(1, &ibo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glGenVertexArrays(1, &vao1);
    //glBindVertexArray(vao1);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(1);
    //size_t colorDataOffset = numberOfVertices * sizeof(float) * 3;
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //glBindVertexArray(0);

    //glGenVertexArrays(1, &vao2);
    //glBindVertexArray(vao2);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glEnableVertexAttribArray(0);
    //const size_t posDataOffset = sizeof(float) * 3 * (numberOfVertices / 2);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)posDataOffset);
    //glEnableVertexAttribArray(1);
    //colorDataOffset += sizeof(float) * 4 * (numberOfVertices / 2);
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
}

//called every loop
void draw()
{
    currentFrameTime = (float)glfwGetTime();
    deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    handleKeyInputs();

    /*or (std::vector<Renderables>::const_iterator currentRenderable; currentRenderable != renderables.end(); std::advance(renderables, 1))
    {
        currentRenderable->Draw(*currentCamera);
    }*/

    armature.Draw(currentCamera);
    lightCube.Draw(currentCamera);

    DrawTextureSquare(currentCamera->cameraToClip, currentCamera->DetermineWorldToCamera());

}


const float cubeVertices[] =
{
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
};

const GLushort cubeIndex[] =
{
    //front
    0, 1, 3,
    0, 3, 2,
    //right
    1, 5, 7,
    1, 7, 3,
    //back
    5, 4, 6,
    5, 6, 7,
    //left
    4, 0, 2,
    4, 2, 6,
    //top
    4, 5, 1,
    4, 1, 0,
    //bottom
    2, 3, 7,
    2, 7, 6
};

GLuint vao1, vao2, vbo, ibo;
Camera camera;

Shader shadertester;

void initLightingTut()
{
    initOpenGL();

    Camera mainCamera;
    cameras.push_back(mainCamera);
    currentCamera = &cameras[0];

    const std::vector<GLenum>& types{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    const std::vector<std::string>& filepaths{ "D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\lighting.vert", "D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\lighting.frag" };
    shadertester.~Shader();
    new(&shadertester) Shader(types, filepaths);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndex), cubeIndex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawLightingTut()
{
    glUseProgram(shadertester.GetHandle());

    glm::mat4 modelToWorld(1.0f);
    modelToWorld[3].z = -10.0f;

    glm::mat4 gpuMatrix(1.0f);
    gpuMatrix = camera.cameraToClip * camera.DetermineWorldToCamera() * modelToWorld * gpuMatrix;

    shadertester.SetMat4("transformationMatrix", gpuMatrix); 

    glBindVertexArray(vao1);
    glDrawElements(GL_TRIANGLES, sizeof(cubeIndex) / sizeof(cubeIndex[0]), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(vao2);
    glDrawElements(GL_TRIANGLES, sizeof(cubeIndex) / sizeof(cubeIndex[0]), GL_UNSIGNED_SHORT, 0);
    glUseProgram(0);
}
int main(void)
{
    try
    {
        //init();
        initLightingTut();
    }
    catch(std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        std::cin.get();
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearDepth(1.0f);
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw();
        drawLightingTut();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}