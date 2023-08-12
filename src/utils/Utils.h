#ifndef UTILS_H
#define UTILS_H


#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaderClass.h"

using namespace std;

unsigned char* loadTexture(const char *image, int *width, int *height, int *numColCh, int reqComp);
void freeTexture(unsigned char *tex);
unsigned int loadBRDFLUT(const char *image);
unsigned int loadCubemap(vector<std::string> faces);
void renderCube();
void precomputeIBL(unsigned int *irradianceMap, unsigned int *prefilterMap, unsigned int skyboxTexture, Shader &irradianceShader, Shader &prefilterShader);

#endif