#include <Windows.h>

#include "glew.h"
#include "glfw3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/matrix_access.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>
#include <stack>
#include <deque>

#define PI 3.14159f

GLuint programHandle, vbo,vao1, vao2,ibo;
GLuint cubeBuffer, cubeArray, cubeIbo;
GLuint offsetUniform, cameraToClipUniform, modelToCameraUniform;
GLFWwindow* window;

float incrementalAngleX = 0.0f, incrementalAngleY = 0.0f, incrementalAngleZ = 0.0f;

glm::mat3 RotateX(float angleInDegree)
{
    glm::mat3 retMatrix(1.0f);

    float AngleInRad = (angleInDegree * PI) / 180;
    float Cos = cosf(AngleInRad);
    float Sin = sinf(AngleInRad);

    retMatrix[1].y = Cos;
    retMatrix[2].y = -Sin;
    retMatrix[1].z = Sin;
    retMatrix[2].z = Cos;

    return retMatrix;
}

glm::mat3 RotateY(float angleInDegree)
{
    glm::mat3 retMatrix(1.0f);

    float AngleInRad = (angleInDegree * PI) / 180;
    float Cos = cosf(AngleInRad);
    float Sin = sinf(AngleInRad);

    retMatrix[0].x = Cos;
    retMatrix[2].x = Sin;
    retMatrix[0].z = -Sin;
    retMatrix[2].z = Cos;

    return retMatrix;
}

glm::mat3 RotateZ(float angleInDegree)
{
    glm::mat3 retMatrix(1.0f);

    float AngleInRad = (angleInDegree * PI) / 180;
    float Cos = cosf(AngleInRad);
    float Sin = sinf(AngleInRad);

    retMatrix[0].x = Cos;
    retMatrix[1].x = -Sin;
    retMatrix[0].y = Sin;
    retMatrix[1].y = Cos;

    return retMatrix;
}

struct MatrixStack {
public:
    MatrixStack()
    {
        Push(glm::mat4(1.0f));
    }

    void Push(glm::mat4 pushMatrix)
    {
        matrixStack.push(pushMatrix);
    }
    void PushTop()
    {
        matrixStack.push(Top());
    }
    glm::mat4& Top() 
    {
        return matrixStack.top();
    }
    void translate(glm::vec3 inputVec)
    {
        glm::mat4 translateMatrix(1.0f);

        translateMatrix[3] = glm::vec4(inputVec, 1.0f);

        matrixStack.top() = matrixStack.top() * translateMatrix;
    }
    void scale(glm::vec3 scaleVec)
    {
        glm::mat4 scaleMatrix(1.0f);
        scaleMatrix[0].x = scaleVec.x;
        scaleMatrix[1].y = scaleVec.y;
        scaleMatrix[2].z = scaleVec.z;

        matrixStack.top() = matrixStack.top() * scaleMatrix;
    }
    void rotateX(float angleInDegrees)
    {
        matrixStack.top() = matrixStack.top() * glm::mat4(RotateX(angleInDegrees));
    }
    void rotateY(float angleInDegrees)
    {
        matrixStack.top() = matrixStack.top() * glm::mat4(RotateY(angleInDegrees));
    }
    void rotateZ(float angleInDegrees)
    {
        matrixStack.top() = matrixStack.top() * glm::mat4(RotateZ(angleInDegrees));
    }
    void Pop()
    {
        if (!matrixStack.empty()) 
            matrixStack.pop();
    }
private:
    std::stack<glm::mat4> matrixStack;
};

MatrixStack matrixStack;



std::chrono::time_point<std::chrono::steady_clock> startOfProgram;

glm::mat4 cameraToClip;

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

const float cubeVertices[] =
{
    -0.5f,   0.5f,  0.5f,
     0.5f,   0.5f,  0.5f,
    -0.5f,  -0.5f,  0.5f,
     0.5f,  -0.5f,  0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,

    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    0.41176f, 1.0f, 0.86274f,1.0f

};

