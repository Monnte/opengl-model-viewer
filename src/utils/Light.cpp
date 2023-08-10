#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 color, glm::float32 intensity)
{
	Light::position = position;
	Light::color = color;
	Light::intensity = intensity;

	std::vector<Vertex> lightVertices = {
		Vertex{glm::vec3(-0.1f, -0.1f, 0.1f)},
		Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, -0.1f, 0.1f)},
		Vertex{glm::vec3(-0.1f, 0.1f, 0.1f)},
		Vertex{glm::vec3(-0.1f, 0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, 0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, 0.1f, 0.1f)}};

	std::vector<GLuint> lightIndices = {
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7};

	lightVAO.Bind();
	VBO lightVBO(lightVertices);
	EBO lightEBO(lightIndices);

	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();
}

void Light::SetUniform(Shader &shaderProgram, int index)
{
	shaderProgram.Activate();
	std::string colorsString = "lightColors[" + std::to_string(index) + "]";
	std::string positionsString = "lightPositions[" + std::to_string(index) + "]";
	glUniform3f(glGetUniformLocation(shaderProgram.ID, colorsString.c_str()), color.x * intensity, color.y * intensity, color.z * intensity);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, positionsString.c_str()), position.x, position.y, position.z);
}

void Light::Draw(Shader &shaderProgram, Camera &camera)
{
	shaderProgram.Activate();
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightVAO.Bind();

	lightModel = glm::translate(lightModel, position);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), color.x, color.y, color.z);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	camera.Matrix(shaderProgram, "camMatrix");
}
