#include "Model.h"

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 *from)
{
	glm::mat4 to;

	to[0][0] = (GLfloat)from->a1;
	to[0][1] = (GLfloat)from->b1;
	to[0][2] = (GLfloat)from->c1;
	to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2;
	to[1][1] = (GLfloat)from->b2;
	to[1][2] = (GLfloat)from->c2;
	to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3;
	to[2][1] = (GLfloat)from->b3;
	to[2][2] = (GLfloat)from->c3;
	to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4;
	to[3][1] = (GLfloat)from->b4;
	to[3][2] = (GLfloat)from->c4;
	to[3][3] = (GLfloat)from->d4;

	return to;
}

Material apply_material(const C_STRUCT aiMaterial *mtl)
{
	Material material;
	aiColor4D diffuse, ambient, specular;
	float metallic;
	float roughness;
	float opacity;

	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
	{
		material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
	}

	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
	{
		material.ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
	}

	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
	{
		material.specular = glm::vec3(specular.r, specular.g, specular.b);
	}

	if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_METALLIC_FACTOR, &metallic))
	{
		material.metallic = metallic;
	}

	if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_ROUGHNESS_FACTOR, &roughness))
	{
		material.roughness = roughness;
	}

	if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_OPACITY, &opacity))
	{
		material.opacity = opacity;
	}

	return material;
}

Model::Model(std::string const &path)
{
	loadModel(path);
}

void Model::UpdateTransformMatrix()
{
	transformMatrix = glm::mat4(1.0f);
	transformMatrix = glm::translate(transformMatrix, translate);
	transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transformMatrix = glm::scale(transformMatrix, scale);
}

void Model::Draw(Shader &shader, Camera &camera)
{

	for (unsigned int i = 0; i < nonTransparentMeshes.size(); ++i)
	{
		nonTransparentMeshes[i].Draw(shader, camera, transformMatrix);
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	for (unsigned int i = 0; i < transparentMeshes.size(); ++i)
	{
		transparentMeshes[i].Draw(shader, camera, transformMatrix);
	}

	glCullFace(GL_BACK);

	for (unsigned int i = 0; i < transparentMeshes.size(); ++i)
	{
		transparentMeshes[i].Draw(shader, camera, transformMatrix);
	}

	glDisable(GL_CULL_FACE);
}

void Model::loadModel(std::string const &path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene, glm::mat4(1.0f));
}

void Model::processNode(aiNode *node, const aiScene *scene, glm::mat4 parentTransform)
{
	glm::mat4 nodeTransform = parentTransform * aiMatrix4x4ToGlm(&node->mTransformation);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh m = processMesh(mesh, scene, nodeTransform);
		if (m.material.opacity < 1.0f)
			transparentMeshes.push_back(m);
		else
			nonTransparentMeshes.push_back(m);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene, nodeTransform);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene, glm::mat4 parentTransform)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Material mat = apply_material(scene->mMaterials[mesh->mMaterialIndex]);
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texUV = vec;
		}
		else
			vertex.texUV = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		assert(face.mNumIndices == 3);

		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", 0);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular", 1);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normal", 2);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "metallic", 3);
		textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
	}


	return Mesh(vertices, indices, textures, parentTransform, mat);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, GLuint slot)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;

		for (unsigned int j = 0; j < texturePaths.size(); ++j)
		{
			if (std::strcmp(texturePaths[j].c_str(), str.C_Str()) == 0)
			{
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			std::string texturePath = directory + '/' + std::string(str.C_Str());
			Texture texture(texturePath.c_str(), typeName.c_str(), slot);

			textures.push_back(texture);
			texturePaths.push_back(str.C_Str());
			texturesLoaded.push_back(texture);
		}
	}

	return textures;
}