const GLushort cubeIndex[] =
{
    //front
    0,1,3,
    0,3,2,
    //right
    1,5,7,
    1,7,3,
    //back
    5,4,6,
    5,6,7,
    //left
    4,0,2,
    4,2,6,
    //top
    4,5,1,
    4,1,0,
    //bottom
    2,3,7,
    2,7,6
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
class Armature
{
public:
    Armature()
        :
        basePos(-5.0f, -5.0f, -20.0f),
        baseAngleY(45),
        baseScale(1.0f, 1.0f, 2.0f),
        baseLeftPos(1.0f, 0.0f, 0.0f),
        baseRightPos(-1.0f, 0.0f, 0.0f),
        upperArmAngle(-60.0),
        upperArmScale(1.0f, 1.0f, 5.0f),
        upperArmPos(0.0f, 0.5f, 2.5f),
        endOfUpperArm(0.0f, 0.0f, 2.5f),
        foreArmAngle(110),
        foreArmPos(0.0f, 0.0f, 1.5f),
        foreArmScale(0.8f, 0.8f, 3.75f),
        endOfForeArm(0.0f, 0.0f, 2.24f),
        rightEdgeOfWrist(0.5f, 0.0f, 0.0f),
        upperFingerAngle(-30),
        upperFingerScale(1.5f, 0.5f, 0.5f),
        upperRightFingerPos(0.60f, 0.0f, 0.0f),
        endOfUpperRightFinger(0.8f, 0.0f, 0.0f),
        lowerRightFingerAngle(-100),
        lowerFingerScale(1.5f, 0.4f, 0.4f),
        lowerRightFingerPos(0.6f, 0.0f, 0.0f),
        leftEdgeOfWrist(-0.5, 0.0f, 0.0f),
        upperLeftFingerPos(-0.6f, 0.0f, 0.0f),
        endOfUpperLeftFinger(-0.8f, 0.0f, 0.0f),
        lowerLeftFingerAngle(-80),
        lowerLeftFingerPos(0.6,0.0f,0.0f)
    {
    }

    void Draw()
    {
        MatrixStack matrixStack;

        matrixStack.translate(basePos);
        matrixStack.rotateY(baseAngleY);

        matrixStack.PushTop();

        matrixStack.scale(baseScale);

        matrixStack.PushTop();
        matrixStack.translate(baseLeftPos);
        DrawCube(matrixStack);
        matrixStack.Pop();

        matrixStack.PushTop();
        matrixStack.translate(baseRightPos);
        DrawCube(matrixStack);
        matrixStack.Pop();

        matrixStack.Pop();

        matrixStack.rotateX(upperArmAngle);
        matrixStack.translate(upperArmPos);

        matrixStack.PushTop();
        matrixStack.scale(upperArmScale);
        DrawCube(matrixStack);
        matrixStack.Pop();

        matrixStack.translate(endOfUpperArm);
        matrixStack.rotateX(foreArmAngle);
        matrixStack.translate(foreArmPos);

        matrixStack.PushTop();
        matrixStack.scale(foreArmScale);
        DrawCube(matrixStack);
        matrixStack.Pop();

        matrixStack.translate(endOfForeArm);
        DrawCube(matrixStack);

        matrixStack.PushTop();
        matrixStack.translate(rightEdgeOfWrist);
        matrixStack.rotateZ(upperFingerAngle);
        matrixStack.translate(upperRightFingerPos);

        matrixStack.PushTop();
        matrixStack.scale(upperFingerScale);
        DrawCube(matrixStack);
        matrixStack.Pop();

        matrixStack.translate(endOfUpperRightFinger);
        matrixStack.rotateZ(lowerRightFingerAngle);
        matrixStack.translate(lowerRightFingerPos);
        matrixStack.PushTop();
        matrixStack.scale(lowerFingerScale);
        DrawCube(matrixStack);
        matrixStack.Pop();
        matrixStack.Pop();

        matrixStack.PushTop();
        matrixStack.translate(leftEdgeOfWrist);
        matrixStack.rotateZ(360 - upperFingerAngle);
        matrixStack.translate(upperLeftFingerPos);
        matrixStack.PushTop();
        matrixStack.scale(upperFingerScale);
        DrawCube(matrixStack);
        matrixStack.Pop();

        matrixStack.PushTop();
        matrixStack.translate(endOfUpperLeftFinger);
        matrixStack.rotateZ(lowerLeftFingerAngle);
        matrixStack.translate(lowerLeftFingerPos);

        matrixStack.PushTop();
        matrixStack.scale(lowerFingerScale);
        DrawCube(matrixStack);
        matrixStack.Pop();
        matrixStack.Pop();
    }
    void IncrementBaseAngle()
    {
        baseAngleY += 5.0;
        //Clamp(baseAngleY, 0.0f, 90.0f);
    }
    void DecrementBaseAngle()
    {
        baseAngleY -= 5.0;
        //Clamp(baseAngleY, 0.0f, 90.0f);
    }
    void IncrementUpperArmAngle()
    {
        upperArmAngle += 5.0f;
        Clamp(upperArmAngle, -80, -45);
    }
    void DecrementUpperArmAngle()
    {
        upperArmAngle -= 5.0f;
        Clamp(upperArmAngle, -80, -45);
    }
    void IncrementForeArmAngle()
    {
        foreArmAngle += 5.0f;
        Clamp(foreArmAngle, 80, 140);
    }
    void DecrementForeArmAngle()
    {
        foreArmAngle -= 5.0f;
        Clamp(foreArmAngle, 80, 140);
    }
    void IncrementFingerAngle()
    {
        upperFingerAngle += 5.0f;
        Clamp(upperFingerAngle, -60, 30);
    }
    void DecrementFingerAngle()
    {
        upperFingerAngle -= 5.0f;
        Clamp(upperFingerAngle, -60, 30);
    }
    void DrawCube(MatrixStack& modelToCameraStack)
    {
        glUseProgram(programHandle);
        glBindVertexArray(cubeArray);
        glUniformMatrix4fv(modelToCameraUniform, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndex) / sizeof(cubeIndex[0]), GL_UNSIGNED_SHORT, 0);
        glUseProgram(0);
        glBindVertexArray(0);
    }
private:
    inline void Clamp(float& value, float minimumValue, float maximumValue)
    {
        if (value > maximumValue)
            value = maximumValue;
        if (value < minimumValue)
            value = minimumValue;
    }

    glm::vec3 basePos, baseLeftPos, baseRightPos;
    glm::vec3 baseScale;
    float baseAngleY;

    glm::vec3 upperArmPos, upperArmScale, endOfUpperArm;
    float upperArmAngle;

    glm::vec3 foreArmPos, foreArmScale, endOfForeArm;
    float foreArmAngle;

    glm::vec3 rightEdgeOfWrist, leftEdgeOfWrist;

    glm::vec3 upperRightFingerPos, upperLeftFingerPos, upperFingerScale, endOfUpperRightFinger, endOfUpperLeftFinger;
    float upperFingerAngle;

    glm::vec3 lowerRightFingerPos, lowerLeftFingerPos, lowerFingerScale;
    float lowerRightFingerAngle, lowerLeftFingerAngle;

};

