// main graphics functions

#include <graphics.h>

#include <cstdio>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

// VERTEX_DATA STUFF //

// create vertex data
Vertex_Data createVertexData(float *vertexData, u32 vertexCount, u32 dataSize){
	Vertex_Data data;
	
	// does not yet have a texture
	data.hasTexture = false;
	
	// no EBO
	data.usingEBO = false;
	
	// assign data
	data.vertexData = vertexData;
	data.vertexCount = vertexCount;
	
	// generate VAO
	glGenVertexArrays(1, &data.VAO);
	
	// generate VBO
	glGenBuffers(1, &data.VBO);
	
	// assign vertex data to vbo
	glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data.vertexData, GL_STATIC_DRAW);
	
	// apply vertex attributes
	glBindVertexArray(data.VAO);
	
	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	
	// texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	
	// normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	
	// enable attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	return data;
}

// overload which allows also for the specification of an EBO, which causes the Vertex_Data to be drawn using indices
Vertex_Data createVertexData(float *vertexData, u32 vertexCount, u32 dataSize, u32 *indices, u32 indicesCount, u32 indicesSize){
	Vertex_Data data;
	
	// does not yet have a texture
	data.hasTexture = false;
	
	// using EBO
	data.usingEBO = true;
	
	// assign data
	data.vertexData = vertexData;
	data.vertexCount = vertexCount;
	
	data.indices = indices;
	data.indicesCount = indicesCount;
	
	// generate buffers
	glGenVertexArrays(1, &data.VAO); // generate VAO
	glGenBuffers(1, &data.VBO); // generate VBO
	glGenBuffers(1, &data.EBO); // generate EBO
	
	// assign vertex data to vbo
	glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data.vertexData, GL_STATIC_DRAW);
	
	// apply vertex attributes
	glBindVertexArray(data.VAO);
	
	// assign indices values
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, data.indices, GL_STATIC_DRAW);
	
	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	
	// texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	
	// normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	
	// enable attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	return data;
}

// bind a texture to vertex data (you might've guessed)
void bindTextureToVertexData(Vertex_Data *vertexData, Texture_Data *textureData, int type){
	if(type == DIFFUSE_MAP){
		vertexData->texture = textureData->texture;
		vertexData->hasTexture = true;
	} else if(type == SPECULAR_MAP){
		vertexData->specularMap = textureData->texture;
		vertexData->hasSpecularMap = true;
	} else if(type == EMISSION_MAP){
		vertexData->emissionMap = textureData->texture;
		vertexData->hasEmissionMap = true;
	}
}

// draw vertex data using shaderProgram assuming shader takes no matrices for transformations (generally unused)
void drawVertexData(Vertex_Data *data, ShaderProgram *shaderProgram){
	useShader(shaderProgram);
	
	// bind texture if available
	if(data->hasTexture){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data->texture);
	}
	
	// bind specular map if available
	if(data->hasSpecularMap){
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, data->specularMap);
	}
	
	// bind emission map if available
	if(data->hasEmissionMap){
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, data->emissionMap);
	}
	
	glBindVertexArray(data->VAO);
	
	if(!data->usingEBO)
		glDrawArrays(GL_TRIANGLES, 0, data->vertexCount);
	else
		glDrawElements(GL_TRIANGLES, data->indicesCount, GL_UNSIGNED_INT, 0);
}

// TEXTURE MANAGEMENT //

