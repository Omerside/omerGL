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

uniform int pointLightCount;
uniform DirectionalLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLight[10];
uniform Material material;
uniform vec3 viewPos;


vec4 CalcDirectionalLight();
vec4 CalcSpotLight();
vec4 CalcPointLight();

vec4 texel = texture(material.textureMap, TexCoord);

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
	vec3 mAmbient = dirLight.ambient * material.ambient; //* vec3(texel);

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(-dirLight.direction);
	float NdotL = max(dot(normal, lightDirection), 0.0f);
	vec3 mDiffuse = dirLight.diffuse * vec3(texel) * NdotL;

	//Specular (base)
	vec3 viewDirection = normalize(viewPos - FragPos);

	//Specular - Blinn-phong components:
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	vec3 mSpecular = dirLight.specular * material.specular * pow(NdotH, material.shininess);

	return vec4(mAmbient + mDiffuse + mSpecular, 1.0f) * (texel);
	
}

vec4 CalcPointLight(){
	int i = 0;
	vec3 normal = normalize(Normal);
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 mSpecular;
	vec3 mDiffuse;

	while (i < pointLight.length() && pointLight[i].diffuse != vec3(0.0f) ){
		
		//Ambient
		vec3 tempAmbient = (pointLight[i].ambient) * material.ambient * vec3(texel);

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

	return vec4((mDiffuse + mSpecular), 1.0f);
	
}

vec4 CalcSpotLight(){

	vec3 lightDirection = normalize(spotLight.position - FragPos);
	vec3 spotDirection = normalize(spotLight.direction);

	float cosDirection = dot(-lightDirection, spotDirection);
	float cosIntensity = smoothstep(spotLight.cosOuterCone, spotLight.cosInnerCone, cosDirection);

	//Diffuse
	vec3 normal = normalize(Normal);
	float NdotL = max(dot(normal, lightDirection), 0.0);
	vec3 mDiffuse = (dirLight.diffuse + spotLight.diffuse) * NdotL* (vec3(texel));


	//Specular - Blinn-phong components:
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	vec3 mSpecular =  ( spotLight.specular) * material.specular * pow(NdotH, material.shininess);

	//final color
	float distance = length(spotLight.position - FragPos);
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear*distance + spotLight.exponent*(distance*distance));

	mDiffuse *= attenuation * cosIntensity;
	mSpecular *= attenuation * cosIntensity;

	return vec4((mDiffuse + mSpecular), 1.0f);

}