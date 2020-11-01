#version 410

layout(location = 0) in vec3 position;

uniform mat4 transformationMat;

void main()
{
	gl_Position = transformationMat * vec4(position,1.0f);
}