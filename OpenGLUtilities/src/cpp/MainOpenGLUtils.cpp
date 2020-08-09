#include <iostream>
#include "OpenGLUtilities.h"

#include "glm.hpp"



#define PI 3.14159f

void TestUtilFunc()
{
	std::cout << "test func 1 succesful" << std::endl;
}

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

