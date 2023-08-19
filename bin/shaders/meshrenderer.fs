// lit mesh renderer

#version 330 core

struct Material {
	sampler2D diffuseMaps[8]; // max of 8 textures
	sampler2D specularMaps[8]; // max of 8 specular
	sampler2D emissionMaps[8]; // max of 8 emission
	
	sampler2D normalMap; // max of 1 normal
	
	int diffuseCount;
	int specularCount;
	int emissionCount;
	
	vec3 color;
	
	float shininess;
	float specularStrength;
};

struct DirectionalLight {
	vec3 direction;
	
	vec3 color;
	
	float ambient;
	float diffuse;
	float specular;
	
	bool exists; // when setting a light, set this to true so the shader knows to actually calculate it
};

struct PointLight {
	vec3 position; // position
	
	// attenuation factors
	float constant;
	float linear;
	float quadratic;
	
	vec3 color; // (this is color)
	
	float ambient; // ambient brightness (not color)
	float diffuse; // diffuse brightness (not color)
	float specular; // specular brightness (not color)
	
	bool exists; // when setting a light, set this to true so the shader knows to actually calculate it
};

struct SpotLight {
	vec3 position; // position
	vec3 direction; // direction
	
	float angle; // angle of cone
	float outerAngle; // outer angle of cone
	
	// attenuation factors
	float constant;
	float linear;
	float quadratic;
	
	vec3 color; // (this is color)
	
	float ambient; // ambient brightness (not color)
	float diffuse; // diffuse brightness (not color)
	float specular; // specular brightness (not color)
	
	bool exists; // when setting a light, set this to true so the shader knows to actually calculate it
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform Material material;

// lights
#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 16
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];

uniform vec3 cameraPos;

uniform sampler2D shadowMap;

uniform DirectionalLight shadowCaster;

// quick definitions
vec3 calculateDirectionalLight(DirectionalLight dlight, Material mat);
vec3 calculatePointLight(PointLight plight, Material mat);
vec3 calculateSpotLight(SpotLight slight, Material mat);
float calculateFragInShadow(vec4 fragPosLightSpace);

// misc
uniform float testing;
uniform samplerCube env;
uniform float fun;

#define PI 3.1415926535897
#define GAMMA 2.2

void main(){
	// combine texture samples
	vec4 diffuseSample = vec4(1, 1, 1, 1);
	vec3 specularSample = vec3(1, 1, 1);
	vec3 emissionSample = vec3(0, 0, 0);
	
	if(material.diffuseCount > 0){
		diffuseSample = texture(material.diffuseMaps[0], TexCoords);
		//diffuseSample = pow(texture(material.diffuseMaps[0], TexCoords), vec4(vec3(GAMMA), 1.0));
		
		for(int i = 1; i < material.diffuseCount; i++){
			diffuseSample = mix(diffuseSample, texture(material.diffuseMaps[i], TexCoords), 0.5);
		}
	}
	
	if(material.specularCount > 0){
		specularSample = vec3(texture(material.specularMaps[0], TexCoords));
		
		for(int i = 1; i < material.specularCount; i++){
			specularSample += vec3(texture(material.specularMaps[i], TexCoords));
		}
	}
	
	if(material.emissionCount > 0){
		emissionSample = vec3(texture(material.emissionMaps[0], TexCoords));
		
		for(int i = 0; i < material.emissionCount; i++){
			emissionSample += vec3(texture(material.emissionMaps[i], TexCoords));
		}	
	}
	
	// final values
	vec3 ambient = 	vec3(0.0f, 0.0f, 0.0f);
	vec3 diffuse = 	vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	
	// loop through each light
	for(int i = 0; i < MAX_POINT_LIGHTS; i++){
		//vec3 lightingFactors = calculatePointLight(pointLights[i], material);
		PointLight l = pointLights[i];
		
		// for now, this is a cheap hack to test if the light actually exists (if not then ambient should be undefined and this if will progress)
		if(l.exists){
			vec3 lightingFactors = calculatePointLight(l, material);
			
			ambient += material.color * vec3(diffuseSample) * lightingFactors.x * l.color * l.ambient;
			diffuse += material.color * vec3(diffuseSample) * lightingFactors.y * l.color * l.diffuse;
			specular += material.specularStrength * specularSample * lightingFactors.z * l.color * l.specular;
		} else {
			break;
		}
	}
	
	for(int i = 0; i < MAX_SPOT_LIGHTS; i++){
		SpotLight l = spotLights[i];
		
		if(l.exists){	
			vec3 lightingFactors = calculateSpotLight(l, material);
			
			ambient += material.color * vec3(diffuseSample) * lightingFactors.x * l.color * l.ambient;
			diffuse += material.color * vec3(diffuseSample) * lightingFactors.y * l.color * l.diffuse;
			specular += material.specularStrength * specularSample * lightingFactors.z * l.color * l.specular;
		} else {
			break;
		}
	}
	
	for(int i = 0; i < MAX_DIRECTIONAL_LIGHTS; i++){
		DirectionalLight l = directionalLights[i];
		
		if(l.exists){
			vec3 lightingFactors = calculateDirectionalLight(directionalLights[i], material);
			
			ambient += material.color * vec3(diffuseSample) * lightingFactors.x * l.color * l.ambient;
			diffuse += material.color * vec3(diffuseSample) * lightingFactors.y * l.color * l.diffuse;
			specular += material.specularStrength * specularSample * lightingFactors.z * l.color * l.specular;
		} else {
			break;
		}
	}
	
	/*if(testing == 1.0f){
		float ratio = 1.00 / 1.33;

		vec3 I = normalize(FragPos - cameraPos);
		vec3 R = reflect(I, normalize(Normal));
		
		FragColor = vec4(texture(env, R).rgb, 1.0);
		return;
	}*/
	
	// final color
	float shadow = calculateFragInShadow(FragPosLightSpace);
	vec3 final = ambient + (1.0 - shadow) * (diffuse + specular) + emissionSample;
	final.rgb = pow(final, vec3(1.0/GAMMA));
	FragColor = vec4(final, diffuseSample.w);
}

// return a vec3 containing the ambient, diffuse and specular lighting factors (ambient, diffuse, specular)
vec3 calculateDirectionalLight(DirectionalLight dlight, Material mat){
	// ambient
	float ambientFactor = 1.0f; // ambience unchanged
	
	// diffuse
	vec3 normalNormal = normalize(Normal);
	vec3 lightDir = normalize(-dlight.direction);
	float diffuseFactor = max(dot(normalNormal, lightDir), 0.0);
	
	// specular
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normalNormal);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	
	return vec3(ambientFactor, diffuseFactor, specularFactor);
}

vec3 calculatePointLight(PointLight plight, Material mat){
	float ambientFactor = 1.0f; // ambience unchanged
	
	// diffuse
	vec3 normalNormal = normalize(Normal);
	vec3 lightDir = normalize(plight.position - FragPos);
	float diffuseFactor = max(dot(normalNormal, lightDir), 0.0);
	
	// specular
	/*vec3 viewDir = normalize(cameraPos - FragPos); // view space calculation
	vec3 reflectDir = reflect(-lightDir, normalNormal);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	*/
	// blinn phong experiment
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 halfway = normalize(lightDir + viewDir);
	
	float specularFactor = pow(max(dot(halfway, normalNormal), 0.0), mat.shininess);
	
	
	float dist = length(plight.position - FragPos);
	float attenuation = 1.0 / (plight.constant + plight.linear*dist + plight.quadratic*dist*dist);

	return vec3(ambientFactor*attenuation, diffuseFactor*attenuation, specularFactor*attenuation);	
}

vec3 calculateSpotLight(SpotLight slight, Material mat){
	float ambientFactor = 1.0f; // ambience unchanged
	
	// spotlight calculation
	vec3 lightDir = normalize(slight.position - FragPos);
	vec3 spotlightDir = normalize(-slight.direction);
	float angle = acos(dot(lightDir, spotlightDir));
	float epsilon = slight.angle - slight.outerAngle;
	float spotlightFactor = smoothstep(0.0, 1.0, (angle-slight.outerAngle)/epsilon);
	// clamp((angle-slight.outerAngle)/epsilon, 0.0, 1.0)
	
	// diffuse
	vec3 normalNormal = normalize(Normal); // I really like this variable name
	float diffuseFactor = max(dot(normalNormal, lightDir), 0.0); // get angle between normal and light direction
	
	// specular
	vec3 viewDir = normalize(cameraPos - FragPos); // view space calculation
	vec3 reflectDir = reflect(-lightDir, normalNormal);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	
	float dist = length(slight.position - FragPos);
	float attenuation = 1.0 / (slight.constant + slight.linear*dist + slight.quadratic*dist*dist);
	
	return vec3(ambientFactor*attenuation, diffuseFactor*spotlightFactor*attenuation, specularFactor*spotlightFactor*attenuation);
}

float calculateFragInShadow(vec4 fragPosLightSpace){
	// perform perspective divide (not done automatically because not passed through gl_Position)
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	projCoords = projCoords * 0.5 + 0.5;
	
	if(projCoords.z > 1.0)
		return 0.0;

	vec3 lightDir = normalize(shadowCaster.direction);
	
	float closestDepth = texture(shadowMap, projCoords.xy).r;

	float currentDepth = projCoords.z;
	
	//float bias = max(0.05 * (1.0 - dot(normalize(Normal), lightDir)), 0.005);
	float bias = 0.0;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(float x = -2; x <= 2; x++)
	{
			for(float y = -2; y <= 2; y++)
			{
					float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
					shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}    
	}
	shadow /= 25.0;

	// if the currentDepth is greater (therefore further) than the closest depth, then the fragment is in shadow)
	return shadow;
}