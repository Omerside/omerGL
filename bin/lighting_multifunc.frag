#version 330 core

struct DirectionalLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

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

uniform DirectionalLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLight;
uniform Material material;
uniform vec3 viewPos;


void CalcDirectionalLight();
void CalcSpotLight();
void CalcPointLight();

vec4 texel = texture(material.textureMap, TexCoord);
vec3 mAmbient = vec3(0.0f);
vec3 mDiffuse = vec3(0.0f);
vec3 mSpecular = vec3(0.0f);

void main()
{
	
	CalcSpotLight();
	//CalcDirectionalLight();
	CalcPointLight();

	frag_color = vec4(mAmbient + mDiffuse + dirLight.diffuse + mSpecular, 1.0f) * texel;

};

void CalcDirectionalLight(){
	
	//Ambient
	//mAmbient += dirLight.ambient * material.ambient * vec3(texel);

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(-dirLight.direction);
	float NdotL = max(dot(normal, lightDirection), 0.0f);
	mDiffuse += dirLight.diffuse * vec3(texel) * NdotL;

	//Specular (base)
	vec3 viewDirection = normalize(viewPos - FragPos);

	//Specular - Blinn-phong components:
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	mSpecular += dirLight.specular * material.specular * pow(NdotH, material.shininess);
	
}

void CalcPointLight(){

	//Ambient
	mAmbient += (pointLight.ambient + dirLight.ambient) * material.ambient * vec3(texel);

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(pointLight.position - FragPos);
	float NdotL = max(dot(normal, lightDirection), 0.0f);
	mDiffuse += pointLight.diffuse * vec3(texel) * NdotL;


	//Specular - Blinn-phong components:
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	mSpecular +=  (pointLight.specular) * material.specular * pow(NdotH, material.shininess);

	//final color
	float distance = length(pointLight.position - FragPos);
	float attenuation = 1.0f / (pointLight.constant + pointLight.linear*distance + pointLight.exponent*(distance*distance));
	
	mDiffuse *= attenuation;
	//mDiffuse += mDiffuse * attenuation;
	mSpecular += mSpecular * attenuation;
}

void CalcSpotLight(){

	vec3 lightDirection = normalize(spotLight.position - FragPos);
	vec3 spotDirection = normalize(spotLight.direction);

	float cosDirection = dot(-lightDirection, spotDirection);
	float cosIntensity = smoothstep(spotLight.cosOuterCone, spotLight.cosInnerCone, cosDirection);

	//Diffuse
	vec3 normal = normalize(Normal);
	float NdotL = max(dot(normal, lightDirection), 0.0);
	mDiffuse += (dirLight.diffuse + spotLight.diffuse) * NdotL * vec3(texel);


	//Specular - Blinn-phong components:
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	mSpecular +=  ( spotLight.specular) * material.specular * pow(NdotH, material.shininess);

	//final color
	float distance = length(spotLight.position - FragPos);
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear*distance + spotLight.exponent*(distance*distance));

	mDiffuse *= attenuation * cosIntensity;
	//mDiffuse += mDiffuse * attenuation * cosIntensity;
	mSpecular += mSpecular * attenuation * cosIntensity;

}