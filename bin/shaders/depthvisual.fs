// visualizes depth buffer

#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 cameraPos;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main(){
	FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1.0);
}