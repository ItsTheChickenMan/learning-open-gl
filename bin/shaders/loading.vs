// display a single texture across the vertex data, without  (good for loading when used with quads)

#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;

out vec2 TexCoords;

void main(){
		gl_Position = vec4(vPos.xy, 1.0, 1.0);
		TexCoords = vTexCoord;
}