Armature armature;

inline std::chrono::duration<float> timeDifference(std::chrono::steady_clock::time_point lhs, std::chrono::steady_clock::time_point rhs)
{
    return lhs - rhs;
}

//pulled from gltut
float CalcFrustumScale(float fFovDeg)
{
    const float degToRad = 3.14159f * 2.0f / 360.0f;
    float fFovRad = fFovDeg * degToRad;
    return 1.0f / tan(fFovRad / 2.0f);
}
//pulled from gltut
float ComputeAngleRad(float fElapsedTime, float fLoopDuration)
{
    const float fScale = 3.14159f * 2.0f / fLoopDuration;
    float fCurrTimeThroughLoop = fmodf(fElapsedTime, fLoopDuration);
    return fCurrTimeThroughLoop * fScale;
}

float frustumScaleF = CalcFrustumScale(45.0f);

glm::vec3 StationaryOffset(float elapsedTimeF) 
{
    return glm::vec3(0.0f, 0.0f, -20.0f);
}

glm::vec3 RotationXYOffset(float elapsedTimeF)
{
    const float loopDuration = 3.0f;
    const float scale = (2 * 3.14159f)/loopDuration;

    float currTimeThroughLoop = fmodf(elapsedTimeF, loopDuration);

    return glm::vec3(cosf(currTimeThroughLoop * scale) * 5, sinf(currTimeThroughLoop * scale) * 5, -20);

}

glm::vec3 RotationXZOffset(float elapsedTimeF)
{
    const float loopDuration = 3.0f;
    const float scale = (2 * 3.14159f) / loopDuration;

    float currTimeThroughLoop = fmodf(elapsedTimeF, loopDuration);

    return glm::vec3(cosf(currTimeThroughLoop * scale) * 5, 0, sinf(currTimeThroughLoop * scale) * -40);

}

