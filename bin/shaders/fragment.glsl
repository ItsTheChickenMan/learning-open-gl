// fragment shader

#version 330 core

out vec4 FragColor;

in vec2 TexCoord; // glory to mother russia

uniform sampler2D tex;

void main(){
	FragColor = texture(tex, TexCoord);
}