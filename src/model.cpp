// models (collection of meshes)

#include <model.h>

// internal texture cache
static std::vector<Texture_Data> textureCache;

// load a model from a path
Model loadModel(std::string path){
	Model model;
	
	model.position = glm::vec3(0, 0, 0);
	model.rotation = glm::vec3(0, 0, 0);
	model.scale = glm::vec3(1, 1, 1);
	
	Assimp::Importer importer;
	
	printf("loading model %s...\n", path.c_str());
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		printf("error (assimp): %s\n", importer.GetErrorString());
		return model;
	}
	
	printf("model loaded\n");
	
	model.path = path.substr(0, path.find_last_of('/'));
	
	printf("parsing model %s...\n", path.c_str());
	processAssimpNode(&model, scene->mRootNode, scene);
	printf("parsed.\n");
	
	return model;
}

void processAssimpNode(Model *model, aiNode *node, const aiScene *scene){
	// loop through each mesh in this node and process them
	for(int i = 0; i < node->mNumMeshes; i++){
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		model->meshes.push_back(processAssimpMesh(mesh, scene, model->path));
	}
	
	// loop through each child of this node
	for(int i = 0; i < node->mNumChildren; i++){
		processAssimpNode(model, node->mChildren[i], scene);
	}
}

Object_Data processAssimpMesh(aiMesh *mesh, const aiScene *scene, std::string path){
	float *vertices = (float*)malloc(mesh->mNumVertices * 8 * sizeof(float)); // 3 + 2 + 3 = 8
	
	// allocation work (# indices dependent on # faces)
	int numIndices = 0;
	for(int i = 0; i < mesh->mNumFaces; i++){
		numIndices += mesh->mFaces[i].mNumIndices;
	}
	
	u32 *indices = (u32*)malloc(numIndices * sizeof(u32));
	
	
	// process vertices
	int verticesIndex = 0; // index in buffer
	
	for(int i = 0; i < mesh->mNumVertices; i++){
		// push vertex coordinates
		vertices[verticesIndex++] = mesh->mVertices[i].x;
		vertices[verticesIndex++] = mesh->mVertices[i].y;
		vertices[verticesIndex++] = mesh->mVertices[i].z;
		
		// push texture coordinates (if they exist)
		if(mesh->mTextureCoords[0]){
			vertices[verticesIndex++] = mesh->mTextureCoords[0][i].x;
			vertices[verticesIndex++] = mesh->mTextureCoords[0][i].y;
		} else {
			vertices[verticesIndex++] = 0;
			vertices[verticesIndex++] = 0;
		}
		
		// push normals
		vertices[verticesIndex++] = mesh->mNormals[i].x;
		vertices[verticesIndex++] = mesh->mNormals[i].y;
		vertices[verticesIndex++] = mesh->mNormals[i].z;
	}
	
	// process indices
	int indicesIndex = 0;
	for(int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		
		for(int j = 0; j < face.mNumIndices; j++){
			indices[indicesIndex++] = face.mIndices[j];
		}
	}
	
	// create vertex data
	// NOTE: mesh->mNumVertices?
	Vertex_Data vertexData = createVertexData(vertices, mesh->mNumVertices, mesh->mNumVertices * 8 * sizeof(float), indices, numIndices, numIndices * sizeof(u32));

	// process materials
	Material material = createMaterial(glm::vec3(1.0f, 1.0f, 1.0f), 64, 1.0);
	
	if(mesh->mMaterialIndex >= 0){
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		
		// diffuse textures
		bindAssimpTexturesToMaterial(&material, mat, aiTextureType_DIFFUSE, DIFFUSE_MAP, path);
		
		// specular textures
		bindAssimpTexturesToMaterial(&material, mat, aiTextureType_SPECULAR, SPECULAR_MAP, path);
	}
	
	// TODO: vertex data not pointer
	Object_Data finalMesh = createObjectData(&vertexData, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), &material);

	return finalMesh;
}

