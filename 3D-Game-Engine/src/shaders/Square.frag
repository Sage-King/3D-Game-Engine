#version 410

out vec4 outputColor;

smooth in vec4 outVertColor;
smooth in vec2 outTexCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	//outputColor = outVertColor;
	outputColor = mix(texture(texture1, outTexCoords), texture(texture2, outTexCoords), 0.5);
}
