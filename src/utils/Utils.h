#ifndef UTILS_H
#define UTILS_H


#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

unsigned char* loadTexture(const char *image, int *width, int *height, int *numColCh, int reqComp);
void freeTexture(unsigned char *tex);

unsigned int loadCubemap(vector<std::string> faces);



#endif