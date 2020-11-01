#version 410

layout(location = 0) in vec3 position;

uniform mat4 transformationMatrix;

void main()
{
	gl_Position = transformationMatrix * vec4(position,1.0f);
}