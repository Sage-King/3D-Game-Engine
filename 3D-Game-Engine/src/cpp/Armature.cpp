#include "Armature.h"

#include "Shader.h"
#include "OpenGLUtilities.h"
#include <vector>
#include <string>
#include "gtc/type_ptr.hpp"

const float Armature::cubeVertices[56] =
{
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,

        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,

        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.41176f, 1.0f, 0.86274f, 1.0f

};

const GLushort Armature::cubeIndex[36] =
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


Armature::Armature()
	:
    vertexShaderFilePath("D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\VertexShader.vert"),
    fragmentShaderFilePath("D:\\Repos\\3D-Game-Engine\\3D-Game-Engine\\src\\shaders\\FragmentShader.frag"),
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
    lowerLeftFingerPos(0.6, 0.0f, 0.0f)
{
}
void Armature::Init()
{
    std::vector<GLenum> types{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    std::vector<std::string> filepaths{ vertexShaderFilePath, fragmentShaderFilePath };

    program = Shader(types, filepaths).GetHandle();

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndex), cubeIndex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const void*)(8 * 3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Armature::Draw(Camera* inCamera)
{
    MatrixStack matrixStack;

    matrixStack.translate(basePos);
    matrixStack.rotateY(baseAngleY);

    matrixStack.PushTop();

    matrixStack.scale(baseScale);

    matrixStack.PushTop();
    matrixStack.translate(baseLeftPos);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());
    matrixStack.Pop();

    matrixStack.PushTop();
    matrixStack.translate(baseRightPos);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());

    matrixStack.Pop();

    matrixStack.Pop();

    matrixStack.rotateX(upperArmAngle);
    matrixStack.translate(upperArmPos);

    matrixStack.PushTop();
    matrixStack.scale(upperArmScale);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());

    matrixStack.Pop();

    matrixStack.translate(endOfUpperArm);
    matrixStack.rotateX(foreArmAngle);
    matrixStack.translate(foreArmPos);

    matrixStack.PushTop();
    matrixStack.scale(foreArmScale);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());

    matrixStack.Pop();

    matrixStack.translate(endOfForeArm);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());


    matrixStack.PushTop();
    matrixStack.translate(rightEdgeOfWrist);
    matrixStack.rotateZ(upperFingerAngle);
    matrixStack.translate(upperRightFingerPos);

    matrixStack.PushTop();
    matrixStack.scale(upperFingerScale);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());

    matrixStack.Pop();

    matrixStack.translate(endOfUpperRightFinger);
    matrixStack.rotateZ(lowerRightFingerAngle);
    matrixStack.translate(lowerRightFingerPos);
    matrixStack.PushTop();
    matrixStack.scale(lowerFingerScale);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());

    matrixStack.Pop();
    matrixStack.Pop();

    matrixStack.PushTop();
    matrixStack.translate(leftEdgeOfWrist);
    matrixStack.rotateZ(360 - upperFingerAngle);
    matrixStack.translate(upperLeftFingerPos);
    matrixStack.PushTop();
    matrixStack.scale(upperFingerScale);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());

    matrixStack.Pop();

    matrixStack.PushTop();
    matrixStack.translate(endOfUpperLeftFinger);
    matrixStack.rotateZ(lowerLeftFingerAngle);
    matrixStack.translate(lowerLeftFingerPos);

    matrixStack.PushTop();
    matrixStack.scale(lowerFingerScale);
    DrawCube(matrixStack, inCamera->cameraToClip, inCamera->DetermineWorldToCamera());

    matrixStack.Pop();
    matrixStack.Pop();
}
void Armature::IncrementBaseAngle()
{
    baseAngleY += 5.0;
    if (baseAngleY > 360.0f)
        baseAngleY -= 360.0f;
    //Clamp(baseAngleY, 0.0f, 90.0f);
}
void Armature::DecrementBaseAngle()
{
    baseAngleY -= 5.0;
    if (baseAngleY < -360.0f)
        baseAngleY += 360.0f;
    //Clamp(baseAngleY, 0.0f, 90.0f);
}
void Armature::IncrementUpperArmAngle()
{
    upperArmAngle += 5.0f;
    Clamp(upperArmAngle, -80, -45);
}
void Armature::DecrementUpperArmAngle()
{
    upperArmAngle -= 5.0f;
    Clamp(upperArmAngle, -80, -45);
}
void Armature::IncrementForeArmAngle()
{
    foreArmAngle += 5.0f;
    Clamp(foreArmAngle, 50, 130);
}
void Armature::DecrementForeArmAngle()
{
    foreArmAngle -= 5.0f;
    Clamp(foreArmAngle, 50, 130);
}
void Armature::IncrementFingerAngle()
{
    upperFingerAngle += 5.0f;
    Clamp(upperFingerAngle, -60, 30);
}
void Armature::DecrementFingerAngle()
{
    upperFingerAngle -= 5.0f;
    Clamp(upperFingerAngle, -60, 30);
}
void Armature::DrawCube(MatrixStack& modelToWorldStack, glm::mat4 cameraToClipMatrix, glm::mat4 worldToCameraMatrix)
{
    glm::mat4 gpuMatrix(1.0f);
    gpuMatrix = cameraToClipMatrix * worldToCameraMatrix * modelToWorldStack.Top() * gpuMatrix;

    glUseProgram(program);
    glBindVertexArray(vertexArray);
    glUniformMatrix4fv(transformationMatrixUniform, 1, GL_FALSE, glm::value_ptr(gpuMatrix));
    glDrawElements(GL_TRIANGLES, 36/*sizeof(cubeIndex) / sizeof(cubeIndex[0])*/, GL_UNSIGNED_SHORT, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}



