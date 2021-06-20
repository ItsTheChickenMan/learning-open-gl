// renders shapes without a regard for texture

#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main(){
	// translate according to matrices
	gl_Position = projection * view * model * vec4(vPos, 1.0);

	FragPos = vec3(model * vec4(vPos, 1.0));
	
	Normal = normalMatrix * vNormal;
	TexCoords = vTexCoord;
}