void bindAssimpTexturesToMaterial(Material *material, aiMaterial *assimpMat, aiTextureType type, int intType, std::string modelDirectory){
	for(int i = 0; i < assimpMat->GetTextureCount(type); i++){
		aiString path;
		assimpMat->GetTexture(type, i, &path);
		
		bool usedCached = false;
		for(int j = 0; j < textureCache.size(); j++){
			if(strcmp( textureCache[j].path.c_str(), (modelDirectory + "/" + path.C_Str()).c_str() ) == 0){
				bindTextureToMaterial(material, &textureCache[j], intType);
				usedCached = true;
				break;
			}
		}
		
		if(!usedCached){
			Texture_Data texture = createTexture( (modelDirectory + "/" + path.C_Str()).c_str() );
			textureCache.push_back(texture);
			
			bindTextureToMaterial(material, &texture, intType);
		}
	}
}

// updates the position of all contained meshes (NOTE: do NOT call updateObjectData on a mesh if it is within a model!)
void updateModel(Model *model){
	for(int i = 0; i < model->meshes.size(); i++){
		glm::vec3 tempPosition = model->meshes[i].position;
		glm::vec3 tempRotation = model->meshes[i].rotation;
		glm::vec3 tempScale = model->meshes[i].scale;
		
		model->meshes[i].position += model->position;
		model->meshes[i].rotation += model->rotation;
		model->meshes[i].scale += model->scale;
	
		updateObjectData(&model->meshes[i]);
		
		model->meshes[i].position = tempPosition;
		model->meshes[i].rotation = tempRotation;
		model->meshes[i].scale = tempScale;
	}
}

void drawModel(Model *model, Camera *camera, ShaderProgram *program){
	useShader(program);
	
	setUniformMat4(*program, "projection", camera->projection);
	setUniformMat4(*program, "view", camera->view);
	
	for(int i = 0; i < model->meshes.size(); i++){
		/*Object_Data *object = &model->meshes[i];
		
		setUniformMat4(*program, "model", object->modelMatrix);
	
		setUniformMat3(*program, "normalMatrix", glm::mat3(glm::transpose(glm::inverse(object->modelMatrix))) );
	
		setUniformMaterial(&object->material, *program);
		
		char locationBuffer[64];
		int currentTexture = 0;
		
		// bind textures
		for(int i = 0; i < object->material.diffuseCount || 0; i++){
			glActiveTexture(GL_TEXTURE0 + currentTexture);
			glBindTexture(GL_TEXTURE_2D, object->material.diffuseMaps[i]);
			
			strcpy(locationBuffer, "");
			sprintf(locationBuffer, "material.diffuseMaps[%d]", i);
			setUniformInt(*program, locationBuffer, currentTexture);
			
			currentTexture++;
		}
		
		// bind specular maps
		for(int i = 0; i < object->material.specularCount; i++){
			glActiveTexture(GL_TEXTURE0 + currentTexture);
			glBindTexture(GL_TEXTURE_2D, object->material.specularMaps[i]);
			
			strcpy(locationBuffer, "");
			sprintf(locationBuffer, "material.specularMaps[%d]", i);
			setUniformInt(*program, locationBuffer, currentTexture);
			
			currentTexture++;
		}
		
		// bind emission maps
		for(int i = 0; i < object->material.emissionCount || 0; i++){
			glActiveTexture(GL_TEXTURE0 + currentTexture);
			glBindTexture(GL_TEXTURE_2D, object->material.emissionMaps[i]);
			
			strcpy(locationBuffer, "");
			sprintf(locationBuffer, "material.emissionMaps[%d]", i);
			setUniformInt(*program, locationBuffer, currentTexture);
			
			currentTexture++;
		}
		
		drawVertexData(object->vertexData, program);*/
		
		drawObjectData(&model->meshes[i], camera, program);
	}
}