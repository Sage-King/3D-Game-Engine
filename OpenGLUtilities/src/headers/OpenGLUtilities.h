#pragma once

#include "glm.hpp"

#include <stack>
#include <deque>

void TestUtilFunc();


glm::mat3 RotateX(float angleInDegree);
glm::mat3 RotateY(float angleInDegree);
glm::mat3 RotateZ(float angleInDegree);

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


