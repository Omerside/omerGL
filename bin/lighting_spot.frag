#version 330 core

struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float exponent;
	float cosInnerCone;
	float cosOuterCone;
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

uniform SpotLight light;
uniform Material material;
uniform vec3 viewPos;

vec3 calcSpotLight();

vec4 texel = texture(material.textureMap, TexCoord);

void main()
{
	//Ambient
	vec3 ambient = light.ambient * material.ambient * vec3(texel);

	vec3 spotColor = calcSpotLight();
	
	frag_color = vec4(ambient + spotColor, 1.0f);
	//frag_color = vec4(lightColor, 1.0f);
};

vec3 calcSpotLight(){

	vec3 lightDirection = normalize(light.position - FragPos);
	vec3 spotDirection = normalize(light.direction);

	float cosDirection = dot(-lightDirection, spotDirection);
	float cosIntensity = smoothstep(light.cosOuterCone, light.cosInnerCone, cosDirection);

	//Diffuse
	vec3 normal = normalize(Normal);
	float NdotL = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * NdotL * vec3(texel);


	//Specular - Blinn-phong components:
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	vec3 specular =  light.specular * material.specular * pow(NdotH, material.shininess);

	//final color
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear*distance + light.exponent*(distance*distance));

	diffuse *= attenuation * cosIntensity;
	specular *= attenuation * cosIntensity;

	return (diffuse + specular);

}