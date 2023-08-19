// models (collection of meshes)

#ifndef PRACTICE_MODEL_H
#define PRACTICE_MODEL_H

#include <vector>
#include <string>

#include <graphics.h>
#include <shader.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Model {
	std::vector<Object_Data> meshes;
	std::string path; // I don't like to use std::string, but when it comes to strings in structs it just gets too complicated when I just want a quick test thing
	
	// transformations (no model matrix because only object data is rendered)
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

Model loadModel(std::string path);
Model loadModel(std::string path, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
void processAssimpNode(Model *model, aiNode *node, const aiScene *scene);
Object_Data processAssimpMesh(aiMesh *mesh, const aiScene *scene, std::string path);
void bindAssimpTexturesToMaterial(Material *material, aiMaterial *assimpMat, aiTextureType type, int intType, std::string modelDirectory);
void updateModel(Model *model);
void drawModel(Model *model, Camera *camera, ShaderProgram *program);

#endif