// create a texture
Texture_Data createTexture(char* path){
	Texture_Data textureData;
	
	// load texture data
	stbi_set_flip_vertically_on_load(true); // flip because opengl expects textures to start at end of buffer
	
	u8 *data = stbi_load(path, &textureData.width, &textureData.height, &textureData.channels, 0);
	
	if(data){
		// create texture
		glGenTextures(1, &textureData.texture);
		
		// bind texture
		glBindTexture(GL_TEXTURE_2D, textureData.texture); // bind texture so function calls affect it
		
		// assign parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// generate texture
		if(textureData.channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureData.width, textureData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(textureData.channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureData.width, textureData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		// generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		printf("error loading texture %s\n", path);
	}
	
	stbi_image_free(data);
	
	return textureData;
}

// MATERIAL //

// create material
Material createMaterial(glm::vec3 color, float shininess, float specularStrength){
	Material material;
	
	material.color = color;
	material.shininess = shininess;
	material.specularStrength = specularStrength;
	
	return material;
}

// TODO: change naming conventions and stuff
void setUniformMaterial(Material *material, ShaderProgram program){
	float color[] = {material->color.x, material->color.y, material->color.z};
	
	setUniformFloat(program, "material.color", color, 3);
	setUniformFloat(program, "material.shininess", &material->shininess, 1);
	setUniformFloat(program, "material.specularStrength", &material->specularStrength, 1);
}

// LIGHTS //

// lights

// there's a pretty good chance I'm going to remove the specular component, since specular highlights is not something a light can control.  Use material's specular settings instead
Light createLight(glm::vec3 position, glm::vec3 color, float ambient, float diffuse, float specular){
	Light light;
	
	light.position = position;
	light.color = color;
	
	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;

	return light;
}

// TODO: change naming conventions and stuff
void setUniformLight(Light *light, ShaderProgram program){
	float position[] = {light->position.x, light->position.y, light->position.z};
	float color[] = {light->color.x, light->color.y, light->color.z};
	
	setUniformFloat(program, "light.position", position, 3);
	setUniformFloat(program, "light.color", color, 3);
	setUniformFloat(program, "light.ambient", &light->ambient, 1);
	setUniformFloat(program, "light.diffuse", &light->diffuse, 1);
	setUniformFloat(program, "light.specular", &light->specular, 1);
}

// directional lights

DirectionalLight createDirectionalLight(glm::vec3 direction, glm::vec3 color, float ambient, float diffuse, float specular){
	DirectionalLight light;
	
	light.direction = direction;
	light.color = color;
	
	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;

	return light;
}

void setUniformDirectionalLight(DirectionalLight *light, ShaderProgram program, const char* buffer){
	float direction[] = {light->direction.x, light->direction.y, light->direction.z};
	float color[] = {light->color.x, light->color.y, light->color.z};
	
	char location[64];
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".direction"), direction, 3);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".color"), color, 3);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".ambient"), &light->ambient, 1);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".diffuse"), &light->diffuse, 1);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".specular"), &light->specular, 1);
	
	strcpy(location, buffer);
	setUniformInt(program, strcat(location, ".exists"), 1);
}

// point lights

PointLight createPointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 color, float ambient, float diffuse, float specular){
	PointLight light;
	
	light.position = position;
	light.color = color;
	
	light.constant = constant;
	light.linear = linear;
	light.quadratic = quadratic;
	
	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;

	return light;
}

void setUniformPointLight(PointLight *light, ShaderProgram program, const char* buffer){
	float position[] = {light->position.x, light->position.y, light->position.z};
	float color[] = {light->color.x, light->color.y, light->color.z};
	
	char location[64];
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".position"), position, 3);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".color"), color, 3);
	
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".constant"), light->constant);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".linear"), light->linear);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".quadratic"), light->quadratic);
	
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".ambient"), &light->ambient, 1);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".diffuse"), &light->diffuse, 1);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".specular"), &light->specular, 1);
	
	strcpy(location, buffer);
	setUniformInt(program, strcat(location, ".exists"), 1);
}

SpotLight createSpotLight(glm::vec3 position, glm::vec3 direction, float angle, float outerAngle, float constant, float linear, float quadratic, glm::vec3 color, float ambient, float diffuse, float specular){
	SpotLight light;
	
	light.position = position;
	light.direction = direction;
	light.angle = angle; // radians!
	light.outerAngle = outerAngle; // ditto!
	
	light.color = color;

	light.constant = constant;
	light.linear = linear;
	light.quadratic = quadratic;
	
	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;

	return light;
}

void setUniformSpotLight(SpotLight *light, ShaderProgram program, const char* buffer){
	float position[] = {light->position.x, light->position.y, light->position.z};
	float direction[] = {light->direction.x, light->direction.y, light->direction.z};
	float color[] = {light->color.x, light->color.y, light->color.z};
	
	char location[64];
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".position"), position, 3);
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".direction"), direction, 3);
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".color"), color, 3);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".constant"), light->constant);
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".linear"), light->linear);
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".quadratic"), light->quadratic);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".angle"), light->angle);
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".outerAngle"), light->outerAngle);
	
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".ambient"), &light->ambient, 1);
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".diffuse"), &light->diffuse, 1);
	strcpy(location, buffer);
	setUniformFloat(program, strcat(location, ".specular"), &light->specular, 1);
	
	strcpy(location, buffer);
	setUniformInt(program, strcat(location, ".exists"), 1);
}

