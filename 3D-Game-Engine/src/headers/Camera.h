#pragma once
#include "glm.hpp"


class Camera {
public:
    Camera()
        :
        pos(0.0f, 0.0f, 0.0f),
        focus(0.0f, 0.0f, -20.0f),
        cameraDirection(pos - focus),
        cameraToClip(1.0f)
    {
        cameraToClip[0][0] = frustumScale;
        cameraToClip[1][1] = frustumScale;
        cameraToClip[2][2] = (cullNear + cullFar) / (cullNear - cullFar);
        cameraToClip[3][2] = (cullNear * cullFar * 2) / (cullNear - cullFar);
        cameraToClip[2][3] = -1;
    }
    glm::mat4 DetermineWorldToCamera()
    {
        cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraDirection.y = sin(glm::radians(pitch));
        cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraZ = glm::normalize(cameraDirection);
        cameraX = glm::normalize(glm::cross(up, cameraZ));
        cameraY = glm::normalize(glm::cross(cameraZ, cameraX));

        glm::mat4 retMatrix(1.0f);
        retMatrix[0].x = cameraX.x;
        retMatrix[1].x = cameraX.y;
        retMatrix[2].x = cameraX.z;

        retMatrix[0].y = cameraY.x;
        retMatrix[1].y = cameraY.y;
        retMatrix[2].y = cameraY.z;

        retMatrix[0].z = cameraZ.x;
        retMatrix[1].z = cameraZ.y;
        retMatrix[2].z = cameraZ.z;

        retMatrix[3].x = -glm::dot(cameraX, pos);
        retMatrix[3].y = -glm::dot(cameraY, pos);
        retMatrix[3].z = -glm::dot(cameraZ, pos);

        //glm::mat4 retMatrix = glm::lookAt(pos, focus, up);

        return retMatrix;
    }
    Camera& operator= (const Camera& inCamera)
    {
        cameraZ = inCamera.cameraZ;
        cameraY = inCamera.cameraY;
        cameraX = inCamera.cameraX;

        cameraDirection = inCamera.cameraDirection;

        focus = inCamera.focus;
        pos = inCamera.pos;

        yaw = inCamera.yaw;
        pitch = inCamera.pitch;
    }
private:
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float CalcFrustumScale(float fFovDeg)
    {
        const float degToRad = 3.14159f * 2.0f / 360.0f;
        float fFovRad = fFovDeg * degToRad;
        return 1.0f / tan(fFovRad / 2.0f);
    }
public:
    float frustumScale = CalcFrustumScale(45.0f);
    glm::mat4 cameraToClip;
    glm::vec3 cameraZ, cameraX, cameraY, cameraDirection;
    glm::vec3 focus, pos;
    float yaw = 90.0f, pitch = 0.0f;
    float cullNear = 1.0f, cullFar = 1000.0f;
};
