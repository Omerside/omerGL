#version 330 core

struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float exponent;
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

uniform PointLight light;
uniform Material material;
uniform vec3 viewPos;


void main()
{
	vec4 texel = texture(material.textureMap, TexCoord);

	//Ambient
	vec3 ambient = light.ambient * material.ambient * vec3(texel);

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.position - FragPos);
	float NdotL = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = light.diffuse * vec3(texel) * NdotL;


	//Specular - Blinn-phong components:
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	vec3 specular =  light.specular * material.specular * pow(NdotH, material.shininess);

	//final color
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear*distance + light.exponent*(distance*distance));

	diffuse *= attenuation;
	specular *= attenuation;
	
	frag_color = vec4(ambient + diffuse + specular, 1.0f) * texel;
	//frag_color = vec4(lightColor, 1.0f);
};