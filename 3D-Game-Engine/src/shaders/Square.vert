#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;

uniform mat4 inTransformationMat;

smooth out vec4 outVertColor;
smooth out vec2 outTexCoords;

void main()
{
	gl_Position = inTransformationMat * vec4(inPosition,1.0f);
	outVertColor = vec4(inColor,1.0f);
	outTexCoords = inTexCoords;
}