#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 cameraToClip;
uniform mat4 modelToCamera;

smooth out vec4 vertColor;

void main()
{
	vec4 cameraPosition = modelToCamera * position;
	gl_Position = cameraToClip * cameraPosition;
	vertColor = color;
}