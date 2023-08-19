// display a single texture across the screen (good for loading)

#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;

#define GAMMA 2.2

void main(){
	vec4 final = texture(tex, TexCoords);
	//final.rgb = pow(final.rgb, vec3(1.0/GAMMA));
	FragColor = vec4(final.rgb, 1.0);
}


/*void main()
{             
	float depthValue = texture(tex, TexCoords).r;
	FragColor = vec4(vec3(depthValue), 1.0);
}*/
