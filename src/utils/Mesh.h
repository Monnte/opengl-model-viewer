#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"
#include "assimp/material.h"

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	Material material;
	glm::mat4 matrix;
	VAO vao;

	Mesh(
		std::vector<Vertex> &vertices,
		std::vector<GLuint> &indices,
		std::vector<Texture> &textures,
		glm::mat4 matrix = glm::mat4(1.0f),
		Material material = Material());
		

	void Draw(Shader &shader, Camera &camera);
};
#endif