glm::vec3 NullScale(float elapsedTimeF)
{
    return glm::vec3(1.0f, 1.0f, 1.0f);
}

glm::vec3 StaticUniformScale(float elapsedTimeF)
{
    return glm::vec3(3.0f, 3.0f, 3.0f);
}

glm::vec3 StaticNonUniformScale(float elapsedTimeF)
{
    return glm::vec3(1.0f, 5.0f, 2.0f);
}

glm::vec3 DynamicNonUniformScale(float elapsedTimeF)
{
    const float loopDuration = 3.0f;
    const float scale = (2 * 3.14159f) / loopDuration;

    float currTimeThroughLoop = fmodf(elapsedTimeF, loopDuration);
    glm::vec3 retVec;
    retVec.x = cosf(currTimeThroughLoop * scale) * 5;
    retVec.y = sinf(currTimeThroughLoop * scale) * 5;
    retVec.z = sinf(currTimeThroughLoop * scale) * cosf(currTimeThroughLoop * scale) * 5;

    return retVec;
}

glm::vec3 DynamicUniformScale(float elapsedTimeF)
{
    const float loopDuration = 3.0f;
    const float scale = (2 * 3.14159f) / loopDuration;

    float currTimeThroughLoop = fmodf(elapsedTimeF, loopDuration);

    glm::vec3 retVec;
    retVec.x = 1 + cosf(currTimeThroughLoop * scale) * 5;
    retVec.y = 1 + cosf(currTimeThroughLoop * scale) * 5;
    retVec.z = 1 + cosf(currTimeThroughLoop * scale) * 5;

    return retVec;
}

glm::mat3 NullRotate(float elapsedtimeF)
{
    return glm::mat3(1.0f);
}

//pulled from gltut. quaternion math is a lot to write out ;-;
glm::mat3 RotateAxis(float fElapsedTime)
{
    float fAngRad = ComputeAngleRad(fElapsedTime, 2.0);
    float fCos = cosf(fAngRad);
    float fInvCos = 1.0f - fCos;
    float fSin = sinf(fAngRad);
    float fInvSin = 1.0f - fSin;

    glm::vec3 axis(1.0f, 1.0f, 1.0f);
    axis = glm::normalize(axis);

    glm::mat3 theMat(1.0f);
    theMat[0].x = (axis.x * axis.x) + ((1 - axis.x * axis.x) * fCos);
    theMat[1].x = axis.x * axis.y * (fInvCos)-(axis.z * fSin);
    theMat[2].x = axis.x * axis.z * (fInvCos)+(axis.y * fSin);

    theMat[0].y = axis.x * axis.y * (fInvCos)+(axis.z * fSin);
    theMat[1].y = (axis.y * axis.y) + ((1 - axis.y * axis.y) * fCos);
    theMat[2].y = axis.y * axis.z * (fInvCos)-(axis.x * fSin);

    theMat[0].z = axis.x * axis.z * (fInvCos)-(axis.y * fSin);
    theMat[1].z = axis.y * axis.z * (fInvCos)+(axis.x * fSin);
    theMat[2].z = (axis.z * axis.z) + ((1 - axis.z * axis.z) * fCos);
    return theMat;
}

struct Instance
{
    typedef glm::mat3(*rotationFunc)(float);

    rotationFunc CalcRotation;

    glm::vec3 offset;

    glm::mat4 ConstructMatrix(float elapsedTimeF)
    {
        
        glm::mat4 returnMatrix(CalcRotation(elapsedTimeF));
        returnMatrix[3] = glm::vec4(offset,1.0f);
        return returnMatrix;
    }
};

const Instance g_InstanceList[] = {
    {RotateX, glm::vec3(3.0f,-3.0f,-20.0f)},
    {RotateY, glm::vec3(-3.0f,-3.0f, -20.0f)},
    {RotateZ, glm::vec3(-3.0,3.0f,-20.0f)},
    {RotateAxis, glm::vec3(3.0f,3.0f,-20.0f)},
    {NullRotate, glm::vec3(0.0f,0.0f,-20.0f)}
};

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

