#pragma once
#include "Renderable.h"
#include "OpenGLUtilities.h"
#include <string>
#include "glew.h"
class Armature : public Renderable
{
public:
    Armature();
    void Init(); //Needs OpenGL initialized before calling
    void Draw(Camera* inCamera);
    void IncrementBaseAngle();
    void DecrementBaseAngle();
    void IncrementUpperArmAngle();
    void DecrementUpperArmAngle();
    void IncrementForeArmAngle();
    void DecrementForeArmAngle();
    void IncrementFingerAngle();
    void DecrementFingerAngle();
    void DrawCube(MatrixStack& modelToWorldStack, glm::mat4 cameraToClipMatrix, glm::mat4 worldToCameraMatrix);
private:
    const static float cubeVertices[];

    const static GLushort cubeIndex[];

    inline void Clamp(float& value, float minimumValue, float maximumValue)
    {
        if (value > maximumValue)
            value = maximumValue;
        if (value < minimumValue)
            value = minimumValue;
    }

    std::string vertexShaderFilePath, fragmentShaderFilePath;

    GLuint vertexBuffer, indexBuffer, vertexArray, program, transformationMatrixUniform;

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