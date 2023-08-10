#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaderClass.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

using namespace std;

class Light
{
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::float32 intensity = 200.0f;

	VAO lightVAO;

	Light(glm::vec3 position, glm::vec3 color, glm::float32 intensity);
	void SetUniform(Shader &shaderProgram, int index);
	
	void Draw(Shader &shaderProgram, Camera &camera);
};

#endif