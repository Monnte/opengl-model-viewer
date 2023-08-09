#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include "Mesh.h"
#include "assimp/Importer.hpp"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <assimp/pbrmaterial.h>


class Model
{
public:
	Model(std::string const &path);
	void Draw(Shader &shader, Camera &camera);

private:
	std::vector<Mesh> nonTransparentMeshes;
	std::vector<Mesh> transparentMeshes;
	std::string directory;
	std::vector<Texture> texturesLoaded;
	std::vector<std::string> texturePaths;

	std::vector<glm::mat4> matricesMeshes;

	void loadModel(std::string const &path);
	void processNode(aiNode *node, const aiScene *scene, glm::mat4 parentTransform);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene, glm::mat4 parentTransform);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, GLuint slot);
};

#endif