// SHADER LIGHT MANAGEMENT //

static u16 MAX_POINT_LIGHTS = 16;
static u16 MAX_SPOT_LIGHTS = 16;
static u16 MAX_DIRECTIONAL_LIGHTS = 16;

static s32 currentPointLight = 0;
static s32 currentSpotLight = 0;
static s32 currentDirectionalLight = 0;

// push a point light
void pushPointLight(ShaderProgram program, PointLight *light){
	if(currentPointLight >= MAX_POINT_LIGHTS){
		printf("Attempted to push PointLight when max (%d) was reached\n", MAX_POINT_LIGHTS);
		return;
	}
	
	char buffer[64];
	
	sprintf(buffer, "pointLights[%d]", currentPointLight);
  setUniformPointLight(light, program, buffer);
	
	currentPointLight++;
}

// reset counter to 0 (doesn't necessarily get rid of lights, need to explicitly be written over)
void resetPointLights(){
	currentPointLight = 0;
}

void pushSpotLight(ShaderProgram program, SpotLight *light){
	if(currentSpotLight >= MAX_SPOT_LIGHTS){
		printf("Attempted to push SpotLight when max (%d) was reached\n", MAX_SPOT_LIGHTS);
		return;
	}
	
	char buffer[64];
	
	sprintf(buffer, "spotLights[%d]", currentSpotLight);
  setUniformSpotLight(light, program, buffer);
	
	currentSpotLight++;
}

void resetSpotLights(){
	currentSpotLight = 0;
}

void pushDirectionalLight(ShaderProgram program, DirectionalLight *light){
	if(currentDirectionalLight >= MAX_DIRECTIONAL_LIGHTS){
		printf("Attempted to push DirectionalLight when max (%d) was reached\n", MAX_DIRECTIONAL_LIGHTS);
		return;
	}
	
	char buffer[64];
	
	sprintf(buffer, "directionalLights[%d]", currentDirectionalLight);
  setUniformDirectionalLight(light, program, buffer);
	
	currentDirectionalLight++;
}

void resetDirectionalLights(){
	currentDirectionalLight = 0;
}

// 3D OBJECTS //

// create a 3d object
Object_Data createObjectData(Vertex_Data *vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Material *material){
	Object_Data object;
	
	// assign vertex data
	object.vertexData = vertexData;

	// assign position/rotation
	object.position = position;
	object.rotation = rotation;
	object.scale = scale;
	
	// create model matrix (stays as an identity matrix until updated)
	object.modelMatrix = glm::mat4(1.0f);
	
	object.material = *material;
	
	return object;
}

// updates Object_Data model matrix according to position and rotation (for efficiency, only call if the position/rotation of Object changes)
void updateObjectData(Object_Data *object){
	// TODO: probably a more efficient way to do this than creating a new matrix each time
	
	// revert to identity matrix
	object->modelMatrix = glm::mat4(1.0f);
		
	// update position
	object->modelMatrix = glm::translate(object->modelMatrix, object->position);
	
	// update rotation
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	object->modelMatrix = glm::rotate(object->modelMatrix, object->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	
	object->modelMatrix = glm::scale(object->modelMatrix, object->scale);
}

// draw object from the perspective of camera with shaderProgram
// assumes shader program has a uniform for projection, view, and model matrices
void drawObjectData(Object_Data *object, Camera *camera, ShaderProgram *program){
	// assign matrices to shader
	useShader(program);
	
	setUniformMat4(*program, "projection", camera->projection);
	setUniformMat4(*program, "view", camera->view);
	setUniformMat4(*program, "model", object->modelMatrix);
	
	setUniformMat3(*program, "normalMatrix", glm::mat3(glm::transpose(glm::inverse(object->modelMatrix))) );
	
	float color[] = {object->material.color.x, object->material.color.y, object->material.color.z};
	
	setUniformMaterial(&object->material, *program);
	
	drawVertexData(object->vertexData, program);
}