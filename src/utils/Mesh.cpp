#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures, glm::mat4 matrix, Material material)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	Mesh::matrix = matrix;
	Mesh::material = material;

	vao.Bind();
	VBO vbo(vertices);
	EBO ebo(indices);

	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
	vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float)));
	vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));

	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();
}

void Mesh::Draw(Shader &shader, Camera &camera)
{
	shader.Activate();
	vao.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	unsigned int numNormal = 0;
	unsigned int metallic = 0;

	glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), false);
	glUniform1i(glGetUniformLocation(shader.ID, "hasNormalMap"), false);
	glUniform1i(glGetUniformLocation(shader.ID, "hasSpecularMap"), false);
	glUniform1i(glGetUniformLocation(shader.ID, "hasMetalicRoughnessMap"), false);

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
			glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), true);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
			glUniform1i(glGetUniformLocation(shader.ID, "hasSpecularMap"), true);
		}
		else if (type == "normal")
		{
			num = std::to_string(numNormal++);
			glUniform1i(glGetUniformLocation(shader.ID, "hasNormalMap"), true);
		}
		else if (type == "metallic")
		{
			num = std::to_string(metallic++);
			glUniform1i(glGetUniformLocation(shader.ID, "hasMetalicRoughnessMap"), true);
		}
		else
		{
			std::cout << "ERROR: Texture type not recognized: " << type << std::endl;
		}

		textures[i].texUnit(shader, (type + num).c_str(), textures[i].unit);
		textures[i].Bind();
	}

	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");

	glUniform3f(glGetUniformLocation(shader.ID, "materialAmbientColor"), material.ambient.x, material.ambient.y, material.ambient.z);
	glUniform3f(glGetUniformLocation(shader.ID, "materialDiffuseColor"), material.diffuse.x, material.diffuse.y, material.diffuse.z);
	glUniform3f(glGetUniformLocation(shader.ID, "materialSpecularColor"), material.specular.x, material.specular.y, material.specular.z);
	glUniform1f(glGetUniformLocation(shader.ID, "materialMetallic"), material.metallic);
	glUniform1f(glGetUniformLocation(shader.ID, "materialRoughness"), material.roughness);
	glUniform1f(glGetUniformLocation(shader.ID, "opacity"), material.opacity);
	

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}