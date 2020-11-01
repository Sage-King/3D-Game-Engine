#version 410

out vec4 outputColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	outputColor = vec4(objectColor * lightColor,1.0f);
}