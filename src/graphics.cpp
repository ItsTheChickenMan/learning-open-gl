// main graphics functions

#include <graphics.h>

#include <cstdio>
#include <cstring>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

// VERTEX_DATA STUFF //

// create vertex data
Vertex_Data createVertexData(float *vertexData, u32 vertexCount, u32 dataSize){
	Vertex_Data data;
	
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

// draw vertex data using shaderProgram assuming shader takes no matrices for transformations (generally unused)
void drawVertexData(Vertex_Data *data, ShaderProgram *shaderProgram){
	useShader(shaderProgram);
	
	glBindVertexArray(data->VAO);
	
	if(!data->usingEBO)
		glDrawArrays(GL_TRIANGLES, 0, data->vertexCount);
	else
		glDrawElements(GL_TRIANGLES, data->indicesCount, GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
}

// TEXTURE MANAGEMENT //

// create a texture
Texture_Data createTexture(const char* path, bool sRGB){
	Texture_Data textureData;
	
	textureData.path = std::string(path);
	
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

		GLint internalFormat = GL_RGB;
		
		// TODO: this is dumb
		if(sRGB){
			if(textureData.channels == 3){
				internalFormat = GL_SRGB;
			} else {
				internalFormat = GL_SRGB_ALPHA;
			}
		} else {
			if(textureData.channels == 4){
				internalFormat = GL_RGBA;
			}
		}
		
		// generate texture
		if(textureData.channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureData.width, textureData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(textureData.channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureData.width, textureData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		// generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
	} else {
		printf("error loading texture %s\n", path);
	}
	
	stbi_image_free(data);
	
	return textureData;
}

// create a texture without predefined data (aka a texture path) (usually used for RenderableBuffers)
Texture_Data createTexture(s32 width, s32 height, GLenum format){
	Texture_Data textureData;
	
	textureData.width = width;
	textureData.height = height;
	
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
	glTexImage2D(GL_TEXTURE_2D, 0, format, textureData.width, textureData.height, 0, format, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	return textureData;
}

u32 createCubemap(std::vector<std::string> paths){
	u32 cubemap;
	
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	
	s32 width, height, channels;
	u8 *data;
	
	stbi_set_flip_vertically_on_load(false); // flip because opengl expects textures to start at end of buffers
	
	for(u32 i = 0; i < paths.size(); i++){
		data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
		
		if(data){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else {
			printf("Error loading skybox: couldn't load texture data\n");
		}
		
		stbi_image_free(data);
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	return cubemap;
}

// putting this in here cause I can't think of a better place for it
// generate a depth framebuffer used for shadow mapping
DepthBuffer generateDepthMap(s32 width, s32 height){
	DepthBuffer buffer;

	glGenFramebuffers(1, &buffer.FBO);
	
	glGenTextures(1, &buffer.map);
	
	glBindTexture(GL_TEXTURE_2D, buffer.map);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, buffer.FBO);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	// make sure framebuffer is complete
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		printf("There was an error creating DepthMap, framebuffer is incomplete: ");
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT){
			printf("Incomplete attachment\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE){
			printf("Multisample err (see docs)\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT){
			printf("Missing attachment\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_UNSUPPORTED){
			printf("Unsupported attachments\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS){
			printf("Layer targets (see docs)\n");
		} else {
			printf("Unknown error\n");
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return buffer;
}

DepthBuffer generateDepthCubemap(s32 width, s32 height){
	DepthBuffer buffer;
	
	glGenFramebuffers(1, &buffer.FBO);
	
	glGenTextures(1, &buffer.map);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer.map);
	
	for(unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, buffer.FBO);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer.map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	// make sure framebuffer is complete
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		printf("There was an error creating DepthMap, framebuffer is incomplete: ");
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT){
			printf("Incomplete attachment\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE){
			printf("Multisample err (see docs)\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT){
			printf("Missing attachment\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_UNSUPPORTED){
			printf("Unsupported attachments\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS){
			printf("Layer targets (see docs)\n");
		} else {
			printf("Unknown error\n");
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return buffer;
}

void drawCubemap(u32 cubemap, Vertex_Data *cube_data, Camera *camera, ShaderProgram program){
	glm::mat4 view = glm::mat4(glm::mat3(camera->view));
	
	useShader(&program);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);	
	
	setUniformMat4(program, "projection", camera->projection);
	setUniformMat4(program, "view", view);
	
	glBindVertexArray(cube_data->VAO);
	glDrawArrays(GL_TRIANGLES, 0, cube_data->vertexCount);
	
	glBindVertexArray(0);
}

// create directional light shadow caster
ShadowCaster createShadowCaster(DirectionalLight *lightSource){
	ShadowCaster caster;
	
	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.5f);
	glm::vec3 viewPos = glm::vec3(0, -1, -8);
	glm::mat4 view = glm::lookAt(viewPos, viewPos + glm::normalize(lightSource->direction), glm::vec3(0, 1, 0));
	
	caster.lightSpace = projection * view;
	
	caster.depthBuffer = generateDepthMap(1024, 1024);
	
	return caster;
}

// create a point light shadow caster
ShadowCaster createShadowCaster(PointLight *lightSource){
	ShadowCaster caster;
	
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 25.0f);
	
	caster.lightSpace =  projection * glm::lookAt(lightSource->position, lightSource->position + glm::vec3( 1, 0, 0), glm::vec3(0, -1, 0));
	caster.lightSpace2 = projection * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
	caster.lightSpace3 = projection * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0,  1, 0), glm::vec3(0, 0,  1));
	caster.lightSpace4 = projection * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
	caster.lightSpace5 = projection * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0, 0,  1), glm::vec3(0, -1, 0));
	caster.lightSpace6 = projection * glm::lookAt(lightSource->position, lightSource->position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
	
	caster.depthBuffer = generateDepthCubemap(1024, 1024);
	
	return caster;
}

void recalculateLightSpace(ShadowCaster *caster, DirectionalLight *lightSource){
	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.5f);
	glm::vec3 viewPos = glm::vec3(0, -1, -8);
	glm::mat4 view = glm::lookAt(viewPos, viewPos + glm::normalize(lightSource->direction), glm::vec3(0, 1, 0));
	
	caster->lightSpace = projection * view;
}

RenderableBuffer createRenderableBuffer(s32 width, s32 height){
	RenderableBuffer buffer;
	
	// create framebuffer
	glGenFramebuffers(1, &buffer.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer.FBO);
	
	// create/bind color buffer
	buffer.colorBuffer = createTexture(width, height, GL_RGB);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer.texture, 0);

	// create renderbuffer
	glGenRenderbuffers(1, &buffer.RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	// attach to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.RBO);
	
	// make sure framebuffer is complete
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		printf("There was an error creating RenderableBuffer, framebuffer is incomplete: ");
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT){
			printf("Incomplete attachment\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE){
			printf("Multisample err (see docs)\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT){
			printf("Missing attachment\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_UNSUPPORTED){
			printf("Unsupported attachments\n");
		} else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS){
			printf("Layer targets (see docs)\n");
		} else {
			printf("Unknown error\n");
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return buffer;
}

// MATERIAL //

// create material
Material createMaterial(glm::vec3 color, float shininess, float specularStrength){
	Material material;
	
	material.color = color;
	material.shininess = shininess;
	material.specularStrength = specularStrength;
	
	material.diffuseCount = 0;
	material.specularCount = 0;
	material.emissionCount = 0;
	
	return material;
}

// bind a texture to material (you might've guessed)
void bindTextureToMaterial(Material *material, Texture_Data *textureData, int type){
	if(type == DIFFUSE_MAP){
		material->diffuseMaps[material->diffuseCount++] = textureData->texture;
	} else if(type == SPECULAR_MAP){
		material->specularMaps[material->specularCount++] = textureData->texture;
	} else if(type == EMISSION_MAP){
		material->emissionMaps[material->emissionCount++] = textureData->texture;
	}
}

// TODO: change naming conventions and stuff
void setUniformMaterial(Material *material, ShaderProgram program){
	float color[] = {material->color.x, material->color.y, material->color.z};
	
	setUniformFloat(program, "material.color", color, 3);
	setUniformFloat(program, "material.shininess", &material->shininess, 1);
	setUniformFloat(program, "material.specularStrength", &material->specularStrength, 1);
	
	setUniformInt(program, "material.diffuseCount", material->diffuseCount);
	setUniformInt(program, "material.specularCount", material->specularCount);
	setUniformInt(program, "material.emissionCount", material->emissionCount);
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
	object.vertexData = *vertexData;

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
	
	drawVertexData(&object->vertexData, program);
}
