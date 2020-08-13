#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 transformationMat;

smooth out vec4 vertColor;

void main()
{
	gl_Position = transformationMat * position;
	vertColor = color;
}