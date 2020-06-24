#version 430

out vec4 outputColor;

void main()
{
	float lerpValue = gl_FragCoord.y / 480.0f;
	outputColor = mix(vec4(0.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.7f, 0.0f, 0.0f, 1.0f), lerpValue);
}
