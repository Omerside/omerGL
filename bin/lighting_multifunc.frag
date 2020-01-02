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
	sampler2D specularMap;
};


in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 frag_color;

uniform DirectionalLight dirLight;
uniform SpotLight[10] spotLight;
uniform PointLight[10] pointLight;
uniform Material material;
uniform vec3 viewPos;


vec4 CalcDirectionalLight();
vec4 CalcSpotLight();
vec4 CalcPointLight();

vec4 texel = texture(material.textureMap, TexCoord);
vec4 spexel = texture(material.specularMap, TexCoord);

void main()
{
	
	vec4 output = vec4(1.0f);
	output += CalcDirectionalLight();
	output += CalcSpotLight();
	output += CalcPointLight();


	//frag_color = vec4(mAmbient + mDiffuse + mSpecular, 1.0f) * (texel);
	frag_color = output * (texel);
	
};

vec4 CalcDirectionalLight(){
	
	//Ambient
	vec3 mAmbient = dirLight.ambient * material.ambient * vec3(texel);

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(-dirLight.direction);
	float NdotL = max(dot(normal, lightDirection), 0.0f);
	vec3 mDiffuse = dirLight.diffuse * vec3(texel)  * NdotL;

	//Specular (base)
	vec3 viewDirection = normalize(viewPos - FragPos);

	//Specular - Blinn-phong components:
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	vec3 mSpecular = dirLight.specular * material.specular * pow(NdotH, material.shininess);

	return vec4((mAmbient) + (mDiffuse) + (mSpecular * (vec3(spexel))), 1.0f);
	
}

vec4 CalcPointLight(){
	int i = 0;
	vec3 normal = normalize(Normal);
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 mSpecular;
	vec3 mDiffuse;

	while (i < pointLight.length() && pointLight[i].diffuse != vec3(0.0f)){

		//Diffuse
		vec3 lightDirection = normalize(pointLight[i].position - FragPos);
		float NdotL = max(dot(normal, lightDirection), 0.0f);
		vec3 tempDiffuse = pointLight[i].diffuse * NdotL * vec3(texel);


		//Specular - Blinn-phong components:

		vec3 halfDirection = normalize(lightDirection + viewDirection);
		float NdotH = max(dot(normal, halfDirection), 0.0f);
		vec3 tempSpecular = (pointLight[i].specular) * material.specular * pow(NdotH, material.shininess);

		//final color
		float distance = length(pointLight[i].position - FragPos);
		float attenuation = 1.0f / (pointLight[i].constant + pointLight[i].linear*distance + pointLight[i].exponent*(distance*distance));
	
		mDiffuse += tempDiffuse * attenuation;
		mSpecular += tempSpecular * attenuation;

		i++;
		
	}

	return vec4((mDiffuse) + (mSpecular * (vec3(spexel))), 1.0f);
	
}

vec4 CalcSpotLight(){
	int i = 0;
	vec3 mSpecular;
	vec3 mDiffuse;
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 normal = normalize(Normal);

	while (i < spotLight.length() && spotLight[i].diffuse != vec3(0.0f)){

		vec3 lightDirection = normalize(spotLight[i].position - FragPos);
		vec3 spotDirection = normalize(spotLight[i].direction);

		float cosDirection = dot(-lightDirection, spotDirection);
		float cosIntensity = smoothstep(spotLight[i].cosOuterCone, spotLight[i].cosInnerCone, cosDirection);

		//Diffuse

		float NdotL = max(dot(normal, lightDirection), 0.0);
		//vec3 tempDiffuse = (dirLight.diffuse + spotLight[i].diffuse) * NdotL * vec3(texel);
		vec3 tempDiffuse = (spotLight[i].diffuse) * NdotL * vec3(texel);


		//Specular - Blinn-phong components:

		vec3 halfDirection = normalize(lightDirection + viewDirection);
		float NdotH = max(dot(normal, halfDirection), 0.0f);
		vec3 tempSpecular =  ( spotLight[i].specular) * material.specular * pow(NdotH, material.shininess) ;

		//final color
		float distance = length(spotLight[i].position - FragPos);
		float attenuation = 1.0f / (spotLight[i].constant + spotLight[i].linear*distance + spotLight[i].exponent*(distance*distance));

		mDiffuse += tempDiffuse * attenuation * cosIntensity;
		mSpecular += tempSpecular * attenuation * cosIntensity;

		i++;
	}

	return vec4((mDiffuse) + (mSpecular * (vec3(spexel))), 1.0f);

}