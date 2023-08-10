#ifndef SHADOW_CLASS_H
#define SHADOW_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "Light.h"
#include <vector>

using namespace std;

class Shadow
{
public:
	GLuint depthMapFBO;
	GLuint depthMap;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	Shadow();

	void setUniforms(Shader shader, Light light, float near_plane, float far_plane);
	void Activate(Shader shader, unsigned int textureUnit);
	void Bind();
	void Unbind();
	void Delete();
};

#endif
