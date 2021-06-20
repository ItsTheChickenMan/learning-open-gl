// graphics

#ifndef PRACTICE_GRAPHICS_H
#define PRACTICE_GRAPHICS_H

#include <string>

#include <shader.h>
#include <types.h>
#include <camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// types
#define DIFFUSE_MAP 0
#define SPECULAR_MAP 1
#define EMISSION_MAP 2

// holds vertex data and VBO
struct Vertex_Data {
	float *vertexData; // vertex data
	u32 vertexCount; // amount of vertices
	
	u32 VBO; // vertex buffer object id of this data
	u32 VAO; // vertex array object id for vertex attributes (NOTE: maybe should be separate?)

	u32 *indices;
	u32 indicesCount;
	
	u32 EBO; // element buffer object id
	bool usingEBO; // whether or not this vertex data uses EBO (for drawVertexData calls)
};

// texture data
struct Texture_Data {
	s32 width;
	s32 height;
	s32 channels;
	
	u32 texture; // id of opengl texture buffer
	
	std::string path;
};

struct Material {
	// TODO: multiple textures
	u32 diffuseMaps[8]; // texture id
	int diffuseCount; // whether or not texture is bound
	
	u32 specularMaps[8]; // texture id
	int specularCount; // whether or not texture is bound
	
	u32 emissionMaps[8]; // texture id
	int emissionCount; // whether or not texture is bound
	
	glm::vec3 color;
	
	float shininess; // the literal shininess value in specular equation
	float specularStrength; // how strong specular reflections are (0-1, 0 being none at all and 1 being full highlights)
};

// a light in a certain position with no falloff
struct Light {
	glm::vec3 position; // position
	
	glm::vec3 color; // (this is color)
	
	float ambient; // ambient brightness
	float diffuse; // diffuse brightness
	float specular; // specular brightness
};

// an infinitely far away light pointing in a single direction
struct DirectionalLight {
	glm::vec3 direction; // position
	
	glm::vec3 color; // (this is color)
	
	float ambient; // ambient brightness
	float diffuse; // diffuse brightness
	float specular; // specular brightness
};

// a light in a position with a set amount of falloff according to the equation 1.0/constant + dist*linear + dist^2*quadratic
struct PointLight {
	glm::vec3 position; // position
	
	// attenuation factors
	float constant;
	float linear;
	float quadratic;
	
	glm::vec3 color; // (this is color)
	
	float ambient; // ambient brightness
	float diffuse; // diffuse brightness
	float specular; // specular brightness
};

struct SpotLight {
	glm::vec3 position; // position
	glm::vec3 direction; // direction
	
	float angle; // angle of cone
	float outerAngle; // outer angle of cone
	
	// attenuation factors
	float constant;
	float linear;
	float quadratic;
	
	glm::vec3 color; // (this is color)
	
	float ambient; // ambient brightness (not color)
	float diffuse; // diffuse brightness (not color)
	float specular; // specular brightness (not color)
};

// a 3 dimensional object which uses a Vertex_Data as the base and has a position, and rotation
struct Object_Data {
	Vertex_Data *vertexData; // vertex data to reference when drawing
	
	glm::vec3 position; // position
	glm::vec3 rotation; // rotation
	glm::vec3 scale;
	
	glm::mat4 modelMatrix; // model matrix used for transformations
	
	Material material; // material
};


Vertex_Data createVertexData(float *vertexData, u32 vertexCount, u32 dataSize);
Vertex_Data createVertexData(float *vertexData, u32 vertexCount, u32 dataSize, u32 *indices, u32 indicesCount, u32 indicesSize);
void drawVertexData(Vertex_Data *data, ShaderProgram *shaderProgram);

Texture_Data createTexture(const char* path);

Material createMaterial(glm::vec3 color, float shininess, float specularStrength);
void bindTextureToMaterial(Material *material, Texture_Data *textureData, int type);
void setUniformMaterial(Material *material, ShaderProgram program);

Light createLight(glm::vec3 position, glm::vec3 color, float ambient, float diffuse, float specular);
void setUniformLight(Light *light, ShaderProgram program);
DirectionalLight createDirectionalLight(glm::vec3 direction, glm::vec3 color, float ambient, float diffuse, float specular);
void setUniformDirectionalLight(DirectionalLight *light, ShaderProgram program, const char* buffer);
PointLight createPointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 color, float ambient, float diffuse, float specular);
void setUniformPointLight(PointLight *light, ShaderProgram program, const char* buffer);
SpotLight createSpotLight(glm::vec3 position, glm::vec3 direction, float angle, float outerAngle, float constant, float linear, float quadratic, glm::vec3 color, float ambient, float diffuse, float specular);
void setUniformSpotLight(SpotLight *light, ShaderProgram program, const char* buffer);

void pushPointLight(ShaderProgram program, PointLight *light);
void resetPointLights();
void pushSpotLight(ShaderProgram program, SpotLight *light);
void resetSpotLights();
void pushDirectionalLight(ShaderProgram program, DirectionalLight *light);
void resetDirectionalLights();

Object_Data createObjectData(Vertex_Data *vertexData, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Material *material);
void updateObjectData(Object_Data *object);
void drawObjectData(Object_Data *object, Camera *camera, ShaderProgram *program);

#endif