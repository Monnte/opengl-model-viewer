#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 color, glm::float32 intensity)
{
	Light::position = position;
	Light::color = color;
	Light::intensity = intensity;
}

void Light::SetUniform(Shader shaderProgram, int index)
{
	shaderProgram.Activate();
	std::string colorsString = "lightColors[" + std::to_string(index) + "]";
	std::string positionsString = "lightPositions[" + std::to_string(index) + "]";
	glUniform3f(glGetUniformLocation(shaderProgram.ID, colorsString.c_str()), color.x * intensity, color.y * intensity, color.z * intensity);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, positionsString.c_str()), position.x, position.y, position.z);
}