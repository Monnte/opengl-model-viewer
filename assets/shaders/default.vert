#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;


out DATA
{
    vec3 Normal;
	vec2 texCoord;
    mat4 projection;
	mat4 model;
	vec3 lightPositions[4];
	vec3 camPos;
} data_out;



// Imports the camera matrix
uniform mat4 camMatrix;
// Imports the transformation matrices
uniform mat4 model;
// Gets the position of the light from the main function
uniform vec3 lightPositions[4];
// Gets the position of the camera from the main function
uniform vec3 camPos;

void main()
{
	gl_Position = model * vec4(aPos, 1.0f);
	data_out.Normal = aNormal;
	data_out.texCoord = aTex;
	data_out.projection = camMatrix;
	data_out.model = model;
	data_out.lightPositions = lightPositions;
	data_out.camPos = camPos;
}