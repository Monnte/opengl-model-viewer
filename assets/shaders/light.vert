#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 camMatrix;
uniform float lightSize;

void main()
{
	mat4 lightSizeMatrix = mat4(
		vec4(lightSize, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, lightSize, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, lightSize, 0.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	
	gl_Position = camMatrix * model * lightSizeMatrix * vec4(aPos, 1.0f);
}