void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    cameraToClip[0][0] = frustumScaleF / (width / (float)height);
    cameraToClip[1][1] = frustumScaleF;

    glUseProgram(programHandle);
    glUniformMatrix4fv(cameraToClipUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
    glUseProgram(0);

    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch(key){

    case GLFW_KEY_0:
        if (action == GLFW_PRESS)
            armature.IncrementBaseAngle();
        if (action == GLFW_REPEAT)
            armature.IncrementBaseAngle();
        break;
    
    case GLFW_KEY_9:
        if (action == GLFW_PRESS)
            armature.DecrementBaseAngle();
        if (action == GLFW_REPEAT)
            armature.DecrementBaseAngle();
        break;

    case GLFW_KEY_8:
        if (action == GLFW_PRESS)
            armature.IncrementUpperArmAngle();
        if (action == GLFW_REPEAT)
            armature.IncrementUpperArmAngle();
        break;

    case GLFW_KEY_7:
        if (action == GLFW_PRESS)
            armature.DecrementUpperArmAngle();
        if (action == GLFW_REPEAT)
            armature.DecrementUpperArmAngle();
        break;

    case GLFW_KEY_P:
        if (action == GLFW_PRESS)
            armature.IncrementForeArmAngle();
        if (action == GLFW_REPEAT)
            armature.IncrementForeArmAngle();
        break;

    case GLFW_KEY_O:
        if (action == GLFW_PRESS)
            armature.DecrementForeArmAngle();
        if (action == GLFW_REPEAT)
            armature.DecrementForeArmAngle();
        break;
    case GLFW_KEY_I:
        if (action == GLFW_PRESS)
            armature.IncrementFingerAngle();
        if (action == GLFW_REPEAT)
            armature.IncrementFingerAngle();
        break;

    case GLFW_KEY_U:
        if (action == GLFW_PRESS)
            armature.DecrementFingerAngle();
        if (action == GLFW_REPEAT)
            armature.DecrementFingerAngle();
        break;
    }
}

void initOpenGL()
{
    if (!glfwInit())
    {
        throw std::runtime_error::runtime_error("glfw failed to initialize");
    }
    window = glfwCreateWindow(640, 480, "Test Application", NULL, NULL);
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
    startOfProgram = std::chrono::steady_clock::now();

    initOpenGL();
    initShaders();

    glGenBuffers(1, &cubeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &cubeIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndex), cubeIndex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &cubeArray);
    glBindVertexArray(cubeArray);
    glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const void*)(8 * 3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIbo);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(intersectingPyramids), intersectingPyramids, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    size_t colorDataOffset = numberOfVertices * sizeof(float) * 3;
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);

    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    const size_t posDataOffset = sizeof(float) * 3 * (numberOfVertices / 2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)posDataOffset);
    glEnableVertexAttribArray(1);
    colorDataOffset += sizeof(float) * 4 * (numberOfVertices / 2);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);

    const float camNearF = 1.0f;
    const float camFarF = 100.0f;

    cameraToClip[0][0] = frustumScaleF;
    cameraToClip[1][1] = frustumScaleF;
    cameraToClip[2][2] = (camNearF + camFarF) / (camNearF - camFarF);
    cameraToClip[3][2] = (camNearF * camFarF * 2) / (camNearF - camFarF);
    cameraToClip[2][3] = -1;

    cameraToClipUniform = glGetUniformLocation(programHandle, "cameraToClip");
    modelToCameraUniform = glGetUniformLocation(programHandle, "modelToCamera");

    glUseProgram(programHandle);
    glUniformMatrix4fv(cameraToClipUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
    glm::mat4 modelToCamera(1.0f);
    modelToCamera[3] = glm::vec4(0.0f, 0.0f, -20.0f, 1.0f);
    modelToCamera[1].y = cosf(PI / 2);
    modelToCamera[1].z = -sinf(PI / 2);
    modelToCamera[2].y = sinf(PI / 2);
    modelToCamera[2].z = cosf(PI / 2);
    glUniformMatrix4fv(modelToCameraUniform, 1, GL_FALSE, glm::value_ptr(modelToCamera));
    glUseProgram(0);    

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
}

glm::mat4 uploadmat(1.0f);

glm::vec3 offset(0.0f, 0.0f, -20.0);


//called every loop
void draw()
{
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    armature.Draw();
    
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