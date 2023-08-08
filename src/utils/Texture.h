#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include "shaderClass.h"
#include <glm/glm.hpp>
#include "Utils.h"

#include <string>

using namespace std;

struct Material
{	
	glm::vec3 ambient = glm::vec3(1.0f);
	glm::vec3 diffuse = glm::vec3(1.0f);
	glm::vec3 specular = glm::vec3(1.0f);
	float metallic = 0.0f;
	float roughness = 0.0f;
	float opacity = 1.0f;
};


class Texture
{
public:
	GLuint ID;
	std::string type;
	GLuint unit;

	Texture(const char *image, const char *texType, GLuint slot);

	void texUnit(Shader &shader, const char *uniform, GLuint unit);
	void Bind();
	void Unbind();
	void Delete();
};

#endif