#include "Skybox.h"

Skybox::Skybox(std::string path)
{

	std::vector<std::string> facesCubemap = {
		path + "/px.png",
		path + "/nx.png",
		path + "/py.png",
		path + "/ny.png",
		path + "/pz.png",
		path + "/nz.png"
	};

	std::vector<Vertex> vertices = {
		Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0), glm::vec2(0)},
		Vertex{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0), glm::vec2(0)},
		Vertex{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0), glm::vec2(0)},
		Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0), glm::vec2(0)},
		Vertex{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0), glm::vec2(0)},
		Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0), glm::vec2(0)},
		Vertex{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec2(0)},
		Vertex{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0), glm::vec2(0)}};

	std::vector<GLuint> indices = {
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3};

	vao.Bind();
	VBO vbo(vertices);
	EBO ebo(indices);

	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();

	textureID = loadCubemap(facesCubemap);
}

void Skybox::Draw(Shader &skyboxShader, Camera &camera)
{
	skyboxShader.Activate();
	vao.Bind();

	glDepthFunc(GL_LEQUAL);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::mat4(glm::mat3(camera.viewMatrix));
	projection = camera.projectionMatrix;

	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}