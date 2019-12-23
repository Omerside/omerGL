#version 330 core

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

struct Material {
	float shininess;
	vec3 specular;
	vec3 ambient;
	sampler2D textureMap;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 frag_color;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;


void main()
{
	vec4 texel = texture(material.textureMap, TexCoord);

	//Ambient
	vec3 ambient = light.ambient * material.ambient;

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.position - FragPos);
	float NdotL = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = light.diffuse * vec3(texel) * NdotL;

	//Specular (base)
	float specularFactor = 0.8f;
	float shininess = 100.0f;
	vec3 viewDirection = normalize(viewPos - FragPos);

	//Specular - Blinn-phong components:
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	vec3 specular =  light.specular * material.specular * pow(NdotH, material.shininess);
	

	//Specular -	Phong components:
	//vec3 reflectDirection = reflect(-lightDirection, normal);
	//float RdotV = max(dot(reflectDirection, viewDirection), 0.0f);
	//vec3 specular =  lightColor * specularFactor * pow(RdotV, material.shininess);
	

	//final color
	
	frag_color = vec4(ambient + diffuse + specular, 1.0f) * texel;
	//frag_color = vec4(lightColor, 1.0f);
};