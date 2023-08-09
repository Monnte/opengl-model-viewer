#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include "Camera.h"
#include <glad/glad.h>
#include <vector>
#include "shaderClass.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Utils.h"

class Skybox
{
public:
	GLuint textureID;
	VAO vao;
	Skybox(std::string path);
	void Draw(Shader &skyboxShader, Camera &camera);
};

#endif