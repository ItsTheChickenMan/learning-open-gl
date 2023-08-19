// we only care about object transformations with shadow mapping

#version 330 core

layout (location = 0) in vec3 vPos;

uniform mat4 model;

uniform mat4 lightSpace;

void main(){
	// translate according to matrices
	gl_Position = lightSpace * model * vec4(vPos, 